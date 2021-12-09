#pragma once
#ifndef LAMBDEX_CHESS_BOARD_WITH_STATE_HPP
#define LAMBDEX_CHESS_BOARD_WITH_STATE_HPP

#include "piece_board.hpp"
#include "bit_board.hpp"

#include <jclib/config.h>

#include <array>
#include <iosfwd>
#include <format>
#include <string>
#include <optional>

namespace lbx::chess
{
	/**
	 * @brief Describes a board of pieces with game state
	*/
	class BoardWithState : public PieceBoard
	{
	public:

		std::optional<Square> en_passant = std::nullopt;
		bool black_can_castle_kingside = true;
		bool black_can_castle_queenside = true;
		bool white_can_castle_kingside = true;
		bool white_can_castle_queenside = true;
		Color turn = Color::white;


		// Pull down special member functions

		using PieceBoard::PieceBoard;
		using PieceBoard::operator=;

		constexpr BoardWithState(PieceBoard&& other) :
			PieceBoard{ std::move(other) }
		{};
		constexpr BoardWithState(const PieceBoard& other) :
			PieceBoard{ other }
		{};

	};

};

namespace std
{
	template <>
	struct formatter<lbx::chess::BoardWithState, char> :
		formatter<lbx::chess::PieceBoard, char>
	{
		using formatter<lbx::chess::PieceBoard, char>::formatter;
		using formatter<lbx::chess::PieceBoard, char>::format;
	};
};

#endif // LAMBDEX_CHESS_BOARD_WITH_STATE_HPP