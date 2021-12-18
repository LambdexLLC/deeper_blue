#include <lambdex/chess/evaluation.hpp>

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
};