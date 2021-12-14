#pragma once

#include "random_engine.hpp"

namespace lbx::chess
{
	/**
	 * @brief Chess engine that plays the move that nets it the most material, only checks one deep
	*/
	class ChessEngine_Greedy : public IChessEngine
	{
	public:

		/**
		 * @brief Plays the first move that nets it the most material
		 * @param _game Game interface
		*/
		void play_turn(IGameInterface& _game) final;
	
		ChessEngine_Greedy() = default;

	private:
		
		/**
		 * @brief Used to generate the moves that may be played
		*/
		ChessEngine_Random random_engine_{};

	};
};