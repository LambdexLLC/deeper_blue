#include "json.hpp"

#include <fstream>

namespace lbx
{
	/**
	 * @brief Loads in a json file from disk.
	 * @param _path Path to the json file.
	 * @return The json object on success, or a discard object on error.
	*/
	json load_json_file(const fs::path& _path, jc::nothrow_t) noexcept
	{
		std::ifstream _file{ _path };
		return json::parse(_file, nullptr, false);
	};

	/**
	 * @brief Loads in a json file from disk.
	 * @param _path Path to the json file.
	 * @return The json object on success.
	 * @exception json::exception on error.
	*/
	json load_json_file(const fs::path& _path) JCLIB_NOEXCEPT
	{
#if JCLIB_EXCEPTIONS_V
		std::ifstream _file{ _path };
		return json::parse(_file);
#else
		return load_json_file(_path, jc::nothrow);
#endif
	};

};

