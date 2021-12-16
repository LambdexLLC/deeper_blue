#pragma once

#include <lambdex/chess/chess_engine.hpp>
#include <lambdex/utility/arena.hpp>

namespace lbx::chess
{
	/**
	 * @brief Picks a random but valid move
	*/
	class ChessEngine_Random : public IChessEngine
	{
	public:

		/**
		 *  Returns all (supposedly) valid moves randomly shuffled
		*/
		lbx::arena<Move> calculate_multiple_moves(const BoardWithState& _board, Color _player);

		/**
		 *	Returns a random but (hopefully) valid move
		*/
		Move calculate_move(const BoardWithState& _board, Color _player)
		{
			return this->calculate_multiple_moves(_board, _player).front();
		};

		void play_turn(IGameInterface& _game) final;

	};

};