#include <lambdex/chess/chess.hpp>
#include <lambdex/chess/move_validation.hpp>

#include <jclib-test.hpp>

using namespace lbx::chess;


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
	PASS();
};
