#include "json.hpp"

#include "filesystem.hpp"

#include <fstream>

namespace lbx
{
	/**
	 * @brief Reads a json file into memory
	 * @param _filepath Path to the file path
	 * @return Json object
	*/
	json read_json_file(const fs::path& _filepath)
	{
		std::ifstream _file{ _filepath };
		return json::parse(_file);
	};
};