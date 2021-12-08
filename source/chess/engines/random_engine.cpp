#include "random_engine.hpp"

#include "chess/chess.hpp"

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
	std::vector<Move> ChessEngine_Random::calculate_multiple_moves(const BoardWithState& _board, Color _player)
	{
		Position _from{};
		Position _to{};

		// The set of moves this will randomly select from
		std::vector<Move> _moves{};

		// Loop through all non-empty squares (so pieces) and check for valid moves
		for (auto& p : _board | std::views::filter(jc::unequals & Piece::empty))
		{
			if (chess::get_color(p) == _player)
			{

			};
		};

		// Probe until we find a "valid" move
		while (true)
		{
			// Check if move is valid
			Move _move{ _from, _to };
			const auto _validity = chess::is_move_valid(_board, _move, _player);
			if (_validity == MoveValidity::valid)
			{
				// Add to set of moves to attempt
				_moves.push_back(_move);
			};

			// Increment position values
			++_from;
			if (_from.get() == 64)
			{
				_from = Position{};
				++_to;
				if (_to.get() == 64)
				{
					// Out of moves!
					break;
				};
			};
		};

		// Randomize found moves
		static std::random_device rd{};
		static std::mt19937 g(rd());
		std::ranges::shuffle(_moves, g);

		// Return randomized moves
		return _moves;
	};
};