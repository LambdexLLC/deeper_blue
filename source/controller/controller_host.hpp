#pragma once

#include "utility/http.hpp"

#include <thread>

namespace lbx::chess
{
	/**
	 * @brief Interface that can be interacted with from a controller.
	*/
	class IControllerAPI
	{
	public:

		/**
		 * @brief Temporary testing function.
		*/
		virtual std::string test() = 0;

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
