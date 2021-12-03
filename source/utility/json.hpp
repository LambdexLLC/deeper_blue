#pragma once

#include "filesystem.hpp"

#include <nlohmann/json.hpp>

namespace lbx
{
	using json = nlohmann::json;
	using json_exception = json::exception;

	/**
	 * @brief Reads a json file into memory
	 * @param _filepath Path to the file path
	 * @return Json object
	 * @exception json_exception Thrown by nlohmann json on failure to parse
	*/
	json read_json_file(const fs::path& _filepath);
};
