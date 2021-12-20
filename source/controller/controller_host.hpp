#pragma once

#include "utility/http.hpp"
#include "utility/json.hpp"

#include <thread>

namespace lbx::chess
{
	/**
	 * @brief Interface that can be interacted with from a controller.
	 * 
	 * The virtual functions defined are essentially callback functions.
	 * These will be invoked from a seperate thread then the main thread
	 * and synchronization MUST be performed by implementations.
	*/
	class IControllerAPI
	{
	public:

		/**
		 * @brief Result structure returned to controller clients.
		*/
		struct Result
		{
			/**
			 * @brief The content of the result
			*/
			json content{};

			/**
			 * @brief HTTP status code, 200 is good!
			*/
			int status = 200;
		};

		/**
		 * @brief Challenges a lichess user to a chess game.
		 * @param _username Lichess username of the user to challenge.
		 * @return HTTP result object.
		*/
		virtual Result challenge_lichess_user(const std::string& _username) = 0;

		/**
		 * @brief Challenges a bot on lichess to a chess match.
		 * @param _level Stockfish level to challenge.
		 * @return HTTP result object.
		*/
		virtual Result challenge_lichess_bot(int _level) = 0;

		// Polymorphic destruction !
		virtual ~IControllerAPI() = default;
	};

	/**
	 * @brief Hosts a controller interface, allowing controllers to connect.
	*/
	class ControllerHost
	{
	private:

		/**
		 * @brief Start the HTTP server.
		*/
		void start_server(const std::string& _host, int _port);

	public:

		

		/**
		 * @brief Constructs the host with an API implementation and starts the HTTP server.
		 * @param _api The API implementation, MUST NOT BE NULL.
		 * @param _host Host address.
		 * @param _port HTTP server port.
		*/
		ControllerHost(std::unique_ptr<IControllerAPI> _api, const std::string& _host, int _port) :
			api_{ std::move(_api) },
			server_{} // using builtin randomized key string
		{
			JCLIB_ASSERT(this->api_);
			this->start_server(_host, _port);
		};

	private:

		/**
		 * @brief The controller API implementation.
		*/
		std::unique_ptr<IControllerAPI> api_;

		/**
		 * @brief The http server for controllers to interact with.
		*/
		http::async::Server server_;

	};
};
