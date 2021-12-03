#pragma once

#include "api/lichess.hpp"

#include "utility/json.hpp"
#include "utility/http.hpp"

#include <vector>
#include <string>
#include <string_view>

namespace lbx::api
{
	/**
	 * @brief Client data storage
	*/
	struct LichessClient
	{
	public:


		std::vector<LichessChallenge> list_challenges();

		bool accept_challenge(const std::string_view _challengeID);
		bool accept_challenge(const LichessChallenge& _challenge);
		
		/*
			"fen": "rnbqkbnr/pp3pp1/8/3p3p/1PpPp3/P1P1P3/5PPP/RNBQKBNR",
			"color": "white",
			"lastMove": "c5c4",
			"variant": 
			{},
			"speed": "correspondence",
			"perf": "correspondence",
			"rated": true,
			"opponent": 
			{},
			"isMyTurn": true
		*/

		std::vector<LichessGame> list_games();

		bool send_move(std::string_view _gameID, std::string_view _move);
		bool write_chat(std::string_view _gameID, std::string_view _room, std::string_view _text);

		struct ChallengeParams
		{
			bool rated = false;
			
			bool correspondence = true;
			int seconds_per_move = 0;
			int seconds_added_per_move = 0;
			int days_per_move = 1;

			std::string variant = "standard";
			std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
			std::string message{};
			std::string color = "random";
		};

		bool create_challenge(std::string_view _username, const ChallengeParams& _params, LichessGame& _game);
		bool create_challenge(std::string_view _username, const ChallengeParams& _params, LichessChallenge& _challenge);


		/**
		 * @brief Checks if the client is still alive
		 * @return True if alive, false otherwise
		*/
		bool good() const
		{
			return this->http_client_.is_valid();
		};

		/**
		 * @brief Follows another player
		 * @param _playerName Name of the player to follow
		 * @return True on good follow, false otherwise
		*/
		bool follow_player(std::string_view _playerName);

		void test();



		/**
		 * @brief Constructs the client by connecting to the lichess URL. ENV MUST BE LOADED (api::load_env())
		 * @param _lichessURL Lichess website URL
		*/
		explicit LichessClient(const std::string& _lichessURL);

	private:

		/**
		 * @brief The http client object
		*/
		http::Client http_client_;

	};

};