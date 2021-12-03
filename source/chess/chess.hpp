#pragma once

#include "move.hpp"
#include "board.hpp"

namespace lbx::chess
{
	/**
	 * @brief Possible values that can be returned by the is_move_valid function
	*/
	enum class MoveValidity : int
	{
		/**
		 * @brief Move is valid
		*/
		valid = 0,

		/**
		 * @brief Move isn't moving a piece
		*/
		not_a_piece,

		/**
		 * @brief It is not the player's turn to make a move
		*/
		not_players_turn,

		/**
		 * @brief Piece to move was not owned by the player
		*/
		not_players_piece,

		/**
		 * @brief Move would does not result in piece movement
		*/
		no_piece_movement,

		/**
		 * @brief One of our pieces is in the way of the move
		*/
		my_piece_in_the_way,

		/**
		 * @brief Move would cause an illegal piece movement (like pawns moving backwards)
		*/
		illegal_piece_movement,
	};

	/**
	 * @brief Checks if a move is a valid move
	 * @param _board Board the move is being played on
	 * @param _move Move to check
	 * @param _player Player making the move
	 * @return True if move is valid, false otherwise
	*/
	constexpr inline MoveValidity is_move_valid(const Board& _board, const Move& _move, const Color& _player)
	{
		const auto _movingPiece = _board[_move.from];
		
		const auto& _from = _move.from;
		const auto& _to = _move.to;

		// Check if it is the player's turn
		if (_board.turn != _player)
		{
			return MoveValidity::not_players_turn;
		};

		// Check if the move is moving into itself
		if (_move.from == _move.to)
		{
			return MoveValidity::no_piece_movement;
		};

		// Check if the piece being moved is a piece and can be moved by the player
		if (_movingPiece == Piece::empty)
		{
			return MoveValidity::not_a_piece;
		}
		else if (get_color(_movingPiece) != _player)
		{
			return MoveValidity::not_players_piece;
		};

		// Check if the piece would be moving onto a friendly piece
		if (get_color(_board[_move.to]) == _player)
		{
			return MoveValidity::my_piece_in_the_way;
		};

		// Handle based on piece
		switch (_movingPiece)
		{
		case Piece::pawn_black:
			if (_from.file() == _to.file())
			{
				// Check if this isn't the first move
				if (_from.rank() != Rank::r7)
				{
					// Check for valid forward move
					if ((_from.rank() - _to.rank()) != 1)
					{
						return MoveValidity::illegal_piece_movement;
					};
				}
				else
				{
					// Check for valid forward move, allow double move on first move
					if ((_from.rank() - _to.rank()) > 2)
					{
						return MoveValidity::illegal_piece_movement;
					};
				};
			}
			else
			{
				// Cannot move pieces sidewise
				return MoveValidity::illegal_piece_movement;
			};
			break;
		case Piece::pawn_white:
			if (_from.file() == _to.file())
			{
				// Check if this isn't the first move
				if (_from.rank() != Rank::r2)
				{
					// Check for valid forward move
					if ((_to.rank() - _from.rank()) != 1)
					{
						return MoveValidity::illegal_piece_movement;
					};
				}
				else
				{
					// Check for valid forward move, allow double move on first move
					if ((_to.rank() - _from.rank()) > 2)
					{
						return MoveValidity::illegal_piece_movement;
					};
				};
			}
			else
			{
				// Cannot move pieces sidewise
				return MoveValidity::illegal_piece_movement;
			};
			break;
		default:
			break;
		};
		

		return MoveValidity::valid;
	};

	/**
	 * @brief Applies a move to a chess board without checking for validity
	 * @param _board Board to apply move on
	 * @param _move Move to apply
	*/
	constexpr inline void apply_move(Board& _board, const Move& _move)
	{
		auto _piece = _board[_move.from];
		_board[_move.to] = _piece;
		_board[_move.from] = Piece::empty;
	};

};
