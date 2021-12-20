#include "controller_host.hpp"




namespace lbx::chess
{

	/**
	 * @brief Start the HTTP server.
	*/
	void ControllerHost::start_server(const std::string& _host, int _port)
	{
		auto& _api = *this->api_;
		auto& _server = this->server_;

		_server.Get("/test", [this](const http::Request& _request, http::Response& _response)
			{
				_response.set_content(this->api_->test(), "text/plain");
				_response.status = 200;
			});

		_server.listen(_host, _port);
	};
	
};