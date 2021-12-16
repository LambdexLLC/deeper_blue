#include <lambdex/chess/piece_movement.hpp>
#include <lambdex/chess/move_validation.hpp>

namespace lbx::chess
{
	/**
	 * @brief Finds all possible moves for a given chess board.
	 *
	 * This will find the moves that the player who's turn it is currently can play.
	 * ie. "BoardWithState::turn"
	 *
	 * @param _board Chess board with state.
	 * @param _moveBuffer Output variable for where to write the found moves to.
	 *
	 * @return Number of moves found.
	*/
	size_t find_possible_moves(const BoardWithState& _board, std::span<Move> _moveBuffer)
	{
		JCLIB_ASSERT(!_moveBuffer.empty());
		
		// Loop through all non-empty squares (so pieces) and check for valid moves
		Position _from{};

		// Output iterator
		auto _moveIter = _moveBuffer.begin();

		// Adds a move to the output move vector if it is valid
		// 
		// @param _to Where the move is to
		// 
		// @return True unless there is no more room in the move output buffer.
		const auto add_if_valid = [&](PositionPair _to)
		{
			Move _move{ _from, _to };
			const auto _validity = chess::is_move_valid(_board, _move, _board.turn);
			if (_validity == MoveValidity::valid)
			{
				// Add to set of moves to attempt
				*_moveIter = _move;
				++_moveIter;
			};

			// Check if there is room for more moves
			return _moveIter != _moveBuffer.end();
		};

		// Loop through all of our pieces
		for (auto& s : _board)
		{
			if (s != Piece::empty && chess::get_color(s) == _board.turn)
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

						PositionPair_Offset{  1,  0 },
						PositionPair_Offset{ -1,  0 },

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
					};
					if (_pair.file() != File::h)
					{
						add_if_valid((_pair.rank() - 1, _pair.file() + 1));
					};

					add_if_valid((_pair.rank() - 1, _pair.file()));
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
					};
					if (_pair.file() != File::h)
					{
						add_if_valid((_pair.rank() + 1, _pair.file() + 1));
					};

					add_if_valid((_pair.rank() + 1, _pair.file()));
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

		// Return moves written
		return std::distance(_moveBuffer.begin(), _moveIter);
	};

	/**
	 * @brief Finds all possible moves for a given chess board.
	 *
	 * This will find the moves that the player who's turn it is currently can play.
	 * ie. "BoardWithState::turn"
	 *
	 * @param _board Chess board with state.
	 *
	 * @return Vector containing moves found.
	*/
	lbx::arena<Move> find_possible_moves(const BoardWithState& _board)
	{
		// Create a buffer for storing the moves
		constexpr size_t buffer_size = 128;
		std::array<Move, buffer_size> _moves{};
		
		const auto _count = find_possible_moves(_board, _moves);
		JCLIB_ASSERT(_count < buffer_size);
		
		return arena<Move>{ _moves.begin(), _moves.begin() + _count };
	};

};