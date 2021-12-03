#pragma once

#include "utility/json.hpp"
#include "utility/http.hpp"

namespace lbx::api
{
	/**
	 * @brief Client data storage
	*/
	struct LichessClient
	{
	public:

		/**
		 * @brief Checks if the client is still alive
		 * @return True if alive, false otherwise
		*/
		bool good() const
		{
			return this->http_client_.is_valid();
		};

		/**
		 * @brief Constructs the client by connecting to the lichess URL
		 * @param _lichessURL Lichess website URL
		*/
		explicit LichessClient(const std::string& _lichessURL) :
			http_client_{ _lichessURL }
		{};

	private:

		/**
		 * @brief The http client object
		*/
		http::Client http_client_;

	};

};