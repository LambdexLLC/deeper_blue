#pragma once
#ifndef LAMBDEX_CHESS_BOARD_WITH_STATE_HPP
#define LAMBDEX_CHESS_BOARD_WITH_STATE_HPP

#include "piece_board.hpp"
#include "bit_board.hpp"

#include <jclib/config.h>

#include <array>
#include <iosfwd>
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

		/**
		 * @brief Checks if an en passant is possible
		 * @return True if possible false otherwise
		*/
		constexpr bool has_en_passant() const
		{
			return this->en_passant_ != Position::end();
		};

		/**
		 * @brief Gets the square behind a pawn that can be 
		 * en-passant-ed.
		 * 
		 * has_en_passant() MUST RETURN TRUE.
		 * 
		 * @return Board position
		*/
		constexpr Position get_en_passant() const
		{
			JCLIB_ASSERT(this->has_en_passant());
			return this->en_passant_;
		};

		/**
		 * @brief Sets the square behind a pawn that can be 
		 * en-passant-ed.
		 * 
		 * @param _pos Square behind pawn
		*/
		constexpr void set_en_passant(Position _pos)
		{
			this->en_passant_ = _pos;
		};

		/**
		 * @brief Clears the en passant flag such that has_en_passant returns false
		*/
		constexpr void clear_en_passant()
		{
			this->en_passant_ = Position::end();
		};

		/**
		 * @brief Checks if a player can castle queen and/or king side.
		 * @param _player Player to check for.
		 * @return True if player can castle, false otherwise.
		*/
		constexpr bool can_player_castle(Color _player) const noexcept
		{
			if (_player == Color::black)
			{
				return this->black_can_castle_kingside || this->black_can_castle_queenside;
			}
			else
			{
				return this->white_can_castle_kingside || this->white_can_castle_queenside;
			};
		};

		/**
		 * @brief Checks if a player can castle king side.
		 * @param _player Player to check for.
		 * @return True if player can castle kingside, false otherwise.
		*/
		constexpr bool can_player_castle_kingside(Color _player) const noexcept
		{
			if (_player == Color::black)
			{
				return this->black_can_castle_kingside;
			}
			else
			{
				return this->white_can_castle_kingside;
			};
		};

		/**
		 * @brief Checks if a player can castle queen side.
		 * @param _player Player to check for.
		 * @return True if player can castle queenside, false otherwise.
		*/
		constexpr bool can_player_castle_queenside(Color _player) const noexcept
		{
			if (_player == Color::black)
			{
				return this->black_can_castle_queenside;
			}
			else
			{
				return this->white_can_castle_queenside;
			};
		};





		bool black_can_castle_kingside = true;
		bool black_can_castle_queenside = true;
		bool white_can_castle_kingside = true;
		bool white_can_castle_queenside = true;
		Color turn = Color::white;

		/**
		 * @brief The number of halfmoves since the last capture or pawn advance
		*/
		uint16_t half_move_counter = 0;

		/**
		 * @brief The number of the full moves played (kinda minus 1)
		 *
		 * Starts at one and begins incrementing after black plays their first move
		*/
		uint16_t full_move_counter = 1;


		// Pull down special member functions

		using PieceBoard::PieceBoard;
		using PieceBoard::operator=;

		constexpr BoardWithState(PieceBoard&& other) :
			PieceBoard{ std::move(other) }
		{};
		constexpr BoardWithState(const PieceBoard& other) :
			PieceBoard{ other }
		{};


	private:

		/**
		 * @brief En passant position
		 * 
		 * Position behind the square of a pawn that
		 * can be en-passant-ed, if no en passant is possible,
		 * this is just Position::end()
		*/
		Position en_passant_ = Position::end();

	};
};

namespace lbx
{
	template <>
	struct formatter<chess::BoardWithState> : public formatter<lbx::chess::PieceBoard>
	{
		using formatter<chess::PieceBoard>::formatter;
		using formatter<chess::PieceBoard>::format;
	};
};

#endif // LAMBDEX_CHESS_BOARD_WITH_STATE_HPP