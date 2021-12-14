#include <lambdex/chess/chess.hpp>
#include <lambdex/chess/piece_movement.hpp>
#include <lambdex/chess/move_validation.hpp>
#include <lambdex/chess/fen.hpp>

#include <jclib-test.hpp>

#include <ranges>
#include <algorithm>
#include <format>

using namespace lbx::chess;




int subtest_rook()
{
	NEWTEST();

	BoardWithState _baseBoard{};

	{
		const auto _rookPos = (File::a, Rank::r1);
		const auto _rookDest = (File::a, Rank::r8);

		auto _board = _baseBoard;
		_board[_rookPos] = Piece::rook_white;
		
		Move _move{ _rookPos, _rookDest };
		{
			const auto _validity = is_move_valid(_board, _move, Color::white);
			ASSERT(_validity == MoveValidity::valid, "clear rook move considered invalid");
		};

		// Add a pawn in the way
		_board[(File::a, Rank::r5)] = Piece::pawn_white;
		{
			const auto _validity = is_move_valid(_board, _move, Color::white);
			ASSERT(_validity == MoveValidity::other_piece_in_the_way, "rook move with pawn in the way returned wrong validity");
		};
		_board[(File::a, Rank::r5)] = Piece::empty; // remove pawn

		// Add an enemy pawn at destination pos
		_board[_rookDest] = Piece::pawn_black;
		{
			const auto _validity = is_move_valid(_board, _move, Color::white);
			ASSERT(_validity == MoveValidity::valid, "rook move to capture pawn considered invalid");
		};
	};

	PASS();

};
int subtest_pawn_move()
{
	NEWTEST();

	BoardWithState _board = make_standard_board();
	Move _move{ (File::a, Rank::r2), (File::a, Rank::r4) };
	const auto _validity = is_move_valid(_board, _move, Color::white);
	ASSERT(_validity == MoveValidity::valid, "initial pawn move was considered invalid");

	PASS();
};

int subtest_threatened()
{
	NEWTEST();

	const auto _fen = "4k3/1R6/5Bp1/2QB3p/6P1/8/7P/5K2 b - - 4 35";
	const auto _board = create_board_from_fen(_fen);

	const auto _threatened = is_piece_threatened(_board, _board.find(Piece::king_black).value());
	ASSERT(!_threatened, "king is not actually threatened here");

	PASS();
};

int subtest_catalog()
{
	NEWTEST();

	const auto _strings = std::array
	{
		std::vector
		{
			"c2c4", "g8f6", "d1a4", "e7e6", "a4b5", "b7b6",
			"h2h4", "f8c5", "b5c5", "b6c5", "b1c3", "d7d6",
			"e2e4", "h7h5", "e1e2", "e8g8", "a2a4", "f6g4",
			"c3d1", "g7g6", "g1f3", "c8a6", "e2e1", "b8c6"
		}
	};

	for (auto& s : _strings)
	{
		BoardWithState _board = make_standard_board();

		Move _move{};
		for (auto& ms : s)
		{
			from_chars(ms, _move);
			const auto _valid = is_move_valid(_board, _move, _board.turn) == MoveValidity::valid;
			apply_move(_board, _move);

			if (!_valid)
			{
				auto _err = std::format("lichess verified move \"{}\" was considered invalid. Fen = {}\n", _move.to_string(), get_board_fen(_board));
				ASSERT(false, _err);
			};
		};
	};

	PASS();
};

int main()
{
	NEWTEST();
	SUBTEST(subtest_pawn_move);
	SUBTEST(subtest_rook);
	SUBTEST(subtest_catalog);
	SUBTEST(subtest_threatened);
	PASS();
};
