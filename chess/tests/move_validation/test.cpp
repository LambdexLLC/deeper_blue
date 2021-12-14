#include <lambdex/chess/chess.hpp>
#include <lambdex/chess/piece_movement.hpp>
#include <lambdex/chess/move_validation.hpp>

#include <jclib-test.hpp>

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

int main()
{
	NEWTEST();
	SUBTEST(subtest_pawn_move);
	SUBTEST(subtest_rook);
	PASS();
};
