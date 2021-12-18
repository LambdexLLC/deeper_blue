#pragma once

/*
	Configuration loading and interaction code
*/

#include "utility/json.hpp"
#include "utility/filesystem.hpp"

#include <string_view>

namespace lbx::chess_view
{
	/**
	 * @brief Sets the path to the root directory of the application.
	 * @param _path 
	*/
	void set_application_root(const fs::path& _path);

	/**
	 * @brief Gets the previously set application root path.
	 * 
	 * This will return a default constructed path if set_application_root() was not called prior.
	 * 
	 * @return Root directory path.
	*/
	fs::path get_application_root();

	/**
	 * @brief Loads the configuration file.
	 * 
	 * Make sure to set the application root path prior to calling this.
	 * 
	 * @return True on good load, false otherwise.
	*/
	bool load_config();

	/**
	 * @brief Gets a configuration value as a json object.
	 * @param _name Name of the value.
	 * @return Json object with config data or a default constructed json object on error.
	*/
	json get_config_value(std::string_view _name);
};
