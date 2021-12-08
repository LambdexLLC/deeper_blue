#pragma once

/*
	Formatter redirection for GCC not having standard format support reee
*/

#include <jclib/concepts.h>

#include <fmt/format.h>
#include <version>

#if defined(__cpp_lib_format) && __cpp_lib_format >= 201907L

#include <format>

namespace std
{
	template <typename T>
	requires requires(const T& _value, fmt::format_context& _ctx)
	{
		fmt::formatter<T>::format(_value, _ctx);
	}
	struct formatter<T, char> : public fmt::formatter<T>
	{
		using fmt::formatter<T>::formatter;
	};
};

#endif

namespace lbx
{
	/**
	 * @brief Fufilled by types with a fmt::formatter defined
	*/
	template <typename T>
	concept cx_formattable = requires (fmt::formatter<T> _fmt, fmt::format_context& _ctx, const T& _value)
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
		return fmt::format(_fmt, _args...);
	};
};

