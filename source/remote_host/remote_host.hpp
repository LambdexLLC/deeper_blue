#pragma once

#include <string>
#include <cstdint>

namespace lbx::remote
{
	/**
	 * @brief Opaque data type for deeper_blue remote hosts
	*/
	struct RemoteHost;

	/**
	 * @brief Creates a new deeper_blue remote host
	 * @param _hostAddress Address to host the remote on
	 * @param _port Port for the remote to connect to
	 * @return Borrowing handle to the created host object, or nullptr on failure
	*/
	RemoteHost* new_remote_host(const std::string& _hostAddress, uint16_t _port);
};
