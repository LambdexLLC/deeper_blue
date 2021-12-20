#include "controller_host.hpp"




namespace lbx::chess
{

	/**
	 * @brief Start the HTTP server.
	*/
	void ControllerHost::start_server(const std::string& _host, int _port)
	{
		auto& _server = this->server_;
		_server.Post("/lichess/challenge/user", [this](const http::Request& _request, http::Response& _response)
			{
				if (!_request.has_param("username"))
				{
					_response.status = 400;
					_response.body = R"({ "error" : "missing name param" })";
					return;
				};

				const auto _username = _request.get_param_value("username");
				const auto _result = this->api_->challenge_lichess_user(_username);

				_response.status = _result.status;
				_response.body = _result.content.dump();

				return;
			});

		_server.listen(_host, _port);
	};
	
};