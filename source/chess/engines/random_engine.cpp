#include "random_engine.hpp"

#include <lambdex/chess/chess.hpp>
#include <lambdex/chess/piece_movement.hpp>

#include <jclib/ranges.h>
#include <jclib/functional.h>

#include <random>
#include <ranges>
#include <algorithm>

namespace lbx::chess
{
	/**
	 *  Returns all (supposedly) valid moves randomly shuffled
	*/
	lbx::arena<Move> ChessEngine_Random::calculate_multiple_moves(const BoardWithState& _board, Color _player)
	{
		// Find all possible moves
		auto _moves = find_possible_moves(_board);

		// Randomize found moves
		static std::random_device rd{};
		static std::mt19937 g(rd());
		std::shuffle(_moves.data(), _moves.data() + _moves.size(), g);

		// Return randomized moves
		return _moves;
	};

	void ChessEngine_Random::play_turn(IGameInterface& _game)
	{
		auto _moves = this->calculate_multiple_moves(_game.get_board(), _game.get_color());
		for (auto& m : std::span{ _moves.data(), _moves.size() })
		{
			const auto _good = _game.submit_move(m);
			if (_good)
			{
				return;
			};
		};
		_game.resign();
	};

};