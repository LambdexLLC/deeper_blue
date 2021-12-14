#include "random_engine.hpp"

#include <lambdex/chess/chess.hpp>

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
		// The set of moves this will randomly select from
		std::vector<Move> _moves{};

		// Loop through all non-empty squares (so pieces) and check for valid moves
		Position _from{};

		// Adds a move to the output move vector if it is valid
		// @param _to Where the move is to
		const auto add_if_valid = [&](PositionPair _to)
		{
			Move _move{ _from, _to };
			const auto _validity = chess::is_move_valid(_board, _move, _player);
			if (_validity == MoveValidity::valid)
			{
				// Add to set of moves to attempt
				_moves.push_back(_move);
				return true;
			}
			else
			{
				return false;
			};
		};

		for (auto& s : _board)
		{
			if (s != Piece::empty && chess::get_color(s) == _player)
			{
				switch (s)
				{
				case Piece::rook_white: [[fallthrough]];
				case Piece::rook_black:
				{
					PositionPair _pair{ _from };
					for (File f = File::a; f != File::END; ++f)
					{
						add_if_valid((_pair.rank(), f));
					};
					for (Rank r = Rank::r1; r != Rank::END; ++r)
					{
						add_if_valid((r, _pair.file()));
					};
				};
				break;

				case Piece::knight_white: [[fallthrough]];
				case Piece::knight_black:
				{
					const auto _positions = get_possible_move_positions_knight(_board, _from);
					for (auto& p : _positions)
					{
						add_if_valid(p);
					};
				};
				break;

				case Piece::king_white: [[fallthrough]];
				case Piece::king_black:
				{
					constexpr auto _offsets = std::array
					{
						PositionPair_Offset{  1,  1 },
						PositionPair_Offset{  0,  1 },
						PositionPair_Offset{ -1,  1 },
						PositionPair_Offset{  0,  1 },
						PositionPair_Offset{  0, -1 },
						PositionPair_Offset{  1, -1 },
						PositionPair_Offset{  0, -1 },
						PositionPair_Offset{ -1, -1 },
					};
					for (auto& o : _offsets)
					{
						const auto _unboundedDest = PositionPair_Unbounded{ _from } + o;
						if (_unboundedDest.is_within_bounds())
						{
							add_if_valid(_unboundedDest);
						};
					};
				};
				break;

				case Piece::bishop_white: [[fallthrough]];
				case Piece::bishop_black:
				{
					// TODO: Improve this
					
					// Find all diagonal movements
					for (Position _to = Position{}; _to != Position::end(); ++_to)
					{
						if (classify_movement(_from, _to) == MovementClass::diagonal)
						{
							add_if_valid(_to);
						};
					};
				};
				break;

				case Piece::queen_white: [[fallthrough]];
				case Piece::queen_black:
				{
					// TODO: Improve this

					// Find all file, rank, or diagonal movements
					for (Position _to = Position{}; _to != Position::end(); ++_to)
					{
						if (classify_movement(_from, _to) != MovementClass::invalid)
						{
							add_if_valid(_to);
						};
					};
				};
				break;

				case Piece::pawn_black:
				{
					PositionPair _pair{ _from };

					// Look ahead 2
					bool _lookAhead2 = _pair.rank() == Rank::r7;

					if (_pair.file() != File::a)
					{
						add_if_valid((_pair.rank() - 1, _pair.file() - 1));
						if (_lookAhead2)
						{
							add_if_valid((_pair.rank() - 2, _pair.file() - 1));
						};
					};
					if (_pair.file() != File::h)
					{
						add_if_valid((_pair.rank() - 1, _pair.file() + 1));
						if (_lookAhead2)
						{
							add_if_valid((_pair.rank() - 2, _pair.file() + 1));
						};
					};

					add_if_valid((_pair.rank() - 2, _pair.file()));
					if (_lookAhead2)
					{
						add_if_valid((_pair.rank() - 2, _pair.file()));
					};
				};
				break;
				case Piece::pawn_white:
				{
					PositionPair _pair{ _from };

					// Look ahead 2
					bool _lookAhead2 = _pair.rank() == Rank::r7;

					if (_pair.file() != File::a)
					{
						add_if_valid((_pair.rank() + 1, _pair.file() - 1));
						if (_lookAhead2)
						{
							add_if_valid((_pair.rank() + 2, _pair.file() - 1));
						};
					};
					if (_pair.file() != File::h)
					{
						add_if_valid((_pair.rank() + 1, _pair.file() + 1));
						if (_lookAhead2)
						{
							add_if_valid((_pair.rank() + 2, _pair.file() + 1));
						};
					};

					add_if_valid((_pair.rank() + 2, _pair.file()));
					if (_lookAhead2)
					{
						add_if_valid((_pair.rank() + 2, _pair.file()));
					};
				};
				break;

				default:
					JCLIB_ABORT();
					break;
				};
			};
			++_from;
		};

		// Randomize found moves
		static std::random_device rd{};
		static std::mt19937 g(rd());
		std::ranges::shuffle(_moves, g);

		// Return randomized moves
		return _moves;
	};

	void ChessEngine_Random::play_turn(IGameInterface& _game)
	{
		auto _moves = this->calculate_multiple_moves(_game.get_board(), _game.get_color());
		for (auto& m : _moves)
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