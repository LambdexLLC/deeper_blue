#pragma once
#ifndef LAMBDEX_CHESS_FEN_HPP
#define LAMBDEX_CHESS_FEN_HPP

#include "board/board_with_state.hpp"

#include <string_view>

namespace lbx::chess
{
	/**
	 * @brief Creates a board with state from a fen string
	 * @param _fen Fen string, this is not validated!
	 * @return Board with state as specified by the fen string
	*/
	BoardWithState create_board_from_fen(const std::string_view _fen);
};

#endif // LAMBDEX_CHESS_FEN_HPP