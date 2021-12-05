#include "baby_engine.hpp"

#include "chess/chess.hpp"

#include "utility/io.hpp"

#include <jclib/timer.h>
#include <jclib/ranges.h>
#include <jclib/functional.h>

#include <array>
#include <vector>
#include <ranges>
#include <fstream>
#include <algorithm>

namespace lbx::chess
{
	/**
	 * @brief Gets the value of a piece
	 * @return Abstract value
	*/
	int ChessEngine_Baby::get_piece_value(Piece _piece) const
	{
		// Convert to white for ease of checking
		_piece = as_white(_piece);
		switch (_piece)
		{
		case Piece::empty:
			return 0;
		case Piece::pawn:
			return 5;
		case Piece::knight:
			return 15;
		case Piece::bishop:
			return 25;
		case Piece::rook:
			return 50;
		case Piece::queen:
			return 200;
		case Piece::king:
			return 100000;
		default:
			JCLIB_ABORT();
			return 0;
		};
	};

	/**
	 * @brief Gets the total piece value of a player's existing pieces
	 * @param _board Board to get pieces from
	 * @param _player Player to get value of
	 * @return Total value
	*/
	int ChessEngine_Baby::get_player_material(const Board& _board, Color _player) const
	{
		int _value = 0;
		for (auto& s : _board)
		{
			if (get_color(s) == _player)
			{
				_value += this->get_piece_value(s);
			};
		};
		return _value;
	};


	std::vector<ChessEngine_Baby::RankedMove> ChessEngine_Baby::find_best_moves(const Board& _board, Color _player)
	{
		std::vector<RankedMove> _rankedMoves{};

		// Generate possible boards from 1 move
		auto _randomMoves = this->random_fallback_.calculate_multiple_moves(_board, _player);
		for (auto& m : _randomMoves)
		{
			Board _b{ _board };
			chess::apply_move(_b, m, _player);
			_b.turn = !_b.turn;

			_rankedMoves.push_back
			({
				_b,
				this->get_player_material(_b, _player) - this->get_player_material(_b, !_player),
				m
				});
		};

		// Sort by value
		std::ranges::sort(_rankedMoves, [](const auto& lhs, const auto& rhs) -> bool
			{
				return lhs.value > rhs.value;
			});

		return _rankedMoves;
	};



	std::vector<Move> ChessEngine_Baby::calculate_multiple_moves(const Board& _board, Color _player)
	{
		auto _moves = this->find_best_moves(_board, _player);

		for (auto& m : _moves)
		{
			std::vector<int> _possibleOutcomes{};
			auto _responseMoves = this->find_best_moves(m.board, m.board.turn);
			
			for (auto& _rm : _responseMoves)
			{
				auto _nextMoves = this->find_best_moves(_rm.board, _player);
				if (_nextMoves.empty())
				{
					break;
				}
				else
				{
					_possibleOutcomes.push_back(_nextMoves.front().value);
				};
			};

			if (!_possibleOutcomes.empty())
			{
				auto _maxOutcome = *std::ranges::max_element(_possibleOutcomes);
				m.value = _maxOutcome;
			};
		};

		// Resort by value
		std::ranges::sort(_moves, [](const auto& lhs, const auto& rhs) -> bool
			{
				return lhs.value > rhs.value;
			});

		std::ofstream _possibleDump{ SOURCE_ROOT "/possible_moves.txt" };
		writeln(_possibleDump, "current\n{}\n", _board);
		writeln(_possibleDump, "possible:\n");
		
		for (auto& m : _moves)
		{
			writeln(_possibleDump, "\n{}\n", m.board);
		};

		// Grab the individual moves
		std::vector<Move> _final(_moves.size());
		auto it = _final.begin();
		for (auto& m : _moves)
		{
			*it = m.move;
			++it;
		};
		
		return _final;
	};

};
