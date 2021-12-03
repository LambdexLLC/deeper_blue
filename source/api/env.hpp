#pragma once

#include "utility/filesystem.hpp"

#include <string>
#include <utility>

namespace lbx::api
{
	/**
	 * @brief Sets the environment variable folder path
	 * @param _filepath File path
	*/
	void set_env_folder_path(const fs::path& _filepath);

	/**
	 * @brief Loads in the environmnet variables. Make sure you've called set_env_folder_path()
	 * prior to calling this.
	 * @return True if everything loaded correctly, false otherwise.
	*/
	bool load_env();

	/**
	 * @brief Creates a HTTP(s) authentication bearer token header using the lichess oath token
	*/
	std::pair<std::string, std::string> make_lichess_bearer_authentication_token_header();
};
