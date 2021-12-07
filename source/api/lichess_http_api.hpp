#pragma once

/*
	Helper functions for interacting with the
	https://lichess.org api.

	See https://lichess.org/arg for docs.
*/

#include "lichess.hpp"

#include "utility/http.hpp"

#include <jclib/maybe.h>

#include <vector>
#include <string>
#include <string_view>

namespace lbx::api::lichess
{
	// Returns an ok boolean, or an error string
	jc::maybe<bool, std::string> accept_challenge(http::Client& _client, std::string_view _challengeID);

	// Returns vector of game ID strings
	// https://lichess.org/api#operation/apiAccountPlaying
	std::vector<std::string> get_current_games(http::Client& _client);

	// Returns a true or an error string on failure
	// https://lichess.org/api#operation/challengeCreate
	jc::maybe<bool, std::string> challenge_user(http::Client& _client, std::string_view _username);

	// Returns a true or an error string on failure
	// https://lichess.org/api#operation/boardGameMove
	jc::maybe<bool, std::string> send_move(http::Client& _client, std::string_view _gameID, std::string_view _move);

	// Challenges the AI to a game
	// https://lichess.org/api#operation/challengeAi
	jc::maybe<bool, std::string> challenge_ai(http::Client& _client, int _level);

	// Resigns from the game
	// https://lichess.org/api#operation/botGameResign
	jc::maybe<bool, std::string> resign_game(http::Client& _client, std::string_view _gameID);

};
