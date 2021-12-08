#pragma once

/*
	Formatter redirection for GCC not having standard format support reee
*/

#include <jclib/concepts.h>

#include <format>
#include <version>

namespace lbx
{
	/**
	 * @brief Fufilled by types with a fmt::formatter defined
	*/
	template <typename T>
	concept cx_formattable = requires (std::formatter<T> _fmt, std::format_context& _ctx, const T& _value)
	{
		_fmt.format(_value, _ctx);
	}
	|| jc::cx_convertible_to<T, const char*>;

	/**
	 * @brief Same as std::format but checks if the given type has a formatter defined
	 * @param _fmt Format string
	 * @param ..._args Formatted arguements
	 * @return Formatted string result
	*/
	template <cx_formattable... Ts>
	inline auto format(std::string_view _fmt, const Ts&... _args)
	{
		return std::format(_fmt, _args...);
	};
};

