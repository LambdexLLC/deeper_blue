#pragma once

#include "basic.hpp"

#include <jclib/config.h>

#include <array>
#include <optional>

namespace lbx::chess
{
	struct Board
	{
		std::array<Piece, 64> board{};
		std::optional<Square> en_passant = std::nullopt;
		bool black_can_castle_kingside = true;
		bool black_can_castle_queenside = true;
		bool white_can_castle_kingside = true;
		bool white_can_castle_queenside = true;
		Color turn = Color::white;

		/**
		 * @brief Gets the piece at a given board position
		 * @param _pos Position to get piece from
		 * @return Piece at position
		*/
		constexpr Piece& at(Position _pos) 
		{
			JCLIB_ASSERT(_pos.get() < 64);
			return this->board.at(_pos.get());
		};

		/**
		 * @brief Gets the piece at a given board position
		 * @param _pos Position to get piece from
		 * @return Piece at position
		*/
		constexpr const Piece& at(Position _pos) const
		{
			JCLIB_ASSERT(_pos.get() < 64);
			return this->board.at(_pos.get());
		};

		/**
		 * @brief Gets the piece at a given board position
		 * @param _pos Position to get piece from
		 * @return Piece at position
		*/
		constexpr Piece& operator[](Position _pos)
		{
			return this->at(_pos);
		};

		/**
		 * @brief Gets the piece at a given board position
		 * @param _pos Position to get piece from
		 * @return Piece at position
		*/
		constexpr const Piece& operator[](Position _pos) const
		{
			return this->at(_pos);
		};
	};

	constexpr inline Board make_standard_board()
	{
		constexpr auto default_line = std::array<Piece, 8>
		{
			Piece::rook, Piece::knight, Piece::bishop, Piece::queen, Piece::king, Piece::bishop, Piece::knight, Piece::rook
		};

		Board output{};
		auto& board = output.board;

		for (int i = 0; i < 8; i++)
		{
			board[i + 55] = default_line[i] | Color::black;
			board[i + 47] = Piece::pawn | Color::black;
			board[i + 8] = Piece::pawn | Color::white;
			board[i] = default_line[i] | Color::white;
		};

		return output;
	};
};