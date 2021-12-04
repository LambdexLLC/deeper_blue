#pragma once

/*
	Helper functions for interacting with the
	https://lichess.org api.

	See https://lichess.org/arg for docs.
*/

#include "lichess.hpp"

#include "utility/http.hpp"

#include <jclib/maybe.h>

#include <string>
#include <string_view>

namespace lbx::api::lichess
{
	// Returns an ok boolean, or an error string
	jc::maybe<bool, std::string> accept_challenge(http::Client& _client, std::string_view _challengeID);

};
