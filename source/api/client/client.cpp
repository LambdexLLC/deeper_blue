#include "client.hpp"

#include "api/env.hpp"

#include <jclib/config.h>
#include <jclib/type_traits.h>

#include <format>
#include <optional>

namespace lbx::api
{
	namespace
	{
		inline std::optional<json> http_get_json(http::Client& _client, const http::Headers& _headers, const char* _path)
		{
			json _contentJson = json::object();

			uint64_t _current = 0;
			uint64_t _total = 0;

			http::Progress _progressFn = [&](uint64_t _currentIn, uint64_t _totalIn) -> bool
			{
				_current = _currentIn;
				_total = _totalIn;
				return true;
			};

			const http::ContentReceiver _contentReceiverJson = [&](const char* _data, size_t _len) -> bool
			{
				_contentJson = json::parse(std::string_view{ _data,  _len }, nullptr, false);
				return !_contentJson.is_discarded();
			};
			const http::ContentReceiver _contentReceiverNdjson = [&](const char* _data, size_t _len) -> bool
			{
				if (_len == 1 && _data[0] == '\n')
				{
					return true;
				}
				else
				{
					std::string_view _input{ _data, _len };
					auto _firstLineEnd = _input.find('\n');

					_contentJson = json::parse(std::string_view{ _data, _len }, nullptr, false);
					return false;
				};
			};
			const http::ContentReceiver* _useReciever{};

			const http::ContentReceiver _reciever = [&](const char* _data, size_t _len) -> bool
			{
				JCLIB_ASSERT(_useReciever);
				return (*_useReciever)(_data, _len);
			};
			http::ResponseHandler _responseHandler = [&](const http::Response& _response) -> bool
			{
				const auto _contentType = _response.get_header_value("Content-Type");
				if (_contentType == "application/x-ndjson")
				{
					_useReciever = &_contentReceiverNdjson;
				}
				else if (_contentType == "application/json")
				{
					_useReciever = &_contentReceiverJson;
				};

				return true;
			};

			auto _out = _client.Get(_path, _headers, _responseHandler, _reciever, _progressFn);
			
			if (_out)
			{
				if (_contentJson.is_discarded() || (_contentJson.is_object() && _contentJson.empty()))
				{
					_contentJson = json::parse(_out->body, nullptr, false);
				};
			};

			if (_contentJson.is_discarded() || (_contentJson.is_object() && _contentJson.empty()))
			{
				return std::nullopt;
			}
			else
			{
				return _contentJson;
			};
		};
	};

	/**
	 * @brief Follows another player
	 * @param _playerName Name of the player to follow
	 * @return True on good follow, false otherwise
	*/
	bool LichessClient::follow_player(std::string_view _playerName)
	{
		std::string _postPath = "/api/rel/follow/";
		_postPath.append(_playerName);

		http::Headers _headers{};
		_headers.insert(api::make_lichess_bearer_authentication_token_header());
		
		http::Params _params{};
		auto _response = this->http_client_.Post(_postPath.c_str(), _headers, _params);
		if (_response->status == 200)
		{
			json _responseJson = json::parse(_response->body);
			if (const auto& _okJson = _responseJson.at("ok"); _okJson.is_boolean())
			{
				return (bool)_okJson;
			}
			else
			{
				JCLIB_ABORT();
				return false;
			};
		}
		else
		{
			JCLIB_ABORT();
			return false;
		};
	};


	std::vector<LichessChallenge> LichessClient::list_challenges()
	{
		std::vector<LichessChallenge> _out{};

		auto _response = this->http_client_.Get("/api/challenge");
		if (_response->status == 200)
		{
			json _resJson = json::parse(_response->body);
			for (auto& _challengeJson : _resJson.at("in"))
			{
				LichessChallenge _challenge = _challengeJson;
				_out.push_back(_challenge);
			};
		}
		else
		{
			JCLIB_ABORT();
		};

		return _out;
	};

	std::vector<LichessGame> LichessClient::list_games()
	{
		std::vector<LichessGame> _out{};

		auto _response = this->http_client_.Get("/api/account/playing");
		if (_response->status == 200)
		{
			json _resJson = json::parse(_response->body);
			for (auto& _gameJson : _resJson.at("nowPlaying"))
			{
				LichessGame _game = _gameJson;
				_out.push_back(_game);
			};
		}
		else
		{
			JCLIB_ABORT();
		};

		return _out;
	};

