#pragma once

#include "basic.hpp"

#include <jclib/config.h>

#include <array>
#include <string>
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


		std::optional<PositionPair> find(Piece _piece) const
		{
			Position p{};
			for (auto _square : this->board)
			{
				if (_square == _piece)
				{
					return p;
				};
				p = p + 1;
			};
			return std::nullopt;
		};


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
			board[i + 56] = default_line[i] | Color::black;
			board[i + 48] = Piece::pawn | Color::black;
			board[i + 8] = Piece::pawn | Color::white;
			board[i] = default_line[i] | Color::white;
		};

		return output;
	};


	inline std::string stringify_board(const Board& _board)
	{
		std::string _out{};
		_out.resize(64, ' ');

		for (Rank r{}; r != Rank::END; r = r + 1)
		{
			for (File f{}; f != File::END; f = f + 1)
			{
				const auto _at = (Rank(Rank::r8 - r), f);
				char _char{};
				const auto _piece = _board[_at];
				
				switch (_piece)
				{
				case Piece::king_black:
					_char = 'k';
					break;
				case Piece::king_white:
					_char = 'K';
					break;
				case Piece::knight_black:
					_char = 'n';
					break;
				case Piece::knight_white:
					_char = 'N'; 
					break;
				case Piece::pawn_black:
					_char = 'p';
					break;
				case Piece::pawn_white:
					_char = 'P';
					break;
				case Piece::bishop_black:
					_char = 'b';
					break;
				case Piece::bishop_white:
					_char = 'B';
					break;
				case Piece::rook_white:
					_char = 'R'; 
					break;
				case Piece::rook_black:
					_char = 'r';
					break;
				case Piece::queen_white:
					_char = 'Q';
					break;
				case Piece::queen_black:
					_char = 'q';
					break;
				case Piece::empty:
					_char = ' ';
					break;
				default:
					JCLIB_ABORT();
				};

				const auto _strPos = Position{ (r, f) }.get();
				_out.at(_strPos) = _char;
			};
		};

		return _out;
	};
};