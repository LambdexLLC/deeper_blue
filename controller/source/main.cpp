#include <httplib.h>
#include <nlohmann/json.hpp>

#include <iostream>

constexpr auto http_address_v = "localhost";
constexpr auto http_port_v = 42069;

namespace http = httplib;
using json = nlohmann::json;

int main(int _nargs, const char* _vargs[])
{
	http::Client _client(http_address_v, http_port_v);
	
	if (_nargs < 2)
	{
		std::cout << "missing username to challenge!\n";
	};

	std::cout << "https://lichess.org/@/lambdex/tv" << '\n';

	{
		http::Params _params
		{
			{ "username", _vargs[1] }
		};
		
		const auto _result = _client.Post("/lichess/challenge/user", _params);
		if (!_result || _result->status != 200)
		{
			std::cout << "Failed to challenge the user!\n";
		}
		else
		{
			std::cout << json::parse(_result->body).dump(1, '\t') << '\n';
		};
	};


	return 0;
};
