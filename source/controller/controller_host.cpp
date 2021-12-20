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

		_server.Post("/lichess/challenge/ai", [this](const http::Request& _request, http::Response& _response)
			{
				if (!_request.has_param("level"))
				{
					_response.status = 400;
					_response.body = R"({ "error" : "missing level param" })";
					return;
				};

				const auto _levelStr = _request.get_param_value("level");

				int _level = 0;
				const auto _err = std::from_chars(_levelStr.data(), _levelStr.data() + _levelStr.size(), _level);

				if (_err.ec != std::errc{} || _level < 1 || _level > 15)
				{
					_response.status = 400;
					_response.body = R"({ "error" : "invalid level parameter" })";
					return;
				};

				const auto _result = this->api_->challenge_lichess_bot(_level);
				_response.status = _result.status;
				_response.body = _result.content.dump();
				return;
			});

		_server.listen(_host, _port);
	};
	
};

