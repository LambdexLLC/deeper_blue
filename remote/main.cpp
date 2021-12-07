/*
	Remote client for interacting with deeper_blue
*/

#include <httplib.h>
#include <iostream>

namespace http = httplib;





int main()
{
	http::Client _client("localhost", 42069);
	std::cout << _client.Get("/hi")->body << '\n';
	return 0;
};
