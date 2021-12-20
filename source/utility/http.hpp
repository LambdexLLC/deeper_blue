#pragma once

#ifndef CPPHTTPLIB_OPENSSL_SUPPORT
#define CPPHTTPLIB_OPENSSL_SUPPORT
#endif

#include <httplib.h>

#include <jclib/memory.h>

#include <latch>
#include <thread>
#include <string>
#include <string_view>

namespace lbx::http
{
	// Add names to here
	using namespace ::httplib;

	/**
	 * @brief Content reciever function for new line delimeted json data
	 * @param _data Pointer to the received data
	 * @param _len Length of the data
	 * @return True if content is valid, false otherwise
	*/
	bool ndjson_content_reciever(const char* _data, size_t _len);


	namespace async
	{
		/**
		 * @brief Nonblocking HTTP server.
		 * 
		 * Registered endpoint methods are invoked from the server thread and
		 * therefore are not synchronized with the thread constructing the Server.
		 * 
		 * You are responsible for implementing your own synchronization mechanisms.
		*/
		class Server
		{
		private:

			/**
			 * @brief State shared with the server thread.
			*/
			struct State
			{
				/**
				 * @brief The http server object
				*/
				http::Server server;

				/**
				 * @brief The endpoint for the server shutdown Get request.
				*/
				const std::string shutdown_endpoint = "/_____internal_____/shutdown";

				/**
				 * @brief Parameter name for the internal key value.
				*/
				const std::string key_param_name = "key";

				/**
				 * @brief The key string used to allow secure control the HTTP server between threads.
				*/
				std::string internal_key{};

				/**
				 * @brief Latch used as a thread gate to synchronize the main thread with the server thread.
				*/
				std::latch start_gate{ 2 };
			};

			/**
			 * @brief The main thread function for the server.
			 * @param _state State shared with the thread.
			*/
			static void server_main(jc::reference_ptr<State> _state);

			/**
			 * @brief Sets the internal key string.
			 * 
			 * This should not be invoked after calling listen().
			 * 
			 * This is used to ensure internal commands are only accesible from HERE and
			 * not from any client connected. Make sure this string is long and complex.
			 * 
			 * @param _key Internal key string.
			*/
			void set_internal_key(const std::string& _key)
			{
				this->state_->internal_key = _key;
			};

			/**
			 * @brief Directs the server thread to shutdown.
			*/
			void shutdown();

		public:

			// DO NOT CALL AFTER LISTENING !!!!
			Server& Get(const std::string& _pattern, http::Server::Handler _handler)
			{
				this->state_->server.Get(_pattern, std::move(_handler));
				return *this;
			};

			// DO NOT CALL AFTER LISTENING !!!!
			Server& Post(const std::string& _pattern, http::Server::HandlerWithContentReader _handler)
			{
				this->state_->server.Post(_pattern, std::move(_handler));
				return *this;
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
			 * 
			 * @return True on good bind and startup, false otherwise.
			*/
			bool listen(const std::string& _host, int _port);

			/**
			 * @brief Construct the shared state.
			 *
			 * Does not start the thread.
			 * 
			 * @param _internalKey Internal API key string. Make sure this is complex.
			*/
			explicit Server(const std::string& _internalKey) :
				state_{ new State{} }
			{
				this->set_internal_key(_internalKey);
			};

			/**
			 * @brief Construct the shared state.
			 *
			 * Does not start the thread.
			 * 
			 * This uses a randomized key string for the internal key.
			 * If you want to use your own, see the other constructor.
			*/
			explicit Server();

			// Allow move
			Server(Server&& other) noexcept = default;

			// Stops the server thread.
			~Server()
			{
				if (this->state_) JCLIB_LIKELY
				{
					this->shutdown();
				};
			};

		private:
			
			/**
			 * @brief The state shared with the thread.
			*/
			std::unique_ptr<State> state_;

			/**
			 * @brief The server thread handle.
			*/
			std::jthread thread_;

			/**
			 * @brief The server host address, only needed to forward shutdown requests to the server.
			*/
			std::string host_ = "0.0.0.0";

			/**
			 * @brief The server host port, only needed to forward shutdown requests to the server.
			*/
			int port_ = 0;


			// Prevent copy and assignment
			
			Server(const Server& other) = delete;
			Server& operator=(const Server& other) = delete;

			Server& operator=(Server&& other) noexcept = delete;
	
		};

	};
};
