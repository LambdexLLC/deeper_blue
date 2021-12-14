#include "greedy_engine.hpp"

namespace lbx::chess
{
	/**
	 * @brief Plays the first move that nets it the most material
	 * @param _game Game interface
	*/
	void ChessEngine_Greedy::play_turn(IGameInterface& _game)
	{
		const auto _board = _game.get_board();
		const auto _color = _game.get_color();
		
		// Find all possible moves
		const auto _moves = this->random_engine_.calculate_multiple_moves(_board, _color);
		
		// Rate the possible moves
		std::vector<RatedMove> _ratedMoves(_moves.size());
		auto it = _ratedMoves.begin();
		for (auto& m : _moves)
		{
			*it = rate_move(_board, m);
			++it;
		};

		// Sort moves
		std::ranges::sort(_ratedMoves, jc::greater);

		// Try and play the moves
		for (auto& m : _ratedMoves)
		{
			if (_game.submit_move(m.get_move()))
			{
				return;
			};
		};

		// Resign
		_game.resign();
	};
};