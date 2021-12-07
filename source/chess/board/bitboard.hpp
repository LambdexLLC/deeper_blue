#pragma once

/*
	Provides an 8x8 representation of a chess board with each square
	represented by a bit (so board is 8 bytes wide).
*/

#include "chess/basic.hpp"

#include <bitset>
#include <cstdint>

namespace lbx::chess
{
	/**
	 * @brief representation of a chess board with each square holding a single binary value
	*/
	class BitBoard
	{
	public:

		/**
		 * @brief Gets the state of a square
		 * @param _pos Square to get state of
		 * @return Binary state value
		*/
		bool at(Square _pos) const noexcept
		{
			return this->bs_.test(_pos);
		};

		/**
		 * @brief Gets the state of a square
		 * @param _pos Square to get state of
		 * @return Binary state value
		*/
		auto at(Position _pos) const noexcept
		{
			return this->at(_pos.get());
		};

		/**
		 * @brief Gets the state of a square
		 * @param _pos Square to get state of
		 * @return Binary state value
		*/
		bool operator[](Square _pos) const noexcept
		{
			return this->at(_pos);
		};

		/**
		 * @brief Gets the state of a square
		 * @param _pos Square to get state of
		 * @return Binary state value
		*/
		auto operator[](Position _pos) const noexcept
		{
			return this->at(_pos);
		};

		/**
		 * @brief Sets a square's state
		 * @param _pos Position of the square to modify
		 * @param _value State to set the square to
		*/
		void set(Square _pos, bool _value) noexcept
		{
			JCLIB_ASSERT(_pos < 64);
			this->bs_.set(_pos, _value);
		};
		
		/**
		 * @brief Sets a square's state
		 * @param _pos Position of the square to modify
		 * @param _value State to set the square to
		*/
		auto set(Position _pos, bool _value) noexcept
		{
			return this->set(_pos.get(), _value);
		};

		/**
		 * @brief Sets a square's state to "true"
		 * @param _pos Position of the square to modify
		*/
		void set(Square _pos) noexcept
		{
			JCLIB_ASSERT(_pos < 64);
			this->set(_pos, true);
		};

		/**
		 * @brief Sets a square's state to "true"
		 * @param _pos Position of the square to modify
		*/
		auto set(Position _pos) noexcept
		{
			return this->set(_pos.get());
		};

		/**
		 * @brief Sets all bits to "true"
		*/
		void reset_all()
		{
			this->bs_.set();
		};

		/**
		 * @brief Sets a square's state to "false"
		 * @param _pos Position of the square to modify
		*/
		void reset(Square _pos) noexcept
		{
			JCLIB_ASSERT(_pos < 64);
			this->bs_.reset(_pos);
		};

		/**
		 * @brief Sets a square's state to "false"
		 * @param _pos Position of the square to modify
		*/
		auto reset(Position _pos) noexcept
		{
			return this->reset(_pos.get());
		};

		/**
		 * @brief Sets all bits to "false"
		*/
		void set_all()
		{
			this->bs_.reset();
		};

		/**
		 * @brief Flips all bits using bitwise NOT
		*/
		void flip_bits()
		{
			this->bs_.flip();
		};

		/**
		 * @brief Checks if any bits are set to "true"
		 * @return True if one or more bits are set, false otherwise
		*/
		bool any() const
		{
			return this->bs_.any();
		};

		/**
		 * @brief Checks if all bits are set to "true"
		 * @return True if all set, false otherwise
		*/
		bool all() const
		{
			return this->bs_.all();
		};

		/**
		 * @brief Checks if all bits are set to "false"
		 * @return True if all reset, false otherwise
		*/
		bool none() const
		{
			return this->bs_.none();
		};

#pragma region OPERATOR_OVERLOADS

		friend inline BitBoard operator~(const BitBoard& rhs) noexcept
		{
			return BitBoard{ ~rhs.bs_ };
		};
		
		friend inline BitBoard operator&(const BitBoard& lhs, const BitBoard& rhs) noexcept
		{
			return BitBoard{ lhs.bs_ & rhs.bs_ };
		};
		friend inline BitBoard operator|(const BitBoard& lhs, const BitBoard& rhs) noexcept
		{
			return BitBoard{ lhs.bs_ | rhs.bs_ };
		};
		friend inline BitBoard operator^(const BitBoard& lhs, const BitBoard& rhs) noexcept
		{
			return BitBoard{ lhs.bs_ ^ rhs.bs_ };
		};

		friend inline BitBoard& operator&=(BitBoard& lhs, const BitBoard& rhs) noexcept
		{
			lhs.bs_ &= rhs.bs_;
			return lhs;
		};
		friend inline BitBoard& operator|=(BitBoard& lhs, const BitBoard& rhs) noexcept
		{
			lhs.bs_ |= rhs.bs_;
			return lhs;
		};
		friend inline BitBoard& operator^=(BitBoard& lhs, const BitBoard& rhs) noexcept
		{
			lhs.bs_ ^= rhs.bs_;
			return lhs;
		};

		friend inline bool operator==(const BitBoard& lhs, const BitBoard& rhs) noexcept
		{
			return lhs.bs_ == rhs.bs_;
		};
		friend inline bool operator!=(const BitBoard& lhs, const BitBoard& rhs) noexcept
		{
			return lhs.bs_ != rhs.bs_;
		};

#pragma endregion OPERATOR_OVERLOADS


		constexpr BitBoard() = default;
		
		constexpr BitBoard(std::bitset<64>&& _bits) noexcept :
			bs_{ std::move(_bits) }
		{};
		constexpr BitBoard(const std::bitset<64>& _bits) :
			bs_{ _bits }
		{};

	private:

		/**
		 * @brief Bitset containing the squares' states
		*/
		std::bitset<64> bs_;

	};

	// Size should always be small at 8 bytes, otherwise what is the point?
	static_assert(sizeof(BitBoard) == 8);

};