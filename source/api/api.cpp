#include "api.hpp"
#include "application/env.hpp"

#include "lichess/lichess_http_api.hpp"

#include "utility/io.hpp"
#include "utility/httpstream.hpp"
#include "utility/unordered_map.hpp"

#include <ranges>

namespace lbx::api
{
	namespace
	{
		constexpr inline auto lichess_url_v = "https://lichess.org";

		struct LichessGameAPI_State
		{
		public:

			jc::borrow_ptr<LichessGameAPI> api{};

			std::string_view game_id() const
			{
				return this->game_id_;
			};

			void forward_events()
			{
				if (this->api)
				{
					while (this->event_stream_.has_events())
					{
						const auto _event = this->event_stream_.next_event();
						if (_event.is_object() && _event.contains("type"))
						{
							if (const auto _typeJson = _event.at("type"); _typeJson.is_string())
							{
								std::string_view _type = _typeJson;
								if (_type.starts_with("game"))
								{
									if (_type.ends_with("Full"))
									{
										this->api->on_game(_event);
									}
									else if (_type.ends_with("State"))
									{
										this->api->on_game_change(_event);
									}
									else
									{
										JCLIB_ABORT();
									};
								}
								else if (_type.starts_with("chatLine"))
								{
									this->api->on_chat(_event);
								};
							};
						};
					};
				};
			};

			auto& client()
			{
				return this->client_;
			};
			const auto& client() const
			{
				return this->client_;
			};

			static auto make_client()
			{
				http::Client _client{ lichess_url_v };
				chess::set_lichess_bearer_token_auth(_client);
				return _client;
			};

			auto make_event_stream(const std::string_view _gameID)
			{
				this->path_ = std::format("/api/bot/game/stream/{}", _gameID);
				return http::HTTPClientEventStream{ this->event_client_, this->path_.c_str() };
			};

			LichessGameAPI_State(const std::string_view _gameID) :
				game_id_{ _gameID },
				client_{ this->make_client() },
				event_client_{ this->make_client() },
				stream_{ make_event_stream(_gameID) },
				event_stream_{ this->stream_.get_stream() }
			{};

		private:
			std::string game_id_;
			std::string path_;
			http::Client client_;
			http::Client event_client_;
			http::HTTPClientEventStream stream_;
			http::HTTPClientEventStream::Stream event_stream_;
		};

		struct LichessAccountAPI_State
		{
		public:

			jc::borrow_ptr<LichessAccountAPI> account_api{};
			unordered_map<std::string, std::unique_ptr<LichessGameAPI_State>> games{};


			void forward_events()
			{
				if (this->account_api)
				{
					while (this->event_stream_.has_events())
					{
						const auto _event = this->event_stream_.next_event();
						if (_event.is_object() && _event.contains("type"))
						{
							if (const auto _typeJson = _event.at("type"); _typeJson.is_string())
							{
								std::string_view _type = _typeJson;
								if (_type.starts_with("game"))
								{
									if (_type.ends_with("Start"))
									{
										this->account_api->on_game_start(_event);
									}
									else if (_type.ends_with("Finish"))
									{
										this->account_api->on_game_finish(_event);
									}
									else
									{
										JCLIB_ABORT();
									};
								}
								else if (_type.starts_with("challenge"))
								{
									if (_type.ends_with("Canceled"))
									{
										this->account_api->on_challenge_canceled(_event);
									}
									else if (_type.ends_with("Declined"))
									{
										this->account_api->on_challenge_declined(_event);
									}
									else
									{
										this->account_api->on_challenge(_event);
									};
								};
							};
						};
					};
				};

				// Tell game APIs to forward events
				for (auto& _game : this->games | std::views::values)
				{
					_game->forward_events();
				};
			};

			auto& client()
			{
				return this->client_;
			};
			const auto& client() const
			{
				return this->client_;
			};

			static auto make_client()
			{
				http::Client _client{ lichess_url_v };
				chess::set_lichess_bearer_token_auth(_client);
				return _client;
			};

			LichessAccountAPI_State() :
				client_{ this->make_client() },
				event_client_{ this->make_client() },
				stream_{ this->event_client_, "/api/stream/event" },
				event_stream_{ this->stream_.get_stream() }
			{};

		private:
			http::Client client_;
			http::Client event_client_;
			http::HTTPClientEventStream stream_;
			http::HTTPClientEventStream::Stream event_stream_;
		};
	};

	inline auto& get_account_api_state()
	{
		static LichessAccountAPI_State state_;
		return state_;
	};


	/**
	 * @brief Accepts an incoming challenge from another player
	*/
	bool LichessAccountAPI::accept_challenge(std::string_view _challengeID)
	{
		const auto _result = lichess::accept_challenge(get_account_api_state().client(), _challengeID);
		return _result.has_value() && _result.value();
	};

