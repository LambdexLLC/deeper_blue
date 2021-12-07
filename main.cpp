#include "chess/chess.hpp"

#include "chess/engines/random_engine.hpp"
#include "chess/engines/baby_engine.hpp"
#include "chess/engines/neural_engine.hpp"

#include "utility/io.hpp"
#include "utility/json.hpp"
#include "utility/http.hpp"

#include "api/env.hpp"
#include "api/api.hpp"

#include <jclib/thread.h>
#include <jclib/timer.h>

#include <random>
#include <ranges>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <iostream>

#include <thread>

using namespace lbx;

namespace lbx::chess
{
	inline chess::BoardWithState recreate_board_from_move_string(const std::string& _movesString,
		chess::BoardWithState _initialBoardState = chess::make_standard_board())
	{
		auto& _board = _initialBoardState;

		// Parse moves string
		std::vector<chess::Move> _moves{};
		std::array<char, 8> _buffer{ '\0' };
		for (auto _moveStr : _movesString | std::views::split(' '))
		{
			std::ranges::copy(_moveStr, _buffer.begin());
			const auto _bufferLen = std::ranges::distance(_moveStr);

			chess::Move _move{};
			chess::from_chars(_buffer.data(), _buffer.data() + _bufferLen, _move);
			_moves.push_back(_move);
		};

		// Determine whose move it is
		if ((_moves.size() % 2) == 0)
		{
			// Current turn is white
			_board.turn = chess::Color::white;
		}
		else
		{
			// Current turn is black
			_board.turn = chess::Color::black;
		};

		// Apply moves to board
		bool _isBlacksTurn = false;
		for (auto& _move : _moves)
		{
			chess::Color _player = (_isBlacksTurn) ? chess::Color::black : chess::Color::white;
			chess::apply_move(_board, _move, _player);
			_isBlacksTurn = !_isBlacksTurn;
		};

		return _board;
	};
};

/**
 * @brief Interface implementation for interacting with a single chess game
*/
struct GameAPI final : public lbx::api::LichessGameAPI
{
private:

	void log_failed_my_move(const std::string& _errorMessage)
	{
		this->error_log_file_ << "Bot failed to move = " << _errorMessage << std::endl;
	};
	void log_failed_opponent_move(const std::string& _errorMessage)
	{
		this->error_log_file_ << "Opponent move failed move validation = " << _errorMessage << std::endl;
	};

	std::ofstream error_log_file_{ SOURCE_ROOT "/errlog.txt" };
	chess::BoardWithState board_{};
	chess::Color my_color_ = chess::Color::white;
	bool is_my_turn_ = false;

	void process_my_turn()
	{
		JCLIB_ASSERT(this->is_my_turn_);
		const auto _moves = this->engine_->calculate_multiple_moves(this->board_, this->my_color_);
		for (auto& m : _moves)
		{
			std::string _errmsg{};
			if (this->submit_move(m, &_errmsg))
			{
				this->is_my_turn_ = false;
				break;
			}
			else
			{
				this->log_failed_my_move(_errmsg);
			};
		};
	};

	void recreate_board_from_move_string(const std::string& _movesString, chess::BoardWithState _initialBoardState = chess::make_standard_board())
	{
		// Create new board
		this->board_ = _initialBoardState;

		// Parse moves string
		std::vector<chess::Move> _moves{};
		std::array<char, 8> _buffer{ '\0' };
		for (auto _moveStr : _movesString | std::views::split(' '))
		{
			std::ranges::copy(_moveStr, _buffer.begin());
			const auto _bufferLen = std::ranges::distance(_moveStr);

			chess::Move _move{};
			chess::from_chars(_buffer.data(), _buffer.data() + _bufferLen, _move);
			_moves.push_back(_move);
		};

		// Determine whose move it is
		if ((_moves.size() % 2) == 0)
		{
			// Current turn is white
			this->is_my_turn_ = (this->my_color_ == chess::Color::white);
		}
		else
		{
			// Current turn is black
			this->is_my_turn_ = (this->my_color_ == chess::Color::black);
		};

		// Apply moves to board
		bool _isBlacksTurn = false;
		for (auto& _move : _moves)
		{
			chess::Color _player = (_isBlacksTurn) ? chess::Color::black : chess::Color::white;
			if (const auto v = chess::is_move_valid(this->board_, _move, _player); v != chess::MoveValidity::valid)
			{
				this->log_failed_opponent_move(_move.to_string() + " " + std::to_string((int)v));
			};
			chess::apply_move(this->board_, _move, _player);
			_isBlacksTurn = !_isBlacksTurn;
		};
		
	};

public:

	/**
	 * @brief Invoked initially upon loading a game
	 * See https://lichess.org/api#operation/botGameStream
	*/
	void on_game(const lbx::json& _event) final
	{
		// Determine my color
		const auto _dmp = _event.dump(1, '\t', true);
		println("{}", _dmp);

		if (const auto _whiteJson = _event.at("white");
			_whiteJson.is_object() &&
			_whiteJson.contains("id") &&
			_whiteJson.at("id") == "lambdex")
		{
			this->my_color_ = chess::Color::white;
		}
		else if (const auto _blackJson = _event.at("black");
			_blackJson.is_object() &&
			_blackJson.contains("id") &&
			_blackJson.at("id") == "lambdex")
		{
			this->my_color_ = chess::Color::black;
		}
		else
		{
			// Failed to determine what my color is.
			JCLIB_ABORT();
		};
		
		// Recreate board state
		this->recreate_board_from_move_string(_event.at("state").at("moves"));

		// Dump board string
		println("{}", this->board_);

		// If it is our turn to play, make the move and submit
		if (this->is_my_turn_)
		{
			this->process_my_turn();
		};
	};

	/**
	 * @brief Invoked when a move is played, a draw is offered,
	 * or the game ends.
	*/
	void on_game_change(const lbx::json& _event) final
	{
		// Check that this was a move
		if (_event.at("status") != "started")
		{
			// Not a move
			return;
		}
		else
		{
			// Opponent made a move, now its our turn

			// Recreate board from moves
			this->recreate_board_from_move_string(_event.at("moves"));
			
			// Process turn if it is our turn
			if (this->is_my_turn_)
			{
				this->process_my_turn();
			};
		};
	};

	/**
	 * @brief Invoked when a chat message is sent
	*/
	void on_chat(const lbx::json& _event) final
	{
	
	};


	GameAPI(const std::shared_ptr<chess::IChessEngine>& _engine) :
		engine_{ _engine }
	{};

private:
	std::shared_ptr<chess::IChessEngine> engine_{};
};

struct AccountAPI final : public lbx::api::LichessAccountAPI
{
public:

	std::vector<std::unique_ptr<lbx::api::LichessGameAPI>> games_{};


	/**
	 * @brief Invoked when a player challenges you
	*/
	void on_challenge(const lbx::json& _event) final
	{
		const std::string_view _challengeID = _event.at("challenge").at("id");

		// Of course we accept it
		this->accept_challenge(_challengeID);
	};

	/**
	 * @brief Invoked when a game is started
	 * https://lichess.org/api#operation/apiStreamEvent
	*/
	void on_game_start(const lbx::json& _event) final
	{
		lbx::println("game was started");
		const std::string _gameID = _event.at("game").at("id");
		this->games_.push_back(jc::make_unique<GameAPI>(std::shared_ptr<chess::IChessEngine>
			(
				new chess::ChessEngine_Baby{}
			)));

		api::set_game_api(_gameID, this->games_.back().get());
	};

	/**
	 * @brief Invoked when a game finishes
	*/
	void on_game_finish(const lbx::json& _event) final
	{
		lbx::println("game was finished");
	};

	AccountAPI()
	{
		// Create a game API for each of the current games
		const auto _games = this->get_current_games();
		lbx::println("Currently playing {} games", _games.size());
		for (auto& _game : _games)
		{
			this->games_.push_back(jc::make_unique<GameAPI>(std::shared_ptr<chess::IChessEngine>
				(
					new chess::ChessEngine_Baby{}
			)));
			api::set_game_api(_game, this->games_.back().get());
		};

		if (_games.empty())
		{
			this->challenge_ai(2);
		};
	};

};

int main()
{
	using namespace lbx;

	api::set_env_folder_path(SOURCE_ROOT "/env");
	if (!api::load_env())
	{
		return 1;
	};

	AccountAPI _accountAPI{};
	api::set_account_api(&_accountAPI);
	
	while (true)
	{
		api::forward_events();
		jc::sleep(0.1f);
	};

	return 0;
};
