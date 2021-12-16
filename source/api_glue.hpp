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
	class IAPIDebugLogger
	{
	public:
		
		/**
		 * @brief Invoked when a move sent by lichess fails the is_move_valid() check
		*/
		virtual void on_lichess_move_validation_failure() {};

	protected:
		~IAPIDebugLogger() = default;
	};

	/**
	 * @brief Parses a string containing a series of moves into actual moves
	 * @param _movesString String containing series of moves
	 * @return Vector of moves
	*/
	inline std::vector<Move> parse_move_string(const std::string_view _movesString)
	{
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

		return _moves;
	};

	/**
	 * @brief Recreates a chess board state from a move string
	 * @param _movesString String containing a series of moves
	 * @param _initialBoardState The initial board state before applying any of the moves, defaults to standard board
	 * @return Resultant chess board after applying moves
	*/
	inline chess::BoardWithState recreate_board_from_move_string(const std::string& _movesString,
		chess::BoardWithState _initialBoardState = chess::make_standard_board())
	{
		auto& _board = _initialBoardState;
		
		// Parse the moves string
		auto _moves = parse_move_string(_movesString);

		// Apply moves to board
		for (auto& _move : _moves)
		{
			chess::apply_move(_board, _move);
		};
		
		// Return final board
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
				this->api_->resign();
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
				JCLIB_ASSERT(this->api_->is_my_turn());

				std::string _errmsg{};
				if (this->api_->submit_move(_move, &_errmsg))
				{
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

			/**
			 * @brief Optional method allowing the interface to provide the name of the game
			 *
			 * This is mostly for logging purposes.
			 *
			 * @return The name of the game, or an empty string if this is unimplemented (default behavior).
			*/
			std::string get_game_name() final
			{
				return format("game_{}", this->api_->game_id_);
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


		/**
		 * @brief Sets the held board to a board calculated after applying a series of moves to it
		*/
		void recreate_board_from_move_string(const std::string& _movesString)
		{
			this->board_ = chess::recreate_board_from_move_string(_movesString);
		};

		/**
		 * @brief Checks if it is this object's turn to play a move
		*/
		bool is_my_turn() const noexcept
		{
			return this->board_.turn == this->my_color_;
		};

		void process_my_turn()
		{
			JCLIB_ASSERT(this->is_my_turn());
			Interface _interface{ this };
			this->engine_->play_turn(_interface);
		};

	public:

		/**
		 * @brief Invoked initially upon loading a game
		 * See https://lichess.org/api#operation/botGameStream
		*/
		void on_game(const lbx::json& _event) final
		{
			const fs::path _moveStringsFilePath = SOURCE_ROOT "/dump/move_strings.txt";
			
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
			if (this->is_my_turn())
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
				if (this->is_my_turn())
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

		/**
		 * @brief Creates the game API and assigns an engine to it to manage
		 * @param _gameID The ID of the game this is managing for
		 * @param _engine Engine to manage
		*/
		GameAPI(const std::string& _gameID, std::unique_ptr<chess::IChessEngine> _engine) :
			game_id_{ _gameID }, engine_{ std::move(_engine) }
		{};

	private:

		/**
		 * @brief The ID of the game this API is managing fors
		*/
		std::string game_id_;
		
		/**
		 * @brief The chess engine this is managing
		*/
		std::unique_ptr<chess::IChessEngine> engine_{};

	};

	/**
	 * @brief Implementation for the AccountAPI interface
	*/
	struct AccountAPI final : public lbx::api::LichessAccountAPI
	{
	protected:

		// Called when the object is initialized but no games are being played
		void on_no_current_games();

		/**
		 * @brief Assigns a chess engine to a game
		 * @param _gameID ID of the game to assign the engine to
		 * @param _engine Engine to assign to the game
		*/
		void assign_to_game(const std::string& _gameID, std::unique_ptr<chess::IChessEngine> _engine);

	public:

		/**
		 * @brief Invoked when a player challenges you
		*/
		void on_challenge(const lbx::json& _event) final;
		
		/**
		 * @brief Invoked when a game is started
		*/
		void on_game_start(const lbx::json& _event) final;

		/**
		 * @brief Invoked when a game finishes
		*/
		void on_game_finish(const lbx::json& _event) final;

		AccountAPI();

	private:

		/**
		 * @brief Container for tracking the active set of games
		*/
		std::vector<std::unique_ptr<lbx::api::LichessGameAPI>> games_{};

	};

};