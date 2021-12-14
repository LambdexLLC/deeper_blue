#pragma once
#ifndef LAMBDEX_CHESS_BOARD_HPP
#define LAMBDEX_CHESS_BOARD_HPP

/*
	Includes all of the various types of board
*/

#include "board/generic_board.hpp"
#include "board/bit_board.hpp"
#include "board/piece_board.hpp"
#include "board/board_with_state.hpp"

namespace lbx::chess
{
	/**
	 * @brief Type alias for the most common type of board - board with state
	*/
	using Board = BoardWithState;
};

#endif // LAMBDEX_CHESS_BOARD_HPP