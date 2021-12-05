#pragma once

#include "chess/chess_engine.hpp"

namespace lbx::chess
{
	/**
	 * @brief Picks a random but valid move
	*/
	class ChessEngine_Random : public IChessEngine
	{
	public:

		/**
		 *	Always returns true, we need no state
		*/
		bool is_stateless() const final
		{
			return true;
		};

		/**
		 *	Returns a random but (hopefully) valid move
		*/
		Move calculate_move(const Board& _board, Color _player) final
		{
			return this->calculate_multiple_moves(_board, _player).front();
		};

		/**
		 *  Returns all (supposedly) valid moves randomly shuffled
		*/
		std::vector<Move> calculate_multiple_moves(const Board& _board, Color _player) final;

	};

};