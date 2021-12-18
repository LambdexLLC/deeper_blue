#pragma once

#include "filesystem.hpp"

#include <nlohmann/json.hpp>
#include <jclib/exception.h>

namespace lbx
{
	using json = nlohmann::json;

	/**
	 * @brief Loads in a json file from disk.
	 * @param _path Path to the json file.
	 * @return The json object on success, or a discard object on error.
	*/
	json load_json_file(const fs::path& _path, jc::nothrow_t) noexcept;

	/**
	 * @brief Loads in a json file from disk.
	 * @param _path Path to the json file.
	 * @return The json object on success.
	 * @exception json::exception on error.
	*/
	json load_json_file(const fs::path& _path) JCLIB_NOEXCEPT;
};
