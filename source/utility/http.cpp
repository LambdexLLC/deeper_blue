#include "http.hpp"

#include "utility/io.hpp"
#include "utility/json.hpp"

#include <array>
#include <random>
#include <source_location>

namespace lbx::http
{
	/**
	 * @brief Content reciever function for new line delimeted json data
	 * @param _data Pointer to the received data
	 * @param _len Length of the data
	 * @return True if content is valid, false otherwise
	*/
	bool ndjson_content_reciever(const char* _data, size_t _len)
	{
		const auto _result = json::parse(std::string_view{ _data, _len }, nullptr, false, false);
		if (!_result.is_discarded())
		{
			return true;
		}
		else
		{
			return false;
		};
	};
}



namespace lbx::http::async
{
	/**
	 * @brief Directs the server thread to shutdown.
	*/
	void Server::shutdown()
	{
		http::Client _client{ this->host_, this->port_ };
		const auto _result = _client.Post(this->state_->shutdown_endpoint.c_str(), this->state_->internal_key, "text/plain");
		
		// Ensure good shutdown
		if (!_result || _result->status != 200)
		{
			const auto _thisFilePath = fs::relative(std::source_location::current().file_name(), SOURCE_ROOT);
			println("ERROR ({}) : internal shutdown request failed !!! ", _thisFilePath.generic_string());
			JCLIB_ASSERT(false);
		};
	};

	/**
	 * @brief Starts listening on the server.
	 *
	 * This cannot be called twice. Only call this after
	 * setting the get, post, and other functions for
	 * this http server.
	 *
	 * After calling this function, do not add additional
	 * http endpoints like Get or Post.
	 *
	 * @param _host Host name string.
	 * @param _port Port for the server.
	*/
	bool Server::listen(const std::string& _host, int _port)
	{
		// Bind server
		if (!this->state_->server.bind_to_port(_host.c_str(), _port))
		{
			return false;
		};

		// Save the host and port
		this->host_ = _host;
		this->port_ = _port;

		// Start thread
		this->thread_ = std::jthread
		{
			this->server_main,
			jc::reference_ptr{ *this->state_ }
		};

		// Wait until start
		this->state_->start_gate.arrive_and_wait();

		return true;
	};

	/**
	 * @brief The main thread function for the server.
	 * @param _state State shared with the thread.
	*/
	void Server::server_main(jc::reference_ptr<State> _state)
	{
		// Add the shutdown method
		_state->server.Post(_state->shutdown_endpoint,
			[_state](const Request& _request, Response& _response)
			{
				// Get the key value
				const auto _requestKey = _request.body;
				
				// Check if the key param matches our internal copy
				if (_requestKey != _state->internal_key)
				{
					// Bad key
					json _json = json::object();
					_json["error"] = "invalid key";
					_response.set_content(_json.dump(), "application/json");
					_response.status = 400;
					return;
				};

				// Set response even though it will never be sent
				_response.status = 200;

				// Shutdown server.
				_state->server.stop();
				return;
			});

		// Hit start gate
		_state->start_gate.arrive_and_wait();

		// Listen !
		const auto _result = _state->server.listen_after_bind();

		// Yell about error if listen returned false.
		if (!_result)
		{
			const auto _thisFilePath = fs::relative(std::source_location::current().file_name(), SOURCE_ROOT);
			println("ERROR ({}) : http server listen returned false!", _thisFilePath.generic_string());
		}
		else
		{
			println("Async HTTP Server thread exiting");
		};
	};




	namespace
	{
		// Makes a random seed sequence using the std::random_device
		struct SeedGenerator
		{
			void generate(auto _begin, const auto _end) const
			{
				std::random_device _rnd{};
				for (_begin; _begin != _end; std::advance(_begin, 1))
				{
					*_begin = _rnd();
				};
			};
		};

		/**
		 * @brief Makes a new random key string.
		 * @return Key string.
		*/
		inline std::string make_random_key_string()
		{
			const auto _seedGenerator = SeedGenerator{};
			std::mt19937 _mt(_seedGenerator);
			std::uniform_int_distribution<int> _dist{ 65, 122 };

			// Generates a random character
			const auto char_generator = [&_mt, &_dist]() -> char
			{
				return static_cast<char>(_dist(_mt));
			};

			// Make our string and fill with random characters
			std::string _string(512, '\0');
			std::ranges::generate(_string, char_generator);
			std::ranges::replace(_string, '\0', '0');
			return _string;
		};
	};


	/**
	 * @brief Construct the shared state.
	 *
	 * Does not start the thread.
	*/
	Server::Server() :
		Server{ make_random_key_string() }
	{};

}