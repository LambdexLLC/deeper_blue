#include <httplib.h>
#include <nlohmann/json.hpp>

#include <format>
#include <iostream>

constexpr auto http_address_v = "localhost";
constexpr auto http_port_v = 42069;

namespace http = httplib;
using json = nlohmann::json;


inline void print(std::string_view _fmt, const auto&... _args)
{
	std::cout << std::format(_fmt, _args...);
};
inline void println(std::string_view _fmt, const auto&... _args)
{
	std::cout << std::format(_fmt, _args...) << '\n';
};


int main(int _nargs, const char* _vargs[])
{
	http::Client _client(http_address_v, http_port_v);
	
	if (_nargs < 3)
	{
		std::cout << "usage: \n";
		std::cout << std::format("\tuser <username>\n");
		std::cout << std::format("\tai <level>\n");
		return -1;
	};

	std::cout << "https://lichess.org/@/lambdex/tv" << '\n';

	const std::string _command = _vargs[1];

	if (_command == "user")
	{
		http::Params _params
		{
			{ "username", _vargs[2] }
		};
		
		const auto _result = _client.Post("/lichess/challenge/user", _params);
		if (!_result)
		{
			println("Failed to challenge the user!\n");
		}
		else
		{
			println("{}", json::parse(_result->body).dump(1, '\t'));
		};
	}
	else if (_command == "ai")
	{
		http::Params _params
		{
			{ "level", _vargs[2] }
		};

		const auto _result = _client.Post("/lichess/challenge/ai", _params);
		if (!_result)
		{
			println("Failed to challenge the ai!\n");
		}
		else
		{
			println("{}", json::parse(_result->body).dump(1, '\t'));
		};
	}
	else
	{
		println("unrecognized command \"{}\"", _command);
		return -2;
	};

	return 0;
};
