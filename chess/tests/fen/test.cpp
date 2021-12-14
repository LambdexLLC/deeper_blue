#include <lambdex/chess/fen.hpp>

#include <jclib-test.hpp>

#include <string>

using namespace lbx::chess;


int subtest_standard_board_fen()
{
	NEWTEST();

	// Convert fen to board
	const std::string _fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	const auto _board = create_board_from_fen(_fen);
	ASSERT
	(
		(
			_board == make_standard_board() &&
			_board.turn == Color::white &&
			_board.black_can_castle_kingside &&
			_board.black_can_castle_queenside &&
			_board.white_can_castle_kingside &&
			_board.white_can_castle_queenside &&
			_board.half_move_counter == 0 &&
			_board.full_move_counter == 1 &&
			!_board.has_en_passant()
		),
		
		"failed to parse standard board fen string"
	);

	// Convert back to fen
	const auto _boardFen = get_board_fen(_board);
	std::cout << _boardFen << '\n';
	ASSERT(_boardFen == _fen, "conversion from standard layout board to fen string failed");

  PASS();
};

int main()
{
	NEWTEST();
	SUBTEST(subtest_standard_board_fen);
	PASS();
};
