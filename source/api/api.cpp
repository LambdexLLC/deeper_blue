#include "api.hpp"
#include "env.hpp"

#include "lichess_http_api.hpp"

#include "utility/httpstream.hpp"

namespace lbx::api
{
	namespace
	{
		constexpr inline auto lichess_url_v = "https://lichess.org";
		struct LichessAccountAPI_State
		{
		public:

			jc::borrow_ptr<LichessAccountAPI> account_api;

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
				api::set_lichess_bearer_token_auth(_client);
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
		get_account_api_state().account_api = _api;
	};


};