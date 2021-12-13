#pragma once

#include <filesystem>
#include <fstream>
#include <string>

namespace lbx::fs
{
	using namespace std::filesystem;

	/**
	 * @brief Reads a full text file in from disk into memory
	 * @param _path Path to the file to read
	 * @return String containing file text
	*/
	inline std::string read_text_file(const path& _path)
	{
		std::ifstream _file{ _path };
		std::string _out(file_size(_path), '\0');
		_file.read(_out.data(), _out.size());
		return _out;
	};

};