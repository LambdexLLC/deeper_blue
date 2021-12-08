#pragma once

/*
	Defines helper functions for printing formatted IO
*/

#include "format.hpp"

#include <jclib/concepts.h>

#include <cstdio>
#include <ostream>
#include <iostream>

namespace lbx
{
	/**
	 * @brief Writes formatted output to an ostream
	 * @param _ostr Output stream to write to
	 * @param _fmt Format string
	 * @param ..._args Formatted arguements
	 * @return Output stream (_ostr)
	*/
	template <cx_formattable... Ts>
	inline std::ostream& write(std::ostream& _ostr, std::string_view _fmt, const Ts&... _args)
	{
		return _ostr << format(_fmt, _args...);
	};

	/**
	 * @brief Writes formatted output to an ostream and appends a newline
	 * @param _ostr Output stream to write to
	 * @param _fmt Format string
	 * @param ..._args Formatted arguements
	 * @return Output stream (_ostr)
	*/
	template <cx_formattable... Ts>
	inline std::ostream& writeln(std::ostream& _ostr, std::string_view _fmt, const Ts&... _args)
	{
		return _ostr << format(_fmt, _args...) << '\n';
	};

	/**
	 * @brief Writes formatted output to the standard output stream
	 * @param _fmt Format string
	 * @param ..._args Formatted arguements
	*/
	template <cx_formattable... Ts>
	inline void print(std::string_view _fmt, const Ts&... _args)
	{
		using char_type = char;
		const std::basic_string<char_type> _formattedString = format(_fmt, _args...);
		std::fwrite(_formattedString.data(), sizeof(char_type), _formattedString.size(), stdout);
	};

	/**
	 * @brief Writes formatted output to the standard output stream and appends a newline
	 * @param _fmt Format string
	 * @param ..._args Formatted arguements
	*/
	template <cx_formattable... Ts>
	inline void println(std::string_view _fmt, const Ts&... _args)
	{
		using char_type = char;
		const std::basic_string<char_type> _formattedString = format(_fmt, _args...);
		std::fwrite(_formattedString.data(), sizeof(char_type), _formattedString.size(), stdout);
		std::fputc('\n', stdout);
	};

};