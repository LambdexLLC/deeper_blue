#include <lambdex/chess/apply_move.hpp>

namespace lbx::chess
{
	/**
	 * @brief Simple struct for holding some castling-related information
	*/
	struct CastleBehavior
	{
		// Checks if the move is a castle and can be performed
		constexpr bool check(const BoardWithState& _board, const Move& _move) const
		{
			auto& b = (_board.*this->flag);
			return this->king_move == _move && b;
		};

		// Apply the castle to a board, doesn't check for validity
		constexpr void apply(BoardWithState& _board) const
		{
			auto& b = (_board.*this->flag);

			JCLIB_ASSERT(b);

			_board[this->rook_move.to] = _board[this->rook_move.from];
			_board[this->rook_move.from] = Piece::empty;
			_board[this->king_move.to] = _board[this->king_move.from];
			_board[this->king_move.from] = Piece::empty;

			b = false;
		};

		using BoardFlag = bool BoardWithState::*;

		Move king_move;
		Move rook_move;
		BoardFlag flag;
	};

	/**
	 * @brief Applies a move to a chess board without checking for validity
	 * @param _board Board to apply move on
	 * @param _move Move to apply
	 * @param _player Player making the move
	*/
	void apply_move(BoardWithState& _board, const Move& _move, const Color& _player)
	{
		// Castle movement definitions
		constexpr auto _castleMoves = std::array
		{
			CastleBehavior
			{
				Move{ (Rank::r8, File::e), (Rank::r8, File::g) },
				Move{ (Rank::r8, File::h), (Rank::r8, File::f) },
				&BoardWithState::black_can_castle_kingside
			},
			CastleBehavior
			{
				Move{ (Rank::r8, File::e), (Rank::r8, File::c) },
				Move{ (Rank::r8, File::a), (Rank::r8, File::d) },
				&BoardWithState::black_can_castle_queenside
			},
			CastleBehavior
			{
				Move{ (Rank::r1, File::e), (Rank::r1, File::g) },
				Move{ (Rank::r1, File::h), (Rank::r1, File::f) },
				&BoardWithState::white_can_castle_kingside
			},
			CastleBehavior
			{
				Move{ (Rank::r1, File::e), (Rank::r1, File::c) },
				Move{ (Rank::r1, File::a), (Rank::r1, File::d) },
				&BoardWithState::white_can_castle_queenside
			}
		};

		_board.turn = !_board.turn;

		// Handle castling moves
		for (auto& m : _castleMoves)
		{
			if (m.check(_board, _move))
			{
				m.apply(_board);
				return;
			};
		};

		auto _piece = _board[_move.from];
		_board[_move.to] = _piece;
		_board[_move.from] = Piece::empty;

		if (_move.promotion != Piece::empty)
		{
			auto _promoPiece = _board[_move.to];
			if (_player == Color::black)
			{
				_promoPiece = _promoPiece | Color::black;
			};
			_board[_move.to] = _promoPiece;
		};

	};
};