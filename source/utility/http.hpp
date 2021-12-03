#pragma once

#ifndef CPPHTTPLIB_OPENSSL_SUPPORT
#define CPPHTTPLIB_OPENSSL_SUPPORT
#endif

#include <httplib.h>

namespace lbx::http
{
	// Add names to here
	using namespace ::httplib;

	/**
	 * @brief Content reciever function for new line delimeted json data
	 * @param _data Pointer to the received data
	 * @param _len Length of the data
	 * @return True if content is valid, false otherwise
	*/
	bool ndjson_content_reciever(const char* _data, size_t _len);
};