	/**
	 * @brief Gets the games we are currently playing in
	 * @return The list of game IDs
	*/
	std::vector<std::string> LichessAccountAPI::get_current_games()
	{
		return lichess::get_current_games(get_account_api_state().client());
	};

	/**
	 * @brief Creates a challenge against another user
	 * @param _username Username of the user to challenge
	*/
	bool LichessAccountAPI::challenge_user(std::string_view _username)
	{
		const auto _result = lichess::challenge_user(get_account_api_state().client(), _username);
		return _result.has_value() && _result.value();
	};

	/**
	 * @brief Challenges the AI to a game
	 * @param _level Bot level, must be 1 up to 8
	 * @return True on good challenge, false otherwise
	*/
	bool LichessAccountAPI::challenge_ai(int _level)
	{
		const auto _result = lichess::challenge_ai(get_account_api_state().client(), _level);
		return _result.has_value() && _result.value();
	};

	/**
	 * @brief Challenges the AI to a game of bullet chess.
	 * @param _level Bot level, must be 1 up to 8
	 * @return True on good challenge, false otherwise
	*/
	bool LichessAccountAPI::challenge_ai_bullet(int _level)
	{
		const auto _result = lichess::challenge_ai(get_account_api_state().client(), _level, lichess::GameSettings::bullet_chess());
		return _result.has_value() && _result.value();
	};

	/**
	 * @brief Submits this as our move for the turn
	 * @param _move Move to submit
	 * @param _errmsg Optional error message output string, defaults to nullptr
	 * @return True if move was valid, false otherwise
	*/
	bool LichessGameAPI::submit_move(const chess::Move& _move, std::string* _errmsg)
	{
		// Find our API state object
		auto& _accountState = get_account_api_state();
		for (auto& _game : _accountState.games | std::views::values)
		{
			if (_game->api == this)
			{
				// Stringify move
				std::array<char, 6> _buffer{};
				const auto _tocResult = chess::to_chars(_buffer.data(), _buffer.data() + _buffer.size(), _move);
				JCLIB_ASSERT(_tocResult.ec == std::errc{});

				// Move in string form
				std::string_view _moveStr{ _buffer.data(), _tocResult.ptr };
				
				const auto _gameID = _game->game_id();
				
				// Try submit move
				const auto _moveResult = lichess::send_move(_game->client(), _gameID, _moveStr);
				if (_moveResult && _moveResult.value())
				{
					// We did it!
					return true;
				}
				else
				{
					if (_errmsg)
					{
						// keep searching fool
						*_errmsg = _moveResult.alternate();
					}
					else
					{
						// keep searching fool
						println("Invalid Move : {}", _moveResult.alternate());
					};
				};
			};
		};

		// Either no moves were valid or could not find API
		return false;
	};

	/**
	 * @brief Resigns from the game
	 * @return True on good resign, false otherwise
	*/
	bool LichessGameAPI::resign()
	{
		// Find our API state object
		auto& _accountState = get_account_api_state();
		for (auto& _game : _accountState.games | std::views::values)
		{
			if (_game->api == this)
			{
				const auto _gameID = _game->game_id();

				// Try resign
				const auto _moveResult = lichess::resign_game(_game->client(), _gameID);
				if (_moveResult && _moveResult.value())
				{
					return true;
				}
				else
				{
					println("Failed to resing : {}", _moveResult.alternate());
				};
			};
		};
		return false;
	};


	/**
	 * @brief Forwards lichess events to their associated APIs.
	*/
	void forward_events()
	{
		get_account_api_state().forward_events();
	};

	/**
	 * @brief Sets the lichess account api interface
	 * @param _api Borrowing pointer to to an account API interface object
	*/
	void set_account_api(jc::borrow_ptr<LichessAccountAPI> _api)
	{
		JCLIB_ASSERT(_api);

		get_account_api_state().account_api = _api;
	};

	/**
	 * @brief Sets the game API to use for handling events from a particular game
	 * @param _gameID ID of the game to set the API of
	 * @param _api API to use
	*/
	void set_game_api(std::string_view _gameID, jc::borrow_ptr<LichessGameAPI> _api)
	{
		JCLIB_ASSERT(_api);

		auto& _accountState = get_account_api_state();
		auto& _games = _accountState.games;

		auto it = _games.find(_gameID);
		if (it == _games.end())
		{
			// Add state to games
			it = _games.insert(it, { (std::string)_gameID, jc::make_unique<LichessGameAPI_State>(_gameID) });
		};

		// Set api
		it->second->api = _api;

	};

};