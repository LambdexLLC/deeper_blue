#pragma once

#include "move.hpp"
#include "board/board.hpp"

#include <jclib/functional.h>

#include <ranges>
#include <numeric>
#include <algorithm>

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

	/**
	 * @brief Applies a move to a chess board without checking for validity
	 * @param _board Board to apply move on
	 * @param _move Move to apply
	*/
	constexpr inline void apply_move(PieceBoard& _board, const Move& _move, const Color& _player)
	{
		auto _piece = _board[_move.from];
		_board[_move.to] = _piece;
		_board[_move.from] = Piece::empty;
		if (_move.promotion != Piece::empty)
		{
			auto _promoPiece = _board[_move.to];
			if (_player == Color::black)
			{
				_promoPiece = _promoPiece | Color::black;
			};
			_board[_move.to] = _promoPiece;
		};
	};
	constexpr inline void apply_move(BoardWithState& _board, const Move& _move, const Color& _player)
	{
		apply_move(static_cast<PieceBoard&>(_board), _move, _player);
		_board.turn = !_board.turn;
	};


	// Path along file, exclusive min and max, ie. checks squares between points
	constexpr inline std::optional<PositionPair> find_piece_in_path(const PieceBoard& _board, File _file, Rank _minRank, Rank _maxRank)
	{
		if (_maxRank - _minRank >= 2)
		{
			_minRank = _minRank + 1;
			_maxRank = _maxRank - 1;
		}
		else
		{
			return std::nullopt;
		};

		for (_minRank; _minRank <= _maxRank; _minRank = _minRank + 1)
		{
			const auto _at = (_minRank, _file);
			if (_board[_at] != Piece::empty)
			{
				return _at;
			};
		};
		return std::nullopt;
	};

	// Path along rank, exclusive min and max, ie. checks squares between points
	constexpr inline std::optional<PositionPair> find_piece_in_path(const PieceBoard& _board, Rank _rank, File _minFile, File _maxFile)
	{
		if (_maxFile - _minFile >= 2)
		{
			_minFile = _minFile + 1;
			_maxFile = _maxFile - 1;
		}
		else
		{
			return std::nullopt;
		};

		for (_minFile; _minFile != _maxFile; _minFile = _minFile + 1)
		{
			const auto _at = (_rank, _minFile);
			if (_board[_at] != Piece::empty)
			{
				return _at;
			};
		};
		return std::nullopt;
	};

	// Diagonal path, exclusive min and max, ie. checks squares between points
	constexpr inline std::optional<PositionPair> find_piece_in_path(const PieceBoard& _board, Rank _minRank, Rank _maxRank, File _minFile, File _maxFile)
	{
		JCLIB_ASSERT((_maxRank - _minRank) == (_maxFile - _minFile));

		for (uint8_t _offset = 0; _offset != (_maxFile - _minFile); ++_offset)
		{
			const auto _at = (_minRank + _offset, _minFile + _offset);
			if (_board[_at] != Piece::empty &&
				_at != (_minRank, _minFile))
			{
				return _at;
			};
		};
		return std::nullopt;
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
	constexpr inline std::optional<PositionPair> is_piece_threatened(const PieceBoard& _board, PositionPair _position)
	{
		const auto _piece = _board[_position];
		JCLIB_ASSERT(_piece != Piece::empty);
		const auto _pieceColor = get_color(_piece);

		Position _squarePos{ 0 };

		constexpr auto q = PositionPair{ Position{ 31 } };

		for (_squarePos; _squarePos != Position{ 64 }; ++_squarePos)
		{
			auto& _square = _board[_squarePos];
			const auto _squarePosPair = PositionPair{ _squarePos };
			
			if (_square != Piece::empty && _pieceColor != get_color(_square))
			{
				// Found enemy piece
				const auto _squarePiece = as_white(_board[_squarePosPair]);
				switch (_squarePiece)
				{
				case Piece::bishop:
				{
					const auto _fileDist = distance(_squarePosPair.file(), _position.file());
					const auto _rankDist = distance(_squarePosPair.rank(), _position.rank());

					if (_fileDist == _rankDist)
					{
						// Check for collision on path
						const auto [_minFile, _maxFile] = std::minmax(_position.file(), _squarePosPair.file());
						const auto [_minRank, _maxRank] = std::minmax(_position.rank(), _squarePosPair.rank());

						auto _collidePos = find_piece_in_path(_board, _minRank, _maxRank, _minFile, _maxFile);
						if (!_collidePos)
						{
							// Found threat!
							return _squarePosPair;
						};
					};
				};
				break;
				case Piece::queen:
				{
					const auto _fileDist = distance(_squarePosPair.file(), _position.file());
					const auto _rankDist = distance(_squarePosPair.rank(), _position.rank());

					if (_fileDist == _rankDist)
					{
						// Check for collision on path
						const auto [_minFile, _maxFile] = std::minmax(_position.file(), _squarePosPair.file());
						const auto [_minRank, _maxRank] = std::minmax(_position.rank(), _squarePosPair.rank());

						auto _collidePos = find_piece_in_path(_board, _minRank, _maxRank, _minFile, _maxFile);
						if (!_collidePos)
						{
							// Found threat!
							return _squarePosPair;
						};
					}
					else if (_squarePosPair.file() == _position.file())
					{
						// Check for collision on path
						const auto [_min, _max] = std::minmax(_position.rank(), _squarePosPair.rank());
						auto _collidePos = find_piece_in_path(_board, _position.file(), _min, _max);
						if (!_collidePos)
						{
							// Found threat!
							return _squarePosPair;
						};
					}
					else if (_squarePosPair.rank() == _position.rank())
					{
						// Check for collision on path
						const auto [_min, _max] = std::minmax(_position.file(), _squarePosPair.file());
						auto _collidePos = find_piece_in_path(_board, _position.rank(), _min, _max);
						if (!_collidePos)
						{
							// Found threat!
							return _squarePosPair;
						};
					};
				};
					break;
				case Piece::rook:
					if (_squarePosPair.file() == _position.file())
					{
						// Check for collision on path
						const auto [_min, _max] = std::minmax(_position.rank(), _squarePosPair.rank());
						auto _collidePos = find_piece_in_path(_board, _position.file(), _min, _max);
						if (!_collidePos)
						{
							// Found threat!
							return _squarePosPair;
						};
					}
					else if (_squarePosPair.rank() == _position.rank())
					{
						// Check for collision on path
						const auto [_min, _max] = std::minmax(_position.file(), _squarePosPair.file());
						auto _collidePos = find_piece_in_path(_board, _position.rank(), _min, _max);
						if (!_collidePos)
						{
							// Found threat!
							return _squarePosPair;
						};
					};
					break;
				case Piece::knight:
				{
					const auto _possibleMoves = get_knight_possible_moves(_board, _squarePosPair);
					for (auto m : std::views::counted(_possibleMoves.pos.begin(), _possibleMoves.count))
					{
						if (m == _position)
						{
							return _squarePos;
						};
					};
				};
				break;
				case Piece::pawn:
				{
					const auto _pawnColor = get_color(_board[_squarePosPair]);
					if (_pawnColor == Color::white)
					{
						// Look for diagonal + 1 capture
						if ((_squarePosPair.file() != File::h) &&
							(_position == (_squarePosPair.rank() + 1, _squarePosPair.file() + 1)))
						{
							// Pawn would take
							return _squarePosPair;
						};
						if ((_squarePosPair.file() != File::a) &&
							(_position == (_squarePosPair.rank() + 1, _squarePosPair.file() - 1)))
						{
							// Pawn would take
							return _squarePosPair;
						};

						if (_squarePosPair.rank() == Rank::r2)
						{
							// Look for diagonal + 2 capture
							if ((_squarePosPair.file() != File::h) &&
								(_position == (_squarePosPair.rank() + 2, _squarePosPair.file() + 1)))
							{
								// Pawn would take
								return _squarePosPair;
							};
							if ((_squarePosPair.file() != File::a) &&
								(_position == (_squarePosPair.rank() + 2, _squarePosPair.file() - 1)))
							{
								// Pawn would take
								return _squarePosPair;
							};
						};
					}
					else if (_pawnColor == Color::black)
					{
						// Look for diagonal - 1 capture
						if ((_squarePosPair.file() != File::h) && (_squarePosPair.rank() != Rank::r1) &&
							(_position == (_squarePosPair.rank() - 1, _squarePosPair.file() + 1)))
						{
							// Pawn would take
							return _squarePosPair;
						};
						if ((_squarePosPair.file() != File::a) && (_squarePosPair.rank() != Rank::r1) &&
							(_position == (_squarePosPair.rank() - 1, _squarePosPair.file() - 1)))
						{
							// Pawn would take
							return _squarePosPair;
						};

						if (_squarePosPair.rank() == Rank::r7)
						{
							// Look for diagonal - 2 capture
							if ((_squarePosPair.file() != File::h) &&
								(_position == (_squarePosPair.rank() - 2, _squarePosPair.file() + 1)))
							{
								// Pawn would take
								return _squarePosPair;
							};
							if ((_squarePosPair.file() != File::a) &&
								(_position == (_squarePosPair.rank() - 2, _squarePosPair.file() - 1)))
							{
								// Pawn would take
								return _squarePosPair;
							};
						};
					};
				};
				break;
				case Piece::king:
				{
					if (distance(_squarePosPair.file(), _position.file()) <= 1 &&
						distance(_squarePosPair.rank(), _position.rank()) <= 1)
					{
						// King is threatening
						return _squarePos;
					};
				};
				break;
				default:
					break;
				}
			};
		};

		return std::nullopt;
	};


	namespace move_check
	{
		// validation for a rook move, color independent
		constexpr inline MoveValidity validate_move_rook_common(const PieceBoard& _board, const Move& _move, const Color& _player)
		{
			auto& _to = _move.to;
			auto& _from = _move.from;

			if (_from == (Rank::r8, File::h))
			{
				if (_to == (Rank::r8, File::g))
				{
					auto q = jc::equals & true;
				};
			};

			const auto _forwardDistance = distance(_from.rank(), _to.rank());
			const auto _horizontalDistance = distance(_from.file(), _to.file());
			
			if (_forwardDistance != 0 && _horizontalDistance != 0)
			{
				// Cannot move diagonally
				return MoveValidity::illegal_piece_movement;
			}
			else if (_forwardDistance != 0)
			{
				// Check if there are any pieces in the way
				const auto [_min, _max] = std::minmax(_from.rank(), _to.rank());
				for (Rank _at = _min; _at != _max; _at = _at + 1)
				{
					// Ignore this rank and destination rank
					if (_at != _to.rank() && _at != _from.rank())
					{
						// Check if a piece is in the way
						const auto _lookingAt = (_at, _from.file());
						if (_board[_lookingAt] != Piece::empty)
						{
							return MoveValidity::other_piece_in_the_way;
						};
					};
				};
			}
			else
			{
				// Horizontal move

				// Check if there are any pieces in the way
				const auto [_min, _max] = std::minmax(_from.file(), _to.file());
				for (File _at = _min; _at != _max; _at = _at + 1)
				{
					// Ignore this rank and destination rank
					if (_at != _to.file() && _at != _from.file())
					{
						// Check if a piece is in the way
						const auto _lookingAt = (_from.rank(), _at);
						if (_board[_lookingAt] != Piece::empty)
						{
							return MoveValidity::other_piece_in_the_way;
						};
					};
				};
			};

			return MoveValidity::valid;
		};
		
		constexpr inline MoveValidity validate_move_rook_white(const PieceBoard& _board, const Move& _move, const Color& _player)
		{
			return validate_move_rook_common(_board, _move, _player);
		};
		constexpr inline MoveValidity validate_move_rook_black(const PieceBoard& _board, const Move& _move, const Color& _player)
		{
			return validate_move_rook_common(_board, _move, _player);
		};
		
		// validation for a bishop move, color independent
		constexpr inline MoveValidity validate_move_bishop_common(const PieceBoard& _board, const Move& _move, const Color& _player)
		{
			auto& _to = _move.to;
			auto& _from = _move.from;

			const auto _forwardDistance = distance(_from.rank(), _to.rank());
			const auto _horizontalDistance = distance(_from.file(), _to.file());

			if (_forwardDistance != _horizontalDistance)
			{
				// Cannot move horizontally / vertically
				return MoveValidity::illegal_piece_movement;
			}
			else
			{
				// Check if there are any pieces in the way
				
				// How much to increment each axis each loop
				const int8_t _incRank = (_from.rank() < _to.rank()) ? 1 : -1;
				const int8_t _incFile = (_from.file() < _to.file()) ? 1 : -1;

				// Position we are looking at
				auto _at = (_from.rank(), _from.file());

				// Check for collisions
				for (uint8_t _offset = 0; _offset != _forwardDistance; ++_offset)
				{
					// Ignore begin and end positions
					if (_at != _from && _at != _to)
					{
						if (_board[_at] != Piece::empty)
						{
							return MoveValidity::other_piece_in_the_way;
						};
					};

					// Increment by axis
					_at = (_at.rank() + _incRank, _at.file() + _incFile);
				};
			};
		
			return MoveValidity::valid;
		};

		constexpr inline MoveValidity validate_move_bishop_white(const PieceBoard& _board, const Move& _move, const Color& _player)
		{
			return validate_move_bishop_common(_board, _move, _player);
		};
		constexpr inline MoveValidity validate_move_bishop_black(const PieceBoard& _board, const Move& _move, const Color& _player)
		{
			return validate_move_bishop_common(_board, _move, _player);
		};

		// validation for a queen move, color independent
		constexpr inline MoveValidity validate_move_queen_common(const PieceBoard& _board, const Move& _move, const Color& _player)
		{
			auto& _to = _move.to;
			auto& _from = _move.from;

			const auto _forwardDistance = distance(_from.rank(), _to.rank());
			const auto _horizontalDistance = distance(_from.file(), _to.file());

			if (_forwardDistance == _horizontalDistance)
			{
				// Diagonal movement, reuse bishop validator
				if (_player == Color::white)
				{
					return validate_move_bishop_white(_board, _move, _player);
				}
				else
				{
					return validate_move_bishop_black(_board, _move, _player);
				};
			}
			else if (_forwardDistance != 0 && _horizontalDistance != 0)
			{
				// Illegal move
				return MoveValidity::illegal_piece_movement;
			}
			else
			{
				// Horizontal or vertical movement, reuse rook validator
				if (_player == Color::white)
				{
					return validate_move_rook_white(_board, _move, _player);
				}
				else
				{
					return validate_move_rook_black(_board, _move, _player);
				};
			};

			return MoveValidity::valid;
		};

		constexpr inline MoveValidity validate_move_knight_white(const PieceBoard& _board, const Move& _move, const Color& _color)
		{
			auto& _to = _move.to;
			auto& _from = _move.from;

			const auto _forwardDistance = distance(_from.rank(), _to.rank());
			const auto _horizontalDistance = distance(_from.file(), _to.file());

			if (_forwardDistance == 1 && _horizontalDistance == 2)
			{
				// okay
				return MoveValidity::valid;
			}
			else if (_forwardDistance == 2 && _horizontalDistance == 1)
			{
				// okay
				return MoveValidity::valid;
			}
			else
			{
				// Illegal
				return MoveValidity::illegal_piece_movement;
			};
		};
		constexpr inline MoveValidity validate_move_knight_black(const PieceBoard& _board, const Move& _move, const Color& _color)
		{
			auto& _to = _move.to;
			auto& _from = _move.from;

			const auto _forwardDistance = distance(_from.rank(), _to.rank());
			const auto _horizontalDistance = distance(_from.file(), _to.file());

			if (_forwardDistance == 1 && _horizontalDistance == 2)
			{
				// okay
				return MoveValidity::valid;
			}
			else if (_forwardDistance == 2 && _horizontalDistance == 1)
			{
				// okay
				return MoveValidity::valid;
			}
			else
			{
				// Illegal
				return MoveValidity::illegal_piece_movement;
			};
		};
	
		constexpr inline MoveValidity validate_move_king_common(const PieceBoard& _board, const Move& _move, const Color& _color)
		{
			const auto& _from = _move.from;
			const auto& _to = _move.to;
			
			if (_to == (Rank::r4, File::h))
			{
				JCLIB_ASSERT(true);
			};

			const auto _horizontalDistance = distance(_from.file(), _to.file());
			const auto _verticalDistance = distance(_from.rank(), _to.rank());

			if (_horizontalDistance > 1 || _verticalDistance > 1)
			{
				// King can only move 1 square at a time
				return MoveValidity::illegal_piece_movement;
			};

			return MoveValidity::valid;
		};
		constexpr inline MoveValidity validate_move_king_white(const PieceBoard& _board, const Move& _move, const Color& _color)
		{
			return validate_move_king_common(_board, _move, _color);
		};
		constexpr inline MoveValidity validate_move_king_black(const PieceBoard& _board, const Move& _move, const Color& _color)
		{
			return validate_move_king_common(_board, _move, _color);
		};
	};

	/**
	 * @brief Checks if a move is a valid move
	 * @param _board Board the move is being played on
	 * @param _move Move to check
	 * @param _player Player making the move
	 * @return True if move is valid, false otherwise
	*/
	constexpr inline MoveValidity is_move_valid(const BoardWithState& _board, const Move& _move, const Color& _player)
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
		if (_board[_move.to] != Piece::empty && get_color(_board[_move.to]) == _player)
		{
			return MoveValidity::other_piece_in_the_way;
		};


		// Handle based on piece
		switch (_movingPiece)
		{
		case Piece::rook_black:
			if (const auto _result = move_check::validate_move_rook_black(_board, _move, _player);
				_result != MoveValidity::valid)
			{
				return _result;
			};
			break;
		case Piece::rook_white:
			if (const auto _result = move_check::validate_move_rook_white(_board, _move, _player);
				_result != MoveValidity::valid)
			{
				return _result;
			};
			break;
		case Piece::bishop_black:
			if (const auto _result = move_check::validate_move_bishop_black(_board, _move, _player);
				_result != MoveValidity::valid)
			{
				return _result;
			};
			break;
		case Piece::bishop_white:
			if (const auto _result = move_check::validate_move_bishop_white(_board, _move, _player);
				_result != MoveValidity::valid)
			{
				return _result;
			};
			break;
		case Piece::queen_black:
			if (const auto _result = move_check::validate_move_queen_common(_board, _move, _player);
				_result != MoveValidity::valid)
			{
				return _result;
			};
			break;
		case Piece::queen_white:
			if (const auto _result = move_check::validate_move_queen_common(_board, _move, _player);
				_result != MoveValidity::valid)
			{
				return _result;
			};
			break;
		case Piece::pawn_black:
		{
			const auto _forwardDistance = sdistance(_from.rank(), _to.rank());
			const auto _horizontalDistance = distance(_from.file(), _to.file());

			// Cannot move pawn backwards
			if (_forwardDistance < 0)
			{
				return MoveValidity::illegal_piece_movement;
			};

			if (_horizontalDistance == 0)
			{
				// Check if this isn't the first move
				if (_from.rank() != Rank::r7)
				{
					// Check for valid forward move
					if (_forwardDistance != 1)
					{
						return MoveValidity::illegal_piece_movement;
					}
					else if (_board[_to] != Piece::empty)
					{
						return MoveValidity::other_piece_in_the_way;
					};
				}
				else
				{
					// Check for valid forward move, allow double move on first move
					if (_forwardDistance > 2)
					{
						return MoveValidity::illegal_piece_movement;
					};

					if (_board[(_to.rank() - 1, _to.file())] != Piece::empty)
					{
						// Piece in the way
						return MoveValidity::other_piece_in_the_way;
					}
					else if (_board[_to] != Piece::empty)
					{
						// Piece in the way
						return MoveValidity::other_piece_in_the_way;
					};
				};
			}
			else if (_horizontalDistance == 1)
			{
				// Check if this is a diagonal capture
				if (_board[_to] == Piece::empty)
				{
					return MoveValidity::illegal_piece_movement;
				};
				
				// Can only move forward one
				if (_forwardDistance != 1)
				{
					return MoveValidity::illegal_piece_movement;
				};
			}
			else
			{
				// Cannot move pieces sidewise
				return MoveValidity::illegal_piece_movement;
			};
		};
			break;
		case Piece::pawn_white:
		{
			const auto _forwardDistance = sdistance(_to.rank(), _from.rank());
			const auto _horizontalDistance = distance(_from.file(), _to.file());
			
			// Cannot move pawn backwards
			if (_forwardDistance < 0)
			{
				return MoveValidity::illegal_piece_movement;
			};

			if (_horizontalDistance == 0)
			{
				// Check if this isn't the first move
				if (_from.rank() != Rank::r2)
				{
					// Check for valid forward move
					if (_forwardDistance != 1)
					{
						return MoveValidity::illegal_piece_movement;
					}
					else if (_board[_to] != Piece::empty)
					{
						// Piece in the way
						return MoveValidity::other_piece_in_the_way;
					};
				}
				else
				{
					// Check for valid forward move, allow double move on first move
					if (_forwardDistance > 2)
					{
						return MoveValidity::illegal_piece_movement;
					};

					if (_board[(_to.rank() + 1, _to.file())] != Piece::empty)
					{
						// Piece in the way
						return MoveValidity::other_piece_in_the_way;
					}
					else if (_board[_to] != Piece::empty)
					{
						// Piece in the way
						return MoveValidity::other_piece_in_the_way;
					};
				};
			}
			else if (_horizontalDistance == 1)
			{
				// Check that this is a capture
				if (_board[_to] == Piece::empty || get_color(_board[_to]) == _player)
				{
					// Illegal move
					return MoveValidity::illegal_piece_movement;
				};

				// Can only move forward one
				if (_forwardDistance != 1)
				{
					return MoveValidity::illegal_piece_movement;
				};
			}
			else
			{
				// Cannot move pieces sidewise
				return MoveValidity::illegal_piece_movement;
			};

		};
		break;

		case Piece::king_white:
			if (const auto _result = move_check::validate_move_king_white(_board, _move, _player);
				_result != MoveValidity::valid)
			{
				return _result;
			};
			break;
		case Piece::king_black:
			if (const auto _result = move_check::validate_move_king_black(_board, _move, _player);
				_result != MoveValidity::valid)
			{
				return _result;
			};
			break;
		case Piece::knight_black:
			if (const auto _result = move_check::validate_move_knight_black(_board, _move, _player);
				_result != MoveValidity::valid)
			{
				return _result;
			};
			break;
		case Piece::knight_white:
			if (const auto _result = move_check::validate_move_knight_white(_board, _move, _player);
				_result != MoveValidity::valid)
			{
				return _result;
			};
			break;
		default:
			JCLIB_ABORT();
			break;
		};
		
		// Check if the king would be in check after applying the move
		PieceBoard _checkTestBoard{ _board };
		apply_move(_checkTestBoard, _move, _player);
		
		Piece _kingPiece{};
		if (_player == Color::white)
		{
			_kingPiece = Piece::king_white;
		}
		else
		{
			_kingPiece = Piece::king_black;
		};

		auto _king = _checkTestBoard.find(_kingPiece);
		if (_king)
		{
			auto _threatenedByPiece = is_piece_threatened(_checkTestBoard, _king.value());
			if (_threatenedByPiece)
			{
				// King is in check! Move is not valid!
				return MoveValidity::king_in_check;
			};
		};

		return MoveValidity::valid;
	};


};
