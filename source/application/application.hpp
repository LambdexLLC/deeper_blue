#pragma once

/*
	Top level functions for the overall application
*/

#include "utility/filesystem.hpp"

namespace lbx::chess
{
	/**
	 * @brief Sets the file path to the directory the bot's executable is running in.
	 *
	 * If this is provided, the "set_env_folder_path()" function does not need to be
	 * invoked as the env path will be automatically determined.
	 *
	 * @param _filepath Path to the root directory.
	*/
	void set_application_root_path(const fs::path& _filepath);


};

