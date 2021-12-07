#pragma once

#include "basic.hpp"

#include <string>
#include <format>
#include <charconv>

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

#pragma region MOVE_FORMATTER
namespace std
{
	template <>
	struct formatter<lbx::chess::Move> : public formatter<std::string, char>
	{
		auto format(const lbx::chess::Move& _move, auto& _ctx)
		{
			const auto _str = _move.to_string();
			return formatter<std::string, char>::format(_str, _ctx);
		};
	};
};
#pragma endregion MOVE_FORMATTER
