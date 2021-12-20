#include "lichess_http_api.hpp"

#include <lambdex/chess/fen.hpp>

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


	/**
	 * @brief Implementation helper for creating a lichess challenge.
	 *
	 * @param _client HTTP client object to send request.
	 * @param _path HTTP POST endpoint.
	 * @param _params POST parameters.
	 * @return True on success, error string otherwise.
	*/
	inline jc::maybe<bool, std::string> make_challenge(http::Client& _client, const std::string& _path, const http::Params& _params)
	{
		const auto _result = _client.Post(_path.c_str(), _params);
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

	/**
	 * @brief Creates HTTP parameters for a challenge's game settings.
	 * @param _settings Game settings.
	 * @return The challenge parameters object.
	*/
	inline http::Params make_challenge_http_params(const GameSettings& _settings)
	{
		http::Params _params
		{
			{ "color", "random" },
			{ "fen", chess::get_board_fen(chess::make_standard_board()) },
			{ "variant", _settings.variant },
			{ "rated", format("{}", _settings.rated) },
			{ "message", "Your game with {opponent} is ready: {game}." }
		};

		// Set time controls
		switch (_settings.time_control.type())
		{
		case TimeControl::Type::unlimited:
			// No params
			break;
		
		case TimeControl::Type::real_time:
		{
			auto& _tc = _settings.time_control.get<TimeControl::Type::real_time>();
			_params.insert({ "clock.limit", std::to_string(_tc.limit.count()) });
			_params.insert({ "clock.increment", std::to_string(_tc.increment.count()) });
		};
		break;

		case TimeControl::Type::correspondence:
		{
			auto& _tc = _settings.time_control.get<TimeControl::Type::correspondence>();
			_params.insert({ "days", std::to_string(_tc.limit.count()) });
		};
		break;

		default:
			JCLIB_ABORT();
			break;
		}

		return _params;
	};

	/**
	 * @brief Returns a true or an error string on failure
	 *
	 * See https://lichess.org/api#operation/challengeCreate
	 *
	 * @param _client HTTP client to make request with.
	 * @param _username Name of the user to challenge.
	 * @param _settings Game settings for the challenge.
	 *
	 * @return True on success, error string otherwise.
	*/
	jc::maybe<bool, std::string> challenge_user(http::Client& _client, std::string_view _username, GameSettings _settings)
	{
		const auto _path = format("/api/challenge/{}", _username);
		const auto _params = make_challenge_http_params(_settings);
		return make_challenge(_client, _path, _params);
	};


	/**
	 * @brief Challenges the stockfish AI to a game.
	 *
	 * See https://lichess.org/api#operation/challengeAi
	 *
	 * @param _level Stockfish level.
	 * @param _settings Game settings for the challenge.
	 * 
	 * @return True on success, error string otherwise.
	*/
	jc::maybe<bool, std::string> challenge_ai(http::Client& _client, int _level, GameSettings _settings)
	{
		const auto _path = "/api/challenge/ai";

		auto _params = make_challenge_http_params(_settings);
		_params.insert({ "level", std::to_string(_level) });
		
		return make_challenge(_client, _path, _params);
	};


	// Resigns from the game
	// https://lichess.org/api#operation/botGameResign
	jc::maybe<bool, std::string> resign_game(http::Client& _client, std::string_view _gameID)
	{
		const auto _path = format("/api/bot/game/{}/resign", _gameID);
		const auto _result = _client.Post(_path.c_str());
		if (_result)
		{
			if (_result->status == 200 || _result->status == 201)
			{
				// Succesfull resign
				return true;
			}
			else if (_result->status == 400)
			{
				// Bad resign
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

namespace lbx::api::lichess
{

	/**
	 * @brief Constructs the game settings for standard bullet chess.
	 * @return Game settings.
	*/
	GameSettings GameSettings::bullet_chess(bool _rated)
	{
		using namespace std::chrono_literals;
		
		// Use default settings
		GameSettings _out = default_settings(_rated);
		
		// But change the time control
		_out.time_control = TimeControl::RealTime
		{
			.limit = 5min,
			.increment = 0s
		};

		return _out;
	};

	/**
	 * @brief Constructs the default game settings.
	 * @return Game settings.
	*/
	GameSettings GameSettings::default_settings(bool _rated)
	{
		GameSettings _out{};
		_out.time_control = TimeControl::Correspondence{ std::chrono::days{ 1 } };
		_out.variant = "standard";
		_out.rated = _rated;
		return _out;
	};

}