#pragma once

/*
	Contains all of the functionality needed to interact with the remote lichess chess API
*/

#include "utility/json.hpp"

#include <lambdex/chess/move.hpp>

#include <jclib/memory.h>

#include <string>
#include <vector>

namespace lbx::api
{
	/**
	 * @brief Forwards lichess events to their associated APIs.
	*/
	void forward_events();

	// Forward decl for account API
	class LichessGameAPI;

	/**
	 * @brief Abstract base class for recieving and reacting to
	 * lichess account events.
	*/
	class LichessAccountAPI
	{
	protected:

		/**
		 * @brief Accepts an incoming challenge from another player
		*/
		virtual bool accept_challenge(std::string_view _challengeID) final;

		/**
		 * @brief Gets the games we are currently playing in
		 * @return The list of game IDs
		*/
		virtual std::vector<std::string> get_current_games() final;

		/**
		 * @brief Creates a challenge against another user
		 * @param _username Username of the user to challenge
		*/
		virtual bool challenge_user(std::string_view _username) final;

		/**
		 * @brief Challenges the AI to a game
		 * @param _level Bot level, must be 1 up to 8
		 * @return True on good challenge, false otherwise
		*/
		virtual bool challenge_ai(int _level) final;

	public:

		/**
		 * @brief Invoked when a game is started
		*/
		virtual void on_game_start(const json& _event) {};

		/**
		 * @brief Invoked when a game finishes
		*/
		virtual void on_game_finish(const json& _event) {};

		/**
		 * @brief Invoked when a player challenges you
		*/
		virtual void on_challenge(const json& _event) {};

		/**
		 * @brief Invoked when a player cancels their challenge to you
		*/
		virtual void on_challenge_canceled(const json& _event) {};

		/**
		 * @brief Invoked when a challenge you created was declined
		*/
		virtual void on_challenge_declined(const json& _event) {};

	};

	/**
	 * @brief Sets the lichess account api interface
	 * @param _api Borrowing pointer to to an account API interface object
	*/
	void set_account_api(jc::borrow_ptr<LichessAccountAPI> _api);

	/**
	 * @brief Abstract base class for recieving and reacting to
	 * lichess game events.
	*/
	class LichessGameAPI
	{
	protected:

		/**
		 * @brief Submits this as our move for the turn
		 * @param _move Move to submit
		 * @param _errmsg Optional error message output string, defaults to nullptr
		 * @return True if move was valid, false otherwise
		*/
		virtual bool submit_move(const chess::Move& _move, std::string* _errmsg = nullptr) final;

		/**
		 * @brief Resigns from the game
		 * @return True on good resign, false otherwise
		*/
		virtual bool resign() final;

	public:

		/**
		 * @brief Invoked initially upon loading a game
		*/
		virtual void on_game(const json& _event) {};

		/**
		 * @brief Invoked when a move is played, a draw is offered,
		 * or the game ends.
		*/
		virtual void on_game_change(const json& _event) {};

		/**
		 * @brief Invoked when a chat message is sent
		*/
		virtual void on_chat(const json& _event) {};

	};

	/**
	 * @brief Sets the game API to use for handling events from a particular game
	 * @param _gameID ID of the game to set the API of
	 * @param _api API to use
	*/
	void set_game_api(std::string_view _gameID, jc::borrow_ptr<LichessGameAPI> _api);
};
