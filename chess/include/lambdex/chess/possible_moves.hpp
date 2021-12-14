#pragma once

#include "board.hpp"

namespace lbx::chess
{
	/**
	 * @brief Struct for holding the possible moves a knight may make
	*/
	using MovesPositions_Knight = PositionArray<8>;

	/**
	 * @brief Gets the possible moves a knight may make. Does not check for putting same-color king in check.
	 * @param _board Chess board state
	 * @param _knightPos Position of the knight to find moves for, must be a knight!
	 * @return Possible moves struct
	*/
	constexpr inline MovesPositions_Knight get_possible_move_positions_knight(const PieceBoard& _board, PositionPair _knightPos)
	{
		JCLIB_ASSERT((jc::to_underlying(_board[_knightPos]) & ~0b1) == jc::to_underlying(Piece::knight));
		const auto _knightColor = get_color(_board[_knightPos]);
		const auto _unbounded = PositionPair_Unbounded{ _knightPos };

		// The position offsets for where the knight can move
		constexpr std::array<PositionPair_Offset, 8> _offsets
		{
			PositionPair_Offset{ +2, +1 },
			PositionPair_Offset{ +1, +2 },
			PositionPair_Offset{ -2, -1 },
			PositionPair_Offset{ -1, -2 },
			PositionPair_Offset{ -2, +1 },
			PositionPair_Offset{ -1, +2 },
			PositionPair_Offset{ +2, -1 },
			PositionPair_Offset{ +1, -2 }
		};

		// Look through all possible move positions
		MovesPositions_Knight _out{};
		for (auto& _offset : _offsets)
		{
			// Check if the destination pos is in bounds
			const auto _destinationPos = _unbounded + _offset;
			if (_destinationPos.is_within_bounds())
			{
				if (_board[_destinationPos] == Piece::empty || get_color(_board[_destinationPos]) != _knightColor)
				{
					_out.push_back(_destinationPos);
				};
			};
		};

		return _out;
	};
	
	using MovePositions_Pawn = PositionArray<4>;



};