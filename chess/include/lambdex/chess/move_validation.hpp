#pragma once
#ifndef LAMBDEX_CHESS_MOVE_VALIDATION_HPP
#define LAMBDEX_CHESS_MOVE_VALIDATION_HPP

/*
	Provides functionality for checking if a chess move is valid or not
*/

#include "apply_move.hpp"
#include "possible_moves.hpp"

#include "board/piece_board.hpp"
#include "board/board_with_state.hpp"

#include "move.hpp"

#include <optional>

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
		 * @brief There is a piece in the way of the move
		*/
		other_piece_in_the_way,

		/**
		 * @brief Move would cause an illegal piece movement (like pawns moving backwards)
		*/
		illegal_piece_movement,

		/**
		 * @brief Move would put the king into or leave the king in check
		*/
		king_in_check,
	};

	// Path along rank, exclusive min and max, ie. checks squares between points
	constexpr inline PositionPair find_piece_in_path_rank(const PieceBoard& _board, const PositionPair& _from, const PositionPair& _to)
	{
		const int8_t _inc = (_from.rank() < _to.rank()) ? 1 : -1;
		for (auto _rank = _from.rank(); _rank != _to.rank(); _rank = _rank + _inc)
		{
			const auto _at = (_rank, _from.file());
			if (_board[_at] != Piece::empty && _at != _from)
			{
				return _at;
			};
		};
		return PositionPair::end();
	};

	// Path along file, exclusive min and max, ie. checks squares between points
	constexpr inline PositionPair find_piece_in_path_file(const PieceBoard& _board, const PositionPair& _from, const PositionPair& _to)
	{
		const int8_t _inc = (_from.file() < _to.file()) ? 1 : -1;
		for (auto _file = _from.file(); _file != _to.file(); _file = _file + _inc)
		{
			const auto _at = (_from.rank(), _file);
			if (_board[_at] != Piece::empty && _at != _from)
			{
				return _at;
			};
		};
		return PositionPair::end();
	};

	// Path along diagonal
	constexpr inline PositionPair find_piece_in_path_diagonal(const PieceBoard& _board, PositionPair _from, PositionPair _to)
	{
		const int8_t _fileInc = (_from.file() < _to.file()) ? 1 : -1;
		const int8_t _rankInc = (_from.rank() < _to.rank()) ? 1 : -1;

		auto _file = _from.file();
		auto _rank = _from.rank();

		for (_file, _rank;
			_file != _to.file() && _rank != _to.rank();
			_file += _fileInc, _rank += _rankInc)
		{
			const auto _at = (_rank, _file);
			if (_board[_at] != Piece::empty && _at != _from)
			{
				return _at;
			};
		};
		return PositionPair::end();
	};

	/**
	 * @brief Types of movement that a chess piece would make
	*/
	enum class MovementClass : uint8_t
	{
		invalid  = 0b00,
		file	 = 0b01,
		rank	 = 0b10,
		diagonal = 0b11
	};

	
	constexpr inline MovementClass classify_movement(const PositionPair& _from, const PositionPair& _to)
	{
		if (_from.rank() == _to.rank() && _from.file() != _to.file())
		{
			// Movement along file
			return MovementClass::file;
		}
		else if (_from.file() == _to.file() && _from.rank() != _to.rank())
		{
			// Movement along rank
			return MovementClass::rank;
		}
		else if (distance(_from.file(), _to.file()) == distance(_from.rank(), _to.rank()))
		{
			// Movement along diagonal
			return MovementClass::diagonal;
		}
		else
		{
			// Invalid
			return MovementClass::invalid;
		};
	};

	static_assert(classify_movement((Rank::r2, File::b), (Rank::r7, File::b)) == MovementClass::rank);
	static_assert(classify_movement((Rank::r2, File::c), (Rank::r2, File::a)) == MovementClass::file);
	static_assert(classify_movement((Rank::r1, File::a), (Rank::r4, File::d)) == MovementClass::diagonal);
	static_assert(classify_movement((Rank::r1, File::a), (Rank::r4, File::b)) == MovementClass::invalid);

	/**
	 * @brief Checks if a piece is within the path of a horizontal, vertical, or diagonal movement
	 * @param _board 
	 * @param _from 
	 * @param _to 
	 * @param _moveClass Classification for this movement
	 * 
	 * @return The position of the piece within the path, or null position pair if nothing is in the path
	*/
	constexpr inline PositionPair find_piece_in_path(const PieceBoard& _board, const PositionPair& _from, const PositionPair& _to, MovementClass _moveClass)
	{
		JCLIB_ASSERT(_moveClass == classify_movement(_from, _to))
		switch (_moveClass)
		{
		case MovementClass::diagonal:
			return find_piece_in_path_diagonal(_board, _from, _to);
		case MovementClass::file:
			return find_piece_in_path_file(_board, _from, _to);
		case MovementClass::rank:
			return find_piece_in_path_rank(_board, _from, _to);
		case MovementClass::invalid:
			return PositionPair::end();
		default:
			JCLIB_ABORT();
			return PositionPair::end();
		};
	};
	
	/**
	 * @brief Checks if a piece is within the path of a horizontal, vertical, or diagonal movement
	 * @param _board
	 * @param _from
	 * @param _to
	 * @return The position of the piece within the path, or null position pair if nothing is in the path
	*/
	constexpr inline PositionPair find_piece_in_path(const PieceBoard& _board, const PositionPair& _from, const PositionPair& _to)
	{
		return find_piece_in_path(_board, _from, _to, classify_movement(_from, _to));
	};


	/**
	 * @brief Determines if a piece is being immediately threatened by an enemy piece
	 * @param _board Board to check on
	 * @param _position Position of the piece to check for threat
	 * @return Position of the enemy piece that is directly threatening this piece, or nullopt if there isnt one
	*/
	std::optional<PositionPair> is_piece_threatened(const PieceBoard& _board, PositionPair _position);

	/**
	 * @brief Checks if a move is a valid move
	 * @param _board Board the move is being played on
	 * @param _move Move to check
	 * @param _player Player making the move
	 * @return True if move is valid, false otherwise
	*/
	MoveValidity is_move_valid(const BoardWithState& _board, const Move& _move, const Color& _player);
};

#endif // LAMBDEX_CHESS_MOVE_VALIDATION_HPP