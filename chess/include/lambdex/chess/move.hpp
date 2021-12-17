#pragma once
#ifndef LAMBDEX_CHESS_MOVE_HPP
#define LAMBDEX_CHESS_MOVE_HPP

#include "basic.hpp"
#include "position.hpp"

#include <lambdex/utility/format.hpp>

#include <array>
#include <string>
#include <format>
#include <charconv>
#include <string_view>

namespace lbx::chess
{
	/**
	 * @brief Holds a chess piece move
	*/
	struct Move
	{
		/**
		 * @brief Converts to a string, use to_chars() if you do not want to allocate
		 * @return String form of the move
		*/
		std::string to_string() const;

		/**
		 * @brief Square a piece was moved from
		*/
		PositionPair from{};

		/**
		 * @brief Square the piece was moved to
		*/
		PositionPair to{};

		/**
		 * @brief Piece that a pawn was promoted to, this will be set to Piece::empty unless
		 * a promotion actually occured
		*/
		Piece promotion = Piece::empty;
	};

	constexpr inline bool operator==(const Move& lhs, const Move& rhs) noexcept
	{
		return
			lhs.from == rhs.from &&
			lhs.to == rhs.to &&
			lhs.promotion == rhs.promotion;
	};
	constexpr inline bool operator!=(const Move& lhs, const Move& rhs) noexcept
	{
		return !(lhs == rhs);
	};




	/**
	 * @brief Array of positions with pseudo-dynamic sizing using an end sentinal
	*/
	template <size_t MaxSize>
	struct PositionArray
	{
	public:

		constexpr auto begin()
		{
			return this->data_.begin();
		};
		constexpr auto begin() const
		{
			return this->data_.cbegin();
		};
		constexpr auto cbegin() const
		{
			return this->begin();
		};

		constexpr auto end()
		{
			return std::ranges::find(this->data_, PositionPair::end());
		};
		constexpr auto end() const
		{
			return std::ranges::find(this->data_, PositionPair::end());
		};
		constexpr auto cend() const
		{
			return this->end();
		};

		constexpr size_t size() const noexcept
		{
			return this->end() - this->begin();
		};
		constexpr static size_t max_size() noexcept
		{
			return MaxSize;
		};

		constexpr void push_back(PositionPair _pos) noexcept
		{
			auto _end = this->end();
			JCLIB_ASSERT(_end != this->data_.end());
			*_end = _pos;
		};

		constexpr PositionArray() :
			data_{}
		{
			this->data_.fill(PositionPair::end());
		};

	private:

		std::array<PositionPair, MaxSize> data_;
	
	};


	



};

#pragma region STRING_CONVERSIONS
namespace lbx::chess
{

	inline std::from_chars_result from_chars(const char* _begin, const char* _end, Move& _value)
	{
		auto _result = from_chars(_begin, _end, _value.from);
		if (_result.ec != std::errc{})
		{
			// Error occured
			return _result;
		};

		_result = from_chars(_result.ptr, _end, _value.to);
		if (_result.ec != std::errc{})
		{
			// Error occured
			return _result;
		};

		if (_result.ptr != _end)
		{
			// Promo!
			switch (*_result.ptr)
			{
			case 'q':
				_value.promotion = Piece::queen;
				++_result.ptr;
				break;
			case 'r':
				_value.promotion = Piece::rook;
				++_result.ptr;
				break;
			case 'b':
				_value.promotion = Piece::bishop;
				++_result.ptr;
				break;
			case 'k':
				_value.promotion = Piece::king;
				++_result.ptr;
				break;
			default:
				break;
			};
		};

		return _result;
	};
	inline std::from_chars_result from_chars(std::string_view _str, Move& _value)
	{
		return from_chars(_str.data(), _str.data() + _str.size(), _value);
	};

	inline std::to_chars_result to_chars(char* _begin, char* _end, const Move& _value)
	{
		auto _result = to_chars(_begin, _end, _value.from);
		if (_result.ec != std::errc{})
		{
			return _result;
		};
		return to_chars(_result.ptr, _end, _value.to);
	};

	/**
	 * @brief Converts to a string, use to_chars() if you do not want to allocate
	 * @return String form of the move
	*/
	inline std::string Move::to_string() const
	{
		std::string _buffer(4, '\0');
		auto _result = to_chars(_buffer.data(), _buffer.data() + _buffer.size(), *this);
		JCLIB_ASSERT(_result.ec == std::errc{});
		_buffer.resize(_result.ptr - _buffer.data());
		return _buffer;
	};

};
#pragma endregion STRING_CONVERSIONS

#pragma region MOVE_FORMATTER
namespace lbx
{
	template <>
	struct formatter<chess::Move>
	{
		auto format(const chess::Move& _move)
		{
			return _move.to_string();
		};
	};
};
#pragma endregion MOVE_FORMATTER

#endif // LAMBDEX_CHESS_MOVE_HPP