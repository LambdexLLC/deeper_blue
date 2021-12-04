#include "lichess_http_api.hpp"

#include "utility/json.hpp"

#include <format>

namespace lbx::api::lichess
{
	jc::maybe<bool, std::string> accept_challenge(http::Client& _client, std::string_view _challengeID)
	{
		const auto _path = std::format("/api/challenge/{}/accept", _challengeID);
		const auto _result = _client.Post(_path.c_str());
		if (_result)
		{
			auto _json = json::parse(_result->body);
			if (_json.contains("ok"))
			{
				return (bool)_json.at("ok");
			}
			else if (_json.contains("error"))
			{
				return (std::string)_json.at("error");
			}
			else
			{
				JCLIB_ABORT();
				return std::string{};
			};
		}
		else
		{
			JCLIB_ABORT();
			return std::string{};
		};
	};
};
