#pragma once
#ifndef LAMBDEX_CHESS_APPLY_MOVE_HPP
#define LAMBDEX_CHESS_APPLY_MOVE_HPP

#include "move.hpp"
#include "board/board_with_state.hpp"

namespace lbx::chess
{
	/**
	 * @brief Applies a move to a chess board without checking for validity
	 * @param _board Board to apply move on
	 * @param _move Move to apply
	 * @param _player Player making the move
	*/
	void apply_move(BoardWithState& _board, const Move& _move, Color _player);

	/**
	 * @brief Applies a move to a chess board without checking for validity
	 * @param _board Board to apply move on
	 * @param _move Move to apply
	*/
	inline auto apply_move(BoardWithState& _board, const Move& _move)
	{
		return apply_move(_board, _move, _board.turn);
	};
}

#endif // LAMBDEX_CHESS_APPLY_MOVE_HPP