#pragma once

/*
	Contains the "glue" code that allows the lichess API
	to interact with the chess engines
*/

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

namespace lbx
{

	/**
	 * @brief Interface implementation for interacting with a single chess game
	*/
	struct GameAPI final : public lbx::api::LichessGameAPI
	{
	private:

		/**
		 * @brief Interface passed to the game engine
		*/
		class Interface : public lbx::chess::IGameInterface
		{
		public:

			/**
			 * @brief Resigns from the game
			*/
			void resign() final
			{
				if (!this->api_->resign())
				{
					JCLIB_ABORT();
				};
			};

			/**
			 * @brief Offers a draw
			 * @return True if the draw was accepted, false otherwise
			*/
			bool offer_draw() final
			{
				return false;
			};

			/**
			 * @brief Submits a move for the player
			 * @return True if the move was valid, false otherwise
			*/
			bool submit_move(lbx::chess::Move _move) final
			{
				JCLIB_ASSERT(this->api_->is_my_turn_);

				std::string _errmsg{};
				if (this->api_->submit_move(_move, &_errmsg))
				{
					this->api_->is_my_turn_ = false;
					return true;
				}
				else
				{
					this->api_->log_failed_my_move(_errmsg);
					return false;
				};
			};

			/**
			 * @brief Gets the current chess board
			 * @return Chess board with state
			*/
			lbx::chess::BoardWithState get_board() final
			{
				return this->api_->board_;
			};

			/**
			 * @brief Gets the color for the engine
			 * @return Color of the engine
			*/
			lbx::chess::Color get_color() final
			{
				return this->api_->my_color_;
			};


			Interface(GameAPI* _api) :
				api_{ _api }
			{};

			GameAPI* api_;
		};

		// For now we will give it full access, this should be removed
		// as functionality is moved into it
		friend Interface;


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
			Interface _interface{ this };
			this->engine_->play_turn(_interface);
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


	/**
	 * @brief Interface implementation for interacting with a single chess game
	*/
	struct Neuron_GameAPI final : public lbx::api::LichessGameAPI
	{
	private:

		/**
		 * @brief Interface passed to the game engine
		*/
		class Interface : public lbx::chess::IGameInterface
		{
		public:

			/**
			 * @brief Resigns from the game
			*/
			void resign() final
			{
				if (!this->api_->resign())
				{
					JCLIB_ABORT();
				};
			};

			/**
			 * @brief Offers a draw
			 * @return True if the draw was accepted, false otherwise
			*/
			bool offer_draw() final
			{
				return false;
			};

			/**
			 * @brief Submits a move for the player
			 * @return True if the move was valid, false otherwise
			*/
			bool submit_move(lbx::chess::Move _move) final
			{
				JCLIB_ASSERT(this->api_->is_my_turn_);

				std::string _errmsg{};
				if (this->api_->submit_move(_move, &_errmsg))
				{
					this->api_->is_my_turn_ = false;
					return true;
				}
				else
				{
					this->api_->log_failed_my_move(_errmsg);
					this->mutate_ = true;
					return true;
				};
			};

			/**
			 * @brief Gets the current chess board
			 * @return Chess board with state
			*/
			lbx::chess::BoardWithState get_board() final
			{
				return this->api_->board_;
			};

			/**
			 * @brief Gets the color for the engine
			 * @return Color of the engine
			*/
			lbx::chess::Color get_color() final
			{
				return this->api_->my_color_;
			};


			Interface(Neuron_GameAPI* _api) :
				api_{ _api }
			{};

			Neuron_GameAPI* api_;
			bool mutate_ = false;
		};

		// For now we will give it full access, this should be removed
		// as functionality is moved into it
		friend Interface;

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
			Interface _interface{ this };
			while (true)
			{
				this->engine_.play_turn(_interface);
				if (_interface.mutate_)
				{
					_interface.mutate_;
					this->engine_.mutate();
				}
				else
				{
					break;
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


		Neuron_GameAPI() :
			engine_{}
		{
			std::ifstream _file{ SOURCE_ROOT "/nnet_dump/best.txt" };
			std::vector<int> _genes{};
			while (_file)
			{
				int n = 0;
				_file >> n;
				_genes.push_back(n);
				_file.ignore();
			};

			this->engine_.genes_ = _genes;
			this->engine_.sync_genes();
		};

	private:
		chess::ChessEngine_Neural engine_{};
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
			const std::string _gameID = _event.at("game").at("id");
#if true
			this->games_.push_back(jc::make_unique<GameAPI>(std::shared_ptr<chess::IChessEngine>
				(
					new chess::ChessEngine_Baby{}
			)));
#else
			this->games_.push_back(jc::make_unique<Neuron_GameAPI>());
#endif
			api::set_game_api(_gameID, this->games_.back().get());
		};

		/**
		 * @brief Invoked when a game finishes
		*/
		void on_game_finish(const lbx::json& _event) final
		{
		};

		AccountAPI()
		{
			// Create a game API for each of the current games
			const auto _games = this->get_current_games();
			for (auto& _game : _games)
			{
#if true
				this->games_.push_back(jc::make_unique<GameAPI>(std::shared_ptr<chess::IChessEngine>
					(
						new chess::ChessEngine_Baby{}
				)));
#else
				this->games_.push_back(jc::make_unique<Neuron_GameAPI>());
#endif

				api::set_game_api(_game, this->games_.back().get());
			};

			if (_games.empty())
			{
				this->challenge_ai(3);
			};
		};

	};

};