#pragma once

#include <string>
#include <filesystem>

namespace lbx
{
	namespace fs
	{
		using namespace std::filesystem;

#if false
		/**
		 * @brief Reads a text file in from the disk.
		 * @param _path Path to the text file.
		 * @return Text file contents.
		*/
		std::string read_text_file(const path& _path);
#endif
	};
};
