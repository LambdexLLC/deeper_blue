#pragma once

#include <jclib/concepts.h>
#include <random>

namespace lbx
{
	namespace random_impl
	{
		template <typename T>
		struct uniform_distribution_type;

		template <typename T> requires jc::cx_integer<T>
		struct uniform_distribution_type<T>
		{
			using type = std::uniform_int_distribution<T>;
		};

		template <typename T> requires jc::cx_floating_point<T>
		struct uniform_distribution_type<T>
		{
			using type = std::uniform_real_distribution<T>;
		};
	};

	template <typename T>
	using uniform_distribution = typename random_impl::uniform_distribution_type<T>::type;



	/**
	 * @brief Generates a random number using a random engine and distribution
	 * @tparam T Value type to generate
	 * @tparam EngineT Random number engine type to use, defaults to whatever the standard library provides
	 * @tparam DistributionT Value distribution type to use
	 * @param _engine Random number engine
	 * @param _dist Value distribution
	 * @return Random number
	*/
	template <typename T, typename EngineT = std::default_random_engine, typename DistributionT = uniform_distribution<T>>
	requires requires(EngineT& e, const DistributionT& d)
	{
		{ d(e) } -> jc::cx_convertible_to<T>;
	}
	constexpr inline T rand(EngineT& _engine, const DistributionT& _dist)
	{
		return _dist(_engine);
	};

	namespace random_impl
	{
		struct rand_engine_data
		{
			static inline std::random_device device{};
			static inline std::default_random_engine engine{ device() };
		};

		template <typename T>
		struct rand_data;
	
		template <jc::cx_integer T>
		struct rand_data<T> : public rand_engine_data
		{
			static inline uniform_distribution<T> dist
			{
				std::numeric_limits<T>::min(),
				std::numeric_limits<T>::max()
			};
		};
		template <jc::cx_floating_point T>
		struct rand_data<T> : public rand_engine_data
		{
			static inline uniform_distribution<T> dist
			{
				(T)-1.0,
				(T) 1.0
			};
		};
	};

	/**
	 * @brief Generates a random number using a random engine and distribution
	 * @tparam T Value type to generate
	 * @tparam EngineT Random number engine type to use, defaults to whatever the standard library provides
	 * @tparam DistributionT Value distribution type to use
	 * @param _engine Random number engine
	 * @param _dist Value distribution
	 * @return Random number
	*/
	template <typename T, typename DistributionT, typename EngineT = std::default_random_engine>
	requires requires(EngineT& e, const DistributionT& d)
	{
		{ d(e) } -> jc::cx_convertible_to<T>;
	}
	constexpr inline T rand(const DistributionT& _dist)
	{
		return _dist(random_impl::rand_engine_data::engine);
	};

	/**
	 * @brief Generates a random integer with uniform distribution
	 * @tparam T Integer type to generate
	 * @return Random integer between the max and min for the type
	*/
	template <jc::cx_integer T>
	constexpr inline T rand()
	{
		auto& _engine = random_impl::rand_data<T>::engine;
		auto& _dist = random_impl::rand_data<T>::dist;
		return rand<T>(_engine, _dist);
	};

	/**
	 * @brief Generates a random floating point number with uniform distribution
	 * @tparam T Floating point type to generate
	 * @return Random floating point value between -1 and 1
	*/
	template <jc::cx_floating_point T>
	constexpr inline T rand()
	{
		auto& _engine = random_impl::rand_data<T>::engine;
		auto& _dist = random_impl::rand_data<T>::dist;
		return rand<T>(_engine, _dist);
	};

	/**
	 * @brief Type that can be converted to a random value
	 * 
	 * Generates a new random value each time it is converted.
	*/
	class random_value_t
	{
	public:
		template <typename T> requires requires
		{
			{ lbx::rand<T>() } -> jc::cx_convertible_to<T>;
		}
		constexpr operator T() const noexcept { return  lbx::rand<T>(); };
		constexpr explicit random_value_t() noexcept = default;
	};

	/**
	 * @brief Value that can be converted to a new random value
	 *
	 * Generates a new random value each time it is converted.
	*/
	constexpr inline random_value_t random_value{};
};