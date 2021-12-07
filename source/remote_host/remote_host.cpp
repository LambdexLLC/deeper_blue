#include "remote_host.hpp"

#include "utility/io.hpp"
#include "utility/http.hpp"

#include <jclib/memory.h>

#include <thread>
#include <vector>

namespace lbx::remote
{
	/**
	 * @brief Opaque data type for deeper_blue remote hosts
	*/
	struct RemoteHost
	{
		static void thread_main(std::stop_token _stop, std::string _hostAddr, uint16_t _hostPort)
		{
			http::Server _server{};

			_server.Get("/hi", [](const httplib::Request&, httplib::Response& res) {
					res.set_content("Hello World!", "text/plain");
				});

			println("about to listen for remote");

			_server.listen(_hostAddr.c_str(), _hostPort);

			println("exiting remote host thread");
		};

		RemoteHost(const std::string& _hostAddr, uint16_t _hostPort) :
			thread_{ thread_main, _hostAddr, _hostPort }
		{};

		std::jthread thread_;
	};

	/**
	 * @brief Manages memory and cleanup of remote hosts
	*/
	struct HostManager
	{
		/**
		 * @brief The set of remote hosts that are currently active
		*/
		std::vector<std::unique_ptr<RemoteHost>> hosts{};
	};

	namespace
	{
		inline auto& get_host_manager()
		{
			static HostManager manager_{};
			return manager_;
		};
	};

	/**
	 * @brief Creates a new deeper_blue remote host
	 * @param _hostAddress Address to host the remote on
	 * @param _port Port for the remote to connect to
	 * @return Borrowing handle to the created host object, or nullptr on failure
	*/
	RemoteHost* new_remote_host(const std::string& _hostAddress, uint16_t _port)
	{
		auto& _man = get_host_manager();
		_man.hosts.push_back(jc::make_unique<RemoteHost>(_hostAddress, _port));
		return _man.hosts.back().get();
	};
};
