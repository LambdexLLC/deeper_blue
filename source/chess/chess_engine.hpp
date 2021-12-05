#pragma once

/*
	Defines the interface for a chess engine or processor - think of it as
	the interface for individual chess AI implementations.

	Engine implementations should be places in the source directory at path
	
			"source/chess/engines/*"

*/

#include "move.hpp"
#include "board.hpp"

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
		 * @brief Checks if this engine is stateless. Engines that are not stateless will only
		 * ever work on a single game per instance.
		 * @return True if stateless, false otherwise
		*/
		virtual bool is_stateless() const = 0;

		/**
		 * @brief Implement this method as the main engine "thinking" routine. Engine implementations
		 * are expected to take in a board and return the best possible move (in their humble opinion).
		 * 
		 * It can be assumed that this method will only ever be called when it is the engine's turn, so
		 * do not worry about checking for turn order.
		 * 
		 * @param _board Chess board state.
		 * @param _player The color for this engine.
		 * 
		 * @return The move the engine would like to make, this should be pre-checked for validity.
		 * An engine returning illegal moves is considered a broken engine.
		*/
		virtual Move calculate_move(const BoardWithState& _board, Color _player) = 0;

		/**
		 * @brief Same as calculate_move() but this may return multiple moves. Returned moves
		 * do not have to be valid; however, at least one must be valid otherwise the engine is
		 * considered broken.
		 * 
		 * This method is mostly a placeholder while the move validation functions are still
		 * begin implemented.
		 * 
		 * @param _board Chess board state.
		 * @param _player The color for this engine.
		 * 
		 * @return One or more moves that this engine will try to play.
		*/
		virtual std::vector<Move> calculate_multiple_moves(const BoardWithState& _board, Color _player) { return {}; };

		IChessEngine() = default;
		virtual ~IChessEngine() = default;
	};
};