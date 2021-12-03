#pragma once

#include "basic.hpp"

#include <string>
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

	inline std::from_chars_result from_chars(char* _begin, const char* _end, Move& _value)
	{
		auto _result = from_chars(_begin, _end, _value.from);
		if (_result.ec != std::errc{})
		{
			// Error occured
			return _result;
		};

		return from_chars(_result.ptr, _end, _value.to);
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

}