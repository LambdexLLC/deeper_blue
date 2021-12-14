#pragma once
#ifndef LAMBDEX_CHESS_MOVE_VALIDATION_HPP
#define LAMBDEX_CHESS_MOVE_VALIDATION_HPP

/*
	Provides functionality for checking if a chess move is valid or not
*/

#include "apply_move.hpp"

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
	constexpr inline std::optional<PositionPair> find_piece_in_path_rank(const PieceBoard& _board, const PositionPair& _from, const PositionPair& _to)
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
		return std::nullopt;
	};

	// Path along file, exclusive min and max, ie. checks squares between points
	constexpr inline std::optional<PositionPair> find_piece_in_path_file(const PieceBoard& _board, const PositionPair& _from, const PositionPair& _to)
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
		return std::nullopt;
	};

	// Path along diagonal
	constexpr inline std::optional<PositionPair> find_piece_in_path_diagonal(const PieceBoard& _board, PositionPair _from, PositionPair _to)
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
		return std::nullopt;
	};

	enum class MovementClass
	{
		invalid,
		file,
		rank,
		diagonal
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

	// Diagonal path, exclusive min and max, ie. checks squares between points
	constexpr inline std::optional<PositionPair> find_piece_in_path(const PieceBoard& _board, const PositionPair& _from, const PositionPair& _to)
	{
		const auto _movement = classify_movement(_from, _to);
		switch (_movement)
		{
		case MovementClass::diagonal:
			return find_piece_in_path_diagonal(_board, _from, _to);
		case MovementClass::file:
			return find_piece_in_path_file(_board, _from, _to);
		case MovementClass::rank:
			return find_piece_in_path_rank(_board, _from, _to);
		case MovementClass::invalid:
			return std::nullopt;
		default:
			JCLIB_ABORT();
			return std::nullopt;
		};
	};

	/**
	 * @brief Struct for holding the possible moves a knight may make
	*/
	struct PossibleKnightMoves
	{
		/**
		 * @brief Storage for all of the possible moves, get "count" to determine
		 * how many of these positions are actually valid
		*/
		std::array<PositionPair, 8> pos;

		/**
		 * @brief How many "good" positions are held by pos
		*/
		size_t count;
	};

	/**
	 * @brief Gets the possible moves a knight may make. Does not check for putting same-color king in check.
	 * @param _board Chess board state
	 * @param _knightPos Position of the knight to find moves for, must be a knight!
	 * @return Possible moves struct
	*/
	constexpr inline PossibleKnightMoves get_knight_possible_moves(const PieceBoard& _board, PositionPair _knightPos)
	{
		JCLIB_ASSERT((jc::to_underlying(_board[_knightPos]) & ~0b1) == jc::to_underlying(Piece::knight));
		const auto _knightColor = get_color(_board[_knightPos]);
		const auto _rank = (int)_knightPos.rank();
		const auto _file = (int)_knightPos.file();

		std::array<std::pair<int, int>, 8> _unboundedMoves{};
		_unboundedMoves[0] = { _rank + 1, _file + 2 };
		_unboundedMoves[1] = { _rank + 2, _file + 1 };
		_unboundedMoves[2] = { _rank - 1, _file - 2 };
		_unboundedMoves[3] = { _rank - 2, _file - 1 };
		_unboundedMoves[4] = { _rank + 1, _file - 2 };
		_unboundedMoves[5] = { _rank + 2, _file - 1 };
		_unboundedMoves[6] = { _rank - 1, _file + 2 };
		_unboundedMoves[7] = { _rank - 2, _file + 1 };

		PossibleKnightMoves _out{};
		for (auto& _unbounded : _unboundedMoves)
		{
			if (_unbounded.first >= 0 && _unbounded.first <= 7 &&
				_unbounded.second >= 0 && _unbounded.second <= 7)
			{
				auto _pos = (Rank(_unbounded.first), File(_unbounded.second));
				if (_board[_pos] == Piece::empty || get_color(_board[_pos]) != _knightColor)
				{
					_out.pos[_out.count++] = _pos;
				};
			};
		};

		return _out;
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