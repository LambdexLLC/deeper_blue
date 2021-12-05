#pragma once

#include "generic/generic_board.hpp"
#include "basic.hpp"

#include <jclib/config.h>

#include <array>
#include <string>
#include <optional>
#include <format>

namespace lbx::chess
{
	/**
	 * @brief Describes a board of chess pieces
	*/
	class PieceBoard : public GenericBoard<Piece>
	{
	public:
		std::optional<PositionPair> find(Piece _piece) const
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

		using GenericBoard::GenericBoard;
		using GenericBoard::operator=;
	};

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

};

namespace std
{
	template <>
	struct formatter<lbx::chess::PieceBoard, char> :
		formatter<std::string, char>
	{
		auto format(const lbx::chess::PieceBoard& _board, auto& _ctx)
		{
			auto _str = lbx::chess::stringify_board(_board);
			_str.insert(_str.begin() + 64, '\n');
			_str.insert(_str.begin() + 56, '\n');
			_str.insert(_str.begin() + 48, '\n');
			_str.insert(_str.begin() + 40, '\n');
			_str.insert(_str.begin() + 32, '\n');
			_str.insert(_str.begin() + 24, '\n');
			_str.insert(_str.begin() + 16, '\n');
			_str.insert(_str.begin() + 8,  '\n');
			return formatter<std::string, char>::format(_str, _ctx);
		};
	};

	template <>
	struct formatter<lbx::chess::BoardWithState, char> :
		formatter<lbx::chess::PieceBoard, char>
	{
		using formatter<lbx::chess::PieceBoard, char>::formatter;
		using formatter<lbx::chess::PieceBoard, char>::format;
	};
};