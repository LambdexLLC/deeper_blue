#include <lambdex/chess/evaluation.hpp>

#include <jclib/ranges.h>

namespace lbx::chess
{
	/**
	 * @brief Checks if the given board is checkmate
	 * @param _board Board to check
	 * @param _player Player to test for checkmate of
	 * @return True if checkmate
	*/
	bool is_checkmate(const BoardWithState& _board, Color _player)
	{
		const auto _kingPosOpt = _board.find(Piece::king | _player);
		if (!_kingPosOpt)
		{
			return true;
		};

		const auto _kingPos = *_kingPosOpt;
		if (is_piece_threatened(_board, _kingPos))
		{
			// Check if there are any possible moves
			static thread_local std::array<Move, 128> _moveBuffer{};
			const auto _count = find_possible_moves(_board, _moveBuffer);
			if (_count == 0)
			{
				return true;
			}
			else
			{
				return false;
			};
		}
		else
		{
			return false;
		};

	};



	constexpr inline Rating bishop_complexity_v = 25;
	constexpr inline Rating king_complexity_v	= 10;
	constexpr inline Rating knight_complexity_v = 10;
	constexpr inline Rating pawn_complexity_v	= 5;
	constexpr inline Rating rook_complexity_v	= 50;
	constexpr inline Rating queen_complexity_v	= 150;

	constexpr inline Rating rate_complexity(Piece _piece)
	{
		switch (as_white(_piece))
		{
		case Piece::bishop:
			return bishop_complexity_v;
		case Piece::rook:
			return rook_complexity_v;
		case Piece::knight:
			return knight_complexity_v;
		case Piece::pawn:
			return pawn_complexity_v;
		case Piece::queen:
			return queen_complexity_v;
		case Piece::king:
			return king_complexity_v;
		default:
			JCLIB_ASSERT(false);
			return 0;
		};
	};

	/**
	 * @brief Rates the overall complexity of the board based on which pieces are present.
	 * @param _board Chess board.
	 * @return Complexity value as a rating.
	*/
	Rating rate_complexity(const BoardWithState& _board)
	{
		Rating _sum{};
		for (auto& s : _board | jc::views::filter(jc::unequals & Piece::empty))
		{
			_sum += rate_complexity(s);
		};
		return _sum;
	};

};