	bool LichessClient::create_challenge(std::string_view _username, const ChallengeParams& _params, LichessGame& _game)
	{
		std::string _postPath = std::format("/api/challenge/{}", _username);

		http::Headers _headers{};
		const auto _oathTokenHeader = api::make_lichess_bearer_authentication_token_header();
		_headers.insert(_oathTokenHeader);

		http::Params _httpParams{};
		_httpParams.insert({ "rated", std::to_string(_params.rated) });

		if (!_params.correspondence)
		{
			_httpParams.insert({ "clock.limit", std::to_string(_params.seconds_per_move) });
			_httpParams.insert({ "clock.increment", std::to_string(_params.seconds_added_per_move) });
		}
		else
		{
			_httpParams.insert({ "days", std::to_string(_params.days_per_move) });
		};

		_httpParams.insert({ "color", _params.color });
		_httpParams.insert({ "variant", _params.variant });
		_httpParams.insert({ "keepAliveStream", "false" });
		_httpParams.insert({ "acceptByToken", _oathTokenHeader.second });
		_httpParams.insert({ "fen", _params.fen });

		if (!_params.message.empty())
		{
			_httpParams.insert({ "message", _params.message });
		};

		auto _response = this->http_client_.Post(_postPath.c_str(), _headers, _httpParams);
		if (_response->status == 200)
		{
			json _responseJson = json::parse(_response->body);
			_game = _responseJson;
			return true;
		}
		else
		{
			JCLIB_ABORT();
			return false;
		};
	};
	bool LichessClient::create_challenge(std::string_view _username, const ChallengeParams& _params, LichessChallenge& _challenge)
	{
		std::string _postPath = std::format("/api/challenge/{}", _username);

		http::Headers _headers{};
		const auto _oathTokenHeader = api::make_lichess_bearer_authentication_token_header();
		_headers.insert(_oathTokenHeader);

		http::Params _httpParams{};
		_httpParams.insert({ "rated", std::to_string(_params.rated) });

		if (!_params.correspondence)
		{
			_httpParams.insert({ "clock.limit", std::to_string(_params.seconds_per_move) });
			_httpParams.insert({ "clock.increment", std::to_string(_params.seconds_added_per_move) });
		}
		else
		{
			_httpParams.insert({ "days", std::to_string(_params.days_per_move) });
		};

		_httpParams.insert({ "color", _params.color });
		_httpParams.insert({ "variant", _params.variant });
		_httpParams.insert({ "keepAliveStream", "false" });
		_httpParams.insert({ "fen", _params.fen });

		if (!_params.message.empty())
		{
			//_httpParams.insert({ "message", _params.message });
		};

		auto _response = this->http_client_.Post(_postPath.c_str(), _headers, _httpParams);
		if (_response->status == 200)
		{
			json _responseJson = json::parse(_response->body);
			_challenge = _responseJson;
			return true;
		}
		else
		{
			JCLIB_ABORT();
			return false;
		};
	};

	bool LichessClient::accept_challenge(const std::string_view _challengeID)
	{
		std::string _postPath = std::format("/api/challenge/{}/accept", _challengeID);

		http::Headers _headers{};
		_headers.insert(api::make_lichess_bearer_authentication_token_header());

		http::Params _params{};
		auto _response = this->http_client_.Post(_postPath.c_str(), _headers, _params);
		if (_response->status == 200)
		{
			json _responseJson = json::parse(_response->body);
			if (const auto& _okJson = _responseJson.at("ok"); _okJson.is_boolean())
			{
				return (bool)_okJson;
			}
			else
			{
				JCLIB_ABORT();
				return false;
			};
		}
		else
		{
			JCLIB_ABORT();
			return false;
		};
	};
	bool LichessClient::accept_challenge(const LichessChallenge& _challenge)
	{
		return this->accept_challenge(_challenge.id);
	};

	bool LichessClient::send_move(std::string_view _gameID, std::string_view _move, std::string* _errmsg)
	{
		std::string _postPath = std::format("/api/bot/game/{}/move/{}", _gameID, _move);

		http::Headers _headers{};
		_headers.insert(api::make_lichess_bearer_authentication_token_header());

		http::Params _params{};
		_params.insert({ "offeringDraw", "false" });

		auto _response = this->http_client_.Post(_postPath.c_str(), _params);
		if (_response->status == 200)
		{
			json _responseJson = json::parse(_response->body);
			if (const auto& _okJson = _responseJson.at("ok"); _okJson.is_boolean())
			{
				return (bool)_okJson;
			}
			else
			{
				JCLIB_ABORT();
				return false;
			};
		}
		else if (_response->status == 400)
		{
			// Write error message
			if (_errmsg)
			{
				json::parse(_response->body).at("error").get_to(*_errmsg);
			};
			
			return false;
		}
		else
		{
			JCLIB_ABORT();
			return false;
		};
	};

	bool LichessClient::write_chat(std::string_view _gameID, std::string_view _room, std::string_view _text)
	{
		std::string _postPath = std::format("/api/bot/game/{}/chat", _gameID);

		http::Headers _headers{};
		_headers.insert(api::make_lichess_bearer_authentication_token_header());

		http::Params _params{};
		_params.insert({ "room", std::string{ _room } });
		_params.insert({ "text", std::string{ _text } });

		auto _response = this->http_client_.Post(_postPath.c_str(), _params);
		if (_response->status == 200)
		{
			json _responseJson = json::parse(_response->body);
			if (const auto& _okJson = _responseJson.at("ok"); _okJson.is_boolean())
			{
				return (bool)_okJson;
			}
			else
			{
				JCLIB_ABORT();
				return false;
			};
		}
		else
		{
			JCLIB_ABORT();
			return false;
		};

	};

	void LichessClient::get_game_state(std::string_view _gameID, LichessGameStateFull& _game)
	{
		const auto _path = std::format("/api/bot/game/stream/{}", _gameID);
		http::Headers _headers{};
		auto _response = http_get_json(this->http_client_, _headers, _path.c_str());
		if (_response)
		{
			_game = *_response;
		}
		else
		{
			JCLIB_ABORT();
		};
	};


	void LichessClient::test()
	{
		http::Headers _headers{};
		_headers.insert(api::make_lichess_bearer_authentication_token_header());
		
		auto _responseOpt = http_get_json(this->http_client_, _headers, "/api/stream/event");
		JCLIB_ASSERT(_responseOpt);

		auto& _response = *_responseOpt;
		std::cout << _response.dump('\t', 1) << '\n';
	};

	/**
	* @brief Constructs the client by connecting to the lichess URL
	* @param _lichessURL Lichess website URL
	*/
	LichessClient::LichessClient(const std::string& _lichessURL) :
		http_client_{ _lichessURL }
	{
		api::set_lichess_bearer_token_auth(this->http_client_);
	};

};