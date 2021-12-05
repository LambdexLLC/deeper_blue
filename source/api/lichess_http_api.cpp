#include "lichess_http_api.hpp"

#include "utility/io.hpp"
#include "utility/json.hpp"

namespace lbx::api::lichess
{
	jc::maybe<bool, std::string> accept_challenge(http::Client& _client, std::string_view _challengeID)
	{
		const auto _path = std::format("/api/challenge/{}/accept", _challengeID);
		const auto _result = _client.Post(_path.c_str());
		if (_result)
		{
			auto _json = json::parse(_result->body);
			if (_json.contains("ok"))
			{
				return (bool)_json.at("ok");
			}
			else if (_json.contains("error"))
			{
				return (std::string)_json.at("error");
			}
			else
			{
				JCLIB_ABORT();
				return std::string{};
			};
		}
		else
		{
			JCLIB_ABORT();
			return std::string{};
		};
	};

	// Returns vector of game ID strings
	// https://lichess.org/api#operation/apiAccountPlaying
	std::vector<std::string> get_current_games(http::Client& _client)
	{
		constexpr auto _path = "/api/account/playing";
		const auto _result = _client.Get(_path);
		if (_result)
		{
			auto _response = json::parse(_result->body, nullptr, false);
			if (_response.is_discarded())
			{
				JCLIB_ABORT();
				return {};
			};

			std::vector<std::string> _out{};
			for (auto& _game : _response.at("nowPlaying"))
			{
				_out.push_back((std::string)_game.at("gameId"));
			};
			return _out;
		}
		else
		{
			JCLIB_ABORT();
			return {};
		};
	};

	// Returns a true or an error string on failure
	// https://lichess.org/api#operation/challengeCreate
	jc::maybe<bool, std::string> challenge_user(http::Client& _client, std::string_view _username)
	{
		const auto _path = format("/api/challenge/{}", _username);
		http::Params _params
		{
			{ "rated", "false" },
			{ "days",  "1" },
			{ "color", "random" },
			{ "fen",  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" },
			{ "message", "Your game with {opponent} is ready: {game}." }
		};
		const auto _result = _client.Post(_path.c_str(), _params);
		if (_result)
		{
			if (_result->status == 200)
			{
				const auto _response = json::parse(_result->body, nullptr, false);
				if (_response.is_discarded())
				{
					JCLIB_ABORT();
					return std::string{};
				};

				return true;
			}
			else if (_result->status == 400)
			{
				const auto _response = json::parse(_result->body, nullptr, false);
				if (_response.is_discarded())
				{
					JCLIB_ABORT();
					return std::string{};
				};

				const std::string _error = _response.at("error");
				return _error;
			}
			else
			{
				JCLIB_ABORT();
				return std::string{};
			};
		}
		else
		{
			JCLIB_ABORT();
			return std::string{};
		};
	};

	// Returns a true or an error string on failure
	// https://lichess.org/api#operation/boardGameMove
	jc::maybe<bool, std::string> send_move(http::Client& _client, std::string_view _gameID, std::string_view _move)
	{
		const auto _path = std::format("/api/bot/game/{}/move/{}", _gameID, _move);
		http::Params _params{};
		const auto _result = _client.Post(_path.c_str(), _params);

		if (_result)
		{
			if (_result->status == 200)
			{
				// Succesfull move
				return true;
			}
			else if (_result->status == 400)
			{
				// Bad move
				const auto _response = json::parse(_result->body, nullptr, false);
				if (_response.is_discarded())
				{
					JCLIB_ABORT();
					return std::string{};
				};

				// Get error message
				return (std::string)_response.at("error");
			}
			else
			{
				JCLIB_ABORT();
				return std::string{};
			};
		}
		else
		{
			JCLIB_ABORT();
			return std::string{};
		};
	};

	// Challenges the AI to a game
	// https://lichess.org/api#operation/challengeAi
	jc::maybe<bool, std::string> challenge_ai(http::Client& _client, int _level)
	{
		http::Params _params
		{
			{ "level", std::to_string(_level) },
			{ "days", "1" },
			{ "color", "random" },
			{ "variant", "standard" },
			{ "fen",  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" }
		};
		
		const auto _result = _client.Post("/api/challenge/ai", _params);
		if (_result)
		{
			if (_result->status == 200 || _result->status == 201)
			{
				// Succesfull challenge
				return true;
			}
			else if (_result->status == 400)
			{
				// Bad challenge
				const auto _response = json::parse(_result->body, nullptr, false);
				if (_response.is_discarded())
				{
					JCLIB_ABORT();
					return std::string{};
				};

				// Get error message
				return (std::string)_response.at("error");
			}
			else
			{
				JCLIB_ABORT();
				return std::string{};
			};
		}
		else
		{
			JCLIB_ABORT();
			return std::string{};
		};
	};

};
