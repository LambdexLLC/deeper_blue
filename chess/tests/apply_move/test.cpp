#include <jclib-test.hpp>

#include <lambdex/chess/fen.hpp>
#include <lambdex/chess/apply_move.hpp>

int subtest_en_passant()
{
	NEWTEST();

	using namespace lbx::chess;
	const auto _fen = "rnbqkbnr/4p1p1/p1p5/1pPp1p1p/3PP3/1QN5/PP1BNPPP/1R2KB1R w Kkq d6 0 11";
	const auto _board = create_board_from_fen(_fen);
	ASSERT(_board.has_en_passant(), "test preconditions not met");

	const auto _move = Move((File::c, Rank::r5), (File::d, Rank::r6));

	auto _newBoard = _board;
	apply_move(_newBoard, _move);
	ASSERT(_newBoard[(File::d, Rank::r5)] == Piece::empty, "en passant did not capture the pawn");

	PASS();
};


int main()
{
	NEWTEST();
	SUBTEST(subtest_en_passant);
	PASS();
};