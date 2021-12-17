#pragma once
#ifndef LAMBDEX_CHESS_PIECE_BOARD_HPP
#define LAMBDEX_CHESS_PIECE_BOARD_HPP

#include "bit_board.hpp"
#include "generic_board.hpp"

#include "lambdex/chess/basic.hpp"

#include <lambdex/utility/format.hpp>

#include <jclib/config.h>

#include <array>
#include <string>
#include <iosfwd>
#include <optional>

namespace lbx::chess
{
	/**
	 * @brief Describes a board of chess pieces
	*/
	class PieceBoard : public GenericBoard<Piece>
	{
	public:

		constexpr std::optional<PositionPair> find(Piece _piece) const
		{
			Position p{};
			for (auto _square : *this)
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
		 * @brief Makes a bit board with each square set to true if a piece is present
		 * @return Bit board
		*/
		constexpr BitBoard as_bits_with_pieces() const
		{
			BitBoard _out{};
			Position p{};
			for (auto& s : *this)
			{
				_out.set(p, s != Piece::empty);
				++p;
			};
			return _out;
		};

		/**
		 * @brief Makes a bit board with each square set to true if a piece of the player is present
		 * @param _player Player to get pieces of
		 * @return Bit board
		*/
		constexpr BitBoard as_bits_with_pieces(Color _player) const
		{
			BitBoard _out{};
			Position p{};
			for (auto& s : *this)
			{
				_out.set(p, s != Piece::empty && get_color(s) == _player);
				++p;
			};
			return _out;
		};

		/**
		 * @brief Gets the number of pieces on the board
		 * @return Number of non-empty squares
		*/
		constexpr size_t count_pieces() const noexcept
		{
			size_t _count = 0;
			for (auto& v : *this)
			{
				if (v != Piece::empty)
				{
					++_count;
				};
			};
			return _count;
		};


		using GenericBoard::GenericBoard;
		using GenericBoard::operator=;
	};

	/**
	 * @brief Creates a chess board with pieces in the standard starting positions
	 * @return Chess board with pieces
	*/
	constexpr inline PieceBoard make_standard_board()
	{
		constexpr auto default_line = std::array<Piece, 8>
		{
			Piece::rook, Piece::knight, Piece::bishop, Piece::queen, Piece::king, Piece::bishop, Piece::knight, Piece::rook
		};

		PieceBoard _board{};

		for (uint8_t i = 0; i < 8; i++)
		{
			_board[i + 56] = default_line[i] | Color::black;
			_board[i + 48] = Piece::pawn | Color::black;
			_board[i + 8] = Piece::pawn | Color::white;
			_board[i] = default_line[i] | Color::white;
		};

		return _board;
	};

	inline std::string stringify_board(const PieceBoard& _board)
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

	std::ostream& operator<<(std::ostream& _ostr, const PieceBoard& _board);


	static_assert([]() -> bool
		{
			auto _board = make_standard_board();
			if (_board[(Rank::r8, File::d)] != Piece::queen_black)
			{
				return false;
			};
			if (_board[Position((Rank::r8, File::d))] != Piece::queen_black)
			{
				return false;
			};


			return true;
		}());
};

namespace lbx
{
	template <>
	struct formatter<chess::PieceBoard>
	{
		auto format(const chess::PieceBoard& _board)
		{
			auto _str = chess::stringify_board(_board);
			_str.insert(_str.begin() + 64, '\n');
			_str.insert(_str.begin() + 56, '\n');
			_str.insert(_str.begin() + 48, '\n');
			_str.insert(_str.begin() + 40, '\n');
			_str.insert(_str.begin() + 32, '\n');
			_str.insert(_str.begin() + 24, '\n');
			_str.insert(_str.begin() + 16, '\n');
			_str.insert(_str.begin() + 8,  '\n');
			return _str;
		};
	};
};

#endif // LAMBDEX_CHESS_PIECE_BOARD_HPP