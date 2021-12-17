#pragma once
#ifndef LAMBDEX_UTILITY_FORMAT_HPP
#define LAMBDEX_UTILITY_FORMAT_HPP

namespace lbx
{
	/**
	 * @brief Customization point for adding a custom formatter type.
	 *
	 * Types with a formatter specialized and implemented will work with
	 * both libfmt and std::format.
	 *
	 * @tparam T Type to specialize.
	 * @tparam Enable Optional SFINAE specialization point.
	*/
	template <typename T, typename Enable = void>
	struct formatter;
};

#endif // LAMBDEX_UTILITY_FORMAT_HPP