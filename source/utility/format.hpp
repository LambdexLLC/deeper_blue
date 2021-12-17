#pragma once

/*
	Formatter redirection for GCC not having standard format support reee
*/

#include <lambdex/utility/format.hpp>


#include <jclib/concepts.h>

#include <fmt/format.h>

#if __has_include(<format>)
	#define LAMBDEX_CHESS_HAS_STANDARD_FORMAT
	#define LAMBDEX_CHESS_HAS_STANDARD_FORMAT_V true
#else
	#define LAMBDEX_CHESS_HAS_STANDARD_FORMAT_V false
#endif // __has_include(<format>)


namespace lbx
{
	namespace impl
	{
		template <typename T>
		concept cx_has_formatter_lbx = requires (lbx::formatter<T> _fmt, const T & _value)
		{
			{ _fmt.format(_value) } -> jc::cx_any_of<std::string, std::string_view>;
		};

		template <typename T>
		concept cx_has_formatter_libfmt = requires (fmt::formatter<T> _fmt, const T& _value, fmt::format_context& _ctx)
		{
			_fmt.format(_value, _ctx);
		};

	};

};



#if LAMBDEX_CHESS_HAS_STANDARD_FORMAT_V

#include <format>

/*
	Adds a redirection for standard library format to use lbx formatters
*/

namespace std
{
	template <lbx::impl::cx_has_formatter_lbx T>
	struct formatter<T, char> : public formatter<string_view, char>
	{
	public:

		// Invokes the lambdex formatter's format method before letting
		// the std::string formatter do its thing
		auto format(const T& _value, auto& _ctx)
		{
			const auto _str = this->lbxfmt_.format(_value);
			return formatter<string_view, char>::format(_str, _ctx);
		};

		formatter() = default;

	private:
		lbx::formatter<T> lbxfmt_{};
	};
};

namespace lbx::impl
{
#if LAMBDEX_CHESS_HAS_STANDARD_FORMAT_V
	template <typename T>
	concept cx_has_formatter_stdfmt = requires (std::formatter<T, char> _fmt, const T & _value, std::format_context & _ctx)
	{
		_fmt.format(_value, _ctx);
	};
#endif
};

#endif // LAMBDEX_CHESS_HAS_STANDARD_FORMAT_V

/*
	Adds a redirection for libfmt formatter to use lbx formatters
*/

namespace fmt
{
	template <lbx::impl::cx_has_formatter_lbx T>
	struct formatter<T, char>
	{
	public:

		// Does nothing by default
		auto parse(auto& _ctx)
		{
			// Parse the presentation format and store it in the formatter:
			auto it = _ctx.begin();
			auto end = _ctx.end();

			// Check if reached the end of the range:
			if (it != end && *it != '}')
				throw format_error("invalid format");

			// Return an iterator past the end of the parsed range:
			return it;
		};

		// Invokes the lambdex formatter's format method before letting
		// the std::string formatter do its thing
		template <typename ContextT>
		auto format(const T& _value, ContextT& _ctx) ->
			decltype(_ctx.out())
		{
			const auto _str = lbx::formatter<T>{}.format(_value);
			return fmt::format_to(_ctx.out(), "{}", _str);
		};

		formatter() = default;

	private:
		lbx::formatter<T> lbxfmt_{};
	};
};


/*
	Final formatting redirection handling
*/

namespace lbx
{
	/**
	 * @brief Fufilled by types with a fmt::formatter defined
	*/
	template <typename T>
	concept cx_formattable = impl::cx_has_formatter_libfmt<T>;
	
	/**
	 * @brief Same as std::format but checks if the given type has a formatter defined
	 * @param _fmt Format string
	 * @param ..._args Formatted arguements
	 * @return Formatted string result
	*/
	template <cx_formattable... Ts>
	inline auto format(std::string_view _fmt, const Ts&... _args)
	{
		return fmt::vformat(_fmt, fmt::make_format_args(_args...));
	};

};

