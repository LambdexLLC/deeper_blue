#pragma once

/*
	Defines a generic board container
*/

#include <lambdex/chess/basic.hpp>
#include <lambdex/chess/position.hpp>

#include <jclib/concepts.h>

#include <array>
#include <ranges>
#include <cstdint>
#include <algorithm>

namespace lbx::chess
{
	/**
	 * @brief Generic type for holding an 8x8 grid of values
	 * @tparam T Type to hold in the board
	*/
	template <typename T>
	class GenericBoard
	{
	public:

		/**
		 * @brief The type held in this board
		*/
		using value_type = T;
		using pointer = value_type*;
		using reference = value_type&;
		using const_pointer = const value_type*;
		using const_reference = const value_type&;

		/**
		 * @brief Type used to hold size values
		*/
		using size_type = uint8_t;

		// Required to help fufill "Container" named requirement
		using difference_type = std::ptrdiff_t;

	private:

		/**
		 * @brief Internal container type for holding the board data
		*/
		using container_type = std::array<value_type, 64>;

	public:

		// Iterator support

		using iterator = typename container_type::iterator;
		using const_iterator = typename container_type::const_iterator;
		
		constexpr iterator begin() noexcept
		{
			return this->data_.begin();
		};
		constexpr const_iterator begin() const noexcept
		{
			return this->data_.cbegin();
		};
		constexpr const_iterator cbegin() const noexcept
		{
			return this->data_.cbegin();
		};

		constexpr iterator end() noexcept
		{
			return this->data_.end();
		};
		constexpr const_iterator end() const noexcept
		{
			return this->data_.cend();
		};
		constexpr const_iterator cend() const noexcept
		{
			return this->data_.cend();
		};
		

		// Reverse iterator support
		
		using reverse_iterator = typename container_type::reverse_iterator;
		using const_reverse_iterator = typename container_type::const_reverse_iterator;

		constexpr reverse_iterator rbegin() noexcept
		{
			return this->data_.rbegin();
		};
		constexpr const_reverse_iterator rbegin() const noexcept
		{
			return this->data_.crbegin();
		};
		constexpr const_reverse_iterator crbegin() const noexcept
		{
			return this->data_.crbegin();
		};

		constexpr reverse_iterator rend() noexcept
		{
			return this->data_.rend();
		};
		constexpr const_reverse_iterator rend() const noexcept
		{
			return this->data_.crend();
		};
		constexpr const_reverse_iterator crend() const noexcept
		{
			return this->data_.crend();
		};


		// Size related container functions

		constexpr static size_type size() noexcept
		{
			return 64;
		};
		constexpr static size_type max_size() noexcept
		{
			return 64;
		};
		constexpr static size_type capacity() noexcept
		{
			return 64;
		};

		constexpr static bool empty() noexcept
		{
			return false;
		};

		
		// Data access functions
		
		constexpr pointer data() noexcept
		{
			return this->data_.data();
		};
		constexpr const_pointer data() const noexcept
		{
			return this->data_.data();
		};

		constexpr reference front() noexcept
		{
			return this->data_.front();
		};
		constexpr const_reference front() const noexcept
		{
			return this->data_.front();
		};

		constexpr reference back() noexcept
		{
			return this->data_.back();
		};
		constexpr const_reference back() const noexcept
		{
			return this->data_.back();
		};

		/**
		 * @brief Gets the piece at a given board position
		 * @param _pos Position to get piece from
		 * @return Piece at position
		*/
		constexpr reference at(size_type _pos)
		{
			JCLIB_ASSERT(_pos < this->size());
			return *(this->data() + _pos);
		};

		/**
		 * @brief Gets the piece at a given board position
		 * @param _pos Position to get piece from
		 * @return Piece at position
		*/
		constexpr const_reference at(size_type _pos) const
		{
			JCLIB_ASSERT(_pos < this->size());
			return *(this->data() + _pos);
		};

		/**
		 * @brief Gets the piece at a given board position
		 * @param _pos Position to get piece from
		 * @return Piece at position
		*/
		constexpr reference operator[](size_type _pos)
		{
			return this->at(_pos);
		};

		/**
		 * @brief Gets the piece at a given board position
		 * @param _pos Position to get piece from
		 * @return Piece at position
		*/
		constexpr const_reference operator[](size_type _pos) const
		{
			return this->at(_pos);
		};

		/**
		 * @brief Gets the piece at a given board position
		 * @param _pos Position to get piece from
		 * @return Piece at position
		*/
		constexpr reference at(Position _pos)
		{
			return this->at(_pos.get());
		};

		/**
		 * @brief Gets the piece at a given board position
		 * @param _pos Position to get piece from
		 * @return Piece at position
		*/
		constexpr const_reference at(Position _pos) const
		{
			return this->at(_pos.get());
		};

		/**
		 * @brief Gets the piece at a given board position
		 * @param _pos Position to get piece from
		 * @return Piece at position
		*/
		constexpr reference operator[](Position _pos)
		{
			return this->at(_pos);
		};

		/**
		 * @brief Gets the piece at a given board position
		 * @param _pos Position to get piece from
		 * @return Piece at position
		*/
		constexpr const_reference operator[](Position _pos) const
		{
			return this->at(_pos);
		};


		// Container manipulation functions

		constexpr void fill(const_reference& _value)
		{
			std::ranges::fill(this->data_, _value);
		};



		// Special member functions


		constexpr GenericBoard() = default;
	
		constexpr GenericBoard(const std::array<value_type, 64>&_board) :
			data_{ _board }
		{};
		constexpr GenericBoard(std::array<value_type, 64>&& _board) :
			data_{ std::move(_board) }
		{};
		
		constexpr explicit GenericBoard(const_reference _fillValue) :
			data_{}
		{
			this->fill(_fillValue);
		};

		// Constructs the board from an iterator pair
		template <typename IterT> requires requires(IterT _it)
		{
			{ _it != _it } -> jc::cx_same_as<bool>;
			{ *_it } -> jc::cx_convertible_to<value_type>;
			{ ++_it } -> jc::cx_convertible_to<IterT>;
		}
		constexpr explicit GenericBoard(IterT _begin, IterT _end) :
			data_{}
		{
			std::copy(_begin, _end, this->begin());
		};


	private:

		/**
		 * @brief Contains the values for the board
		*/
		container_type data_;

	};
};
