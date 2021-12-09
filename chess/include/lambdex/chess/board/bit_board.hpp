#pragma once
#ifndef LAMBDEX_CHESS_BIT_BOARD_HPP
#define LAMBDEX_CHESS_BIT_BOARD_HPP

/*
	Provides an 8x8 representation of a chess board with each square
	represented by a bit (so board is 8 bytes wide).
*/

#include "lambdex/chess/basic.hpp"

#include <span>
#include <array>
#include <ranges>
#include <cstdint>

namespace lbx::chess
{
	/**
	 * @brief representation of a chess board with each square holding a single binary value
	*/
	class BitBoard
	{
	public:
		using binary_type = uint64_t;
		using size_type = uint8_t;

	private:

		/**
		 * @brief Makes a mask for a specific bit
		 * @param _bitPos Bit position to make mask for
		 * @return Bit mask
		*/
		constexpr static binary_type make_bit_mask(size_type _bitPos) noexcept
		{
			return ((binary_type)0b1) << _bitPos;
		};

	public:

		/**
		 * @brief Gets the raw bit data for this board
		 * @return Unsigned int containing binary data
		*/
		constexpr binary_type bits() const noexcept
		{
			return this->bits_;
		};

		/**
		 * @brief Gets the state of a square
		 * @param _pos Square to get state of
		 * @return Binary state value
		*/
		constexpr bool at(size_type _pos) const noexcept
		{
			return this->bits() & this->make_bit_mask(_pos);
		};

		/**
		 * @brief Gets the state of a square
		 * @param _pos Square to get state of
		 * @return Binary state value
		*/
		constexpr auto at(Position _pos) const noexcept
		{
			return this->at(_pos.get());
		};

		/**
		 * @brief Gets the state of a square
		 * @param _pos Square to get state of
		 * @return Binary state value
		*/
		constexpr bool operator[](size_type _pos) const noexcept
		{
			return this->at(_pos);
		};

		/**
		 * @brief Gets the state of a square
		 * @param _pos Square to get state of
		 * @return Binary state value
		*/
		constexpr auto operator[](Position _pos) const noexcept
		{
			return this->at(_pos);
		};

		/**
		 * @brief Sets a square's state
		 * @param _pos Position of the square to modify
		 * @param _value State to set the square to
		*/
		constexpr void set(size_type _pos, bool _value) noexcept
		{
			JCLIB_ASSERT(_pos < 64);
			const auto _mask = this->make_bit_mask(_pos);
			if (_value)
			{
				this->bits_ |= _mask;
			}
			else
			{
				this->bits_ &= ~_mask;
			};
		};
		
		/**
		 * @brief Sets a square's state
		 * @param _pos Position of the square to modify
		 * @param _value State to set the square to
		*/
		constexpr auto set(Position _pos, bool _value) noexcept
		{
			return this->set(_pos.get(), _value);
		};

		/**
		 * @brief Sets a square's state to "true"
		 * @param _pos Position of the square to modify
		*/
		constexpr void set(size_type _pos) noexcept
		{
			JCLIB_ASSERT(_pos < 64);
			const auto _mask = this->make_bit_mask(_pos);
			this->bits_ |= _mask;
		};

		/**
		 * @brief Sets a square's state to "true"
		 * @param _pos Position of the square to modify
		*/
		constexpr auto set(Position _pos) noexcept
		{
			return this->set(_pos.get());
		};

		/**
		 * @brief Sets all bits to "true"
		*/
		constexpr void set_all()
		{
			this->bits_ = static_cast<binary_type>(-1);
		};

		/**
		 * @brief Sets a square's state to "false"
		 * @param _pos Position of the square to modify
		*/
		constexpr void reset(size_type _pos) noexcept
		{
			JCLIB_ASSERT(_pos < 64);
			const auto _mask = this->make_bit_mask(_pos);
			this->bits_ &= ~_mask;
		};

		/**
		 * @brief Sets a square's state to "false"
		 * @param _pos Position of the square to modify
		*/
		constexpr auto reset(Position _pos) noexcept
		{
			return this->reset(_pos.get());
		};

		/**
		 * @brief Sets all bits to "false"
		*/
		constexpr void reset_all()
		{
			this->bits_ = 0;
		};

		/**
		 * @brief Flips all bits using bitwise NOT
		*/
		constexpr void flip_bits()
		{
			this->bits_ = ~this->bits();
		};

		/**
		 * @brief Checks if any bits are set to "true"
		 * @return True if one or more bits are set, false otherwise
		*/
		constexpr bool any() const
		{
			return this->bits() != 0;
		};

		/**
		 * @brief Checks if all bits are set to "true"
		 * @return True if all set, false otherwise
		*/
		constexpr bool all() const
		{
			return this->bits() == static_cast<binary_type>(-1);
		};

		/**
		 * @brief Checks if all bits are set to "false"
		 * @return True if all reset, false otherwise
		*/
		constexpr bool none() const
		{
			return this->bits() == 0;
		};



#pragma region OPERATOR_OVERLOADS

	public:

		friend constexpr inline BitBoard operator~(const BitBoard& rhs) noexcept
		{
			return BitBoard{ ~rhs.bits() };
		};
		
		friend constexpr inline BitBoard operator&(const BitBoard& lhs, const BitBoard& rhs) noexcept
		{
			return BitBoard{ lhs.bits() & rhs.bits() };
		};
		friend constexpr inline BitBoard operator|(const BitBoard& lhs, const BitBoard& rhs) noexcept
		{
			return BitBoard{ lhs.bits() | rhs.bits() };
		};
		friend constexpr inline BitBoard operator^(const BitBoard& lhs, const BitBoard& rhs) noexcept
		{
			return BitBoard{ lhs.bits() ^ rhs.bits() };
		};

		friend constexpr inline BitBoard& operator&=(BitBoard& lhs, const BitBoard& rhs) noexcept
		{
			lhs.bits_ &= rhs.bits();
			return lhs;
		};
		friend constexpr inline BitBoard& operator|=(BitBoard& lhs, const BitBoard& rhs) noexcept
		{
			lhs.bits_ |= rhs.bits();
			return lhs;
		};
		friend constexpr inline BitBoard& operator^=(BitBoard& lhs, const BitBoard& rhs) noexcept
		{
			lhs.bits_ ^= rhs.bits();
			return lhs;
		};

		friend constexpr inline bool operator==(const BitBoard& lhs, const BitBoard& rhs) noexcept
		{
			return lhs.bits() == rhs.bits();
		};
		friend constexpr inline bool operator!=(const BitBoard& lhs, const BitBoard& rhs) noexcept
		{
			return lhs.bits() != rhs.bits();
		};

#pragma endregion OPERATOR_OVERLOADS

		constexpr BitBoard() = default;
		constexpr explicit BitBoard(binary_type _bits) :
			bits_{ _bits }
		{};

		constexpr explicit BitBoard(std::span<const uint8_t, 8> _bytes)
		{
			binary_type bn = 0;
			for (auto& b : _bytes)
			{
				this->bits_ |= (static_cast<binary_type>(b) << bn);
				bn += 8;
			};
		};

	private:

		/**
		 * @brief Binary storage
		*/
		binary_type bits_{ 0 };

	};

	// Size should always be small at 8 bytes, otherwise what is the point?
	static_assert(sizeof(BitBoard) == 8);

};

#endif // LAMBDEX_CHESS_BIT_BOARD_HPP