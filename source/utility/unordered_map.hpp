#pragma once

/*
	Alias of std::unordered_map that automatically provides
	better key handling
*/

#include <jclib/concepts.h>
#include <jclib/functional.h>

#include <unordered_map>

namespace lbx
{
	namespace impl
	{
		template <typename T>
		concept cx_not_void = !jc::cx_same_as<void, T>;
	};

	/**
	 * @brief Non-cryptographic hashing function object type, invokes std::hash
	*/
	struct hash_t : jc::operator_tag
	{
		/**
		 * @brief Mark as transparent for use in unordered_map
		*/
		using is_transparent = void;

		template <typename T> requires requires(const T& v)
		{
			{ std::hash<T>{}(v) } -> impl::cx_not_void;
		}
		constexpr auto operator()(const T& _value) const
		{
			return std::hash<T>{}(_value);
		};
		constexpr auto operator()(jc::wildcard _wc) const
		{
			return _wc;
		};
	};

	/**
	 * @brief Non-cryptographic hashing function object, invokes std::hash
	*/
	constexpr inline hash_t hash{};

	/**
	 * @brief Basic key comparison function object type, invokes jc::equals
	*/
	struct key_compare_t : jc::operator_tag
	{
		/**
		 * @brief Mark as transparent for use in unordered_map
		*/
		using is_transparent = void;

		template <typename T, typename U> requires jc::has_operator_v<jc::equals_t, T, U>
		constexpr bool operator()(const T& lhs, const U& rhs) const
		{
			return lhs == rhs;
		};
	
	};

	/**
	 * @brief Basic key comparison function object, invokes jc::equals
	*/
	constexpr inline key_compare_t key_compare{};

	/**
	 * @brief Same as std::unordered_map but has transparent hasher and key compare by default
	 * @tparam KeyT Key type
	 * @tparam T Value type for the map
	 * @tparam HasherT Hash function object type for the keys
	 * @tparam KeyCompareT Comparison function object type for keys
	*/
	template <typename KeyT, typename T, typename HasherT = hash_t, typename KeyCompareT = key_compare_t>
	using unordered_map = std::unordered_map<KeyT, T, HasherT, KeyCompareT>;

};