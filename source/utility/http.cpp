#include "http.hpp"

#include "json.hpp"

namespace lbx::http
{
	/**
	 * @brief Content reciever function for new line delimeted json data
	 * @param _data Pointer to the received data
	 * @param _len Length of the data
	 * @return True if content is valid, false otherwise
	*/
	bool ndjson_content_reciever(const char* _data, size_t _len)
	{
		const auto _result = json::parse(std::string_view{ _data, _len }, nullptr, false, false);
		if (!_result.is_discarded())
		{
			return true;
		}
		else
		{
			return false;
		};
	};
}