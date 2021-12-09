#pragma once
#ifndef LAMBDEX_CHESS_CHESS_ENGINE_HPP
#define LAMBDEX_CHESS_CHESS_ENGINE_HPP

/*
	Defines the interface for a chess engine or processor - think of it as
	the interface for individual chess AI implementations.

	Engine implementations should be places in the source directory at path

			"source/chess/engines/*"

*/

#include "move.hpp"
#include "board.hpp"
#include "game_interface.hpp"

#include <vector>

namespace lbx::chess
{
	/**
	 * @brief Interface for chess engine implementations
	*/
	class IChessEngine
	{
	public:

		/**
		 * @brief Implement this method as the main engine "thinking" routine. Engine implementations
		 * are expected to take in a board and return the best possible move (in their humble opinion).
		 *
		 * It can be assumed that this method will only ever be called when it is the engine's turn, so
		 * do not worry about checking for turn order.
		 *
		 * @param _game Interface to the current game
		*/
		virtual void play_turn(IGameInterface& _game) = 0;

		// Allow polymorphic destruction
		virtual ~IChessEngine() = default;
	};
};

#endif // LAMBDEX_CHESS_CHESS_ENGINE_HPP