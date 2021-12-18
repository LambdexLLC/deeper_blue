#include <lambdex/chess/move_validation.hpp>
#include <lambdex/chess/piece_movement.hpp>

#include <lambdex/chess/board/piece_board.hpp>

#include <jclib/functional.h>
#include <jclib/algorithm.h>

#include <iostream>

namespace lbx::chess
{
	namespace move_check
	{
		/*
			Functions below are for validating movement for particular pieces
		*/
		
		constexpr inline BitBoard bits_below_rank(Rank _rank)
		{
			// TODO: Precompute this at compile time
			BitBoard _out{};
			for (Rank r = Rank::r1; r != _rank; ++r)
			{
				_out |= bits_in_rank(r);
			};
			return _out;
		};
		constexpr inline BitBoard bits_above_rank(Rank _rank)
		{
			// TODO: Precompute this at compile time
			BitBoard _out{};
			for (Rank r = Rank::r8; r != _rank; --r)
			{
				_out |= bits_in_rank(r);
				if (_rank == Rank::r1)
				{
					break;
				};
			};
			return _out;
		};
			
		constexpr inline BitBoard bits_right_of_file(File _file)
		{
			// TODO: Precompute this at compile time
			BitBoard _out{};
			for (File f = File::h; f != _file; --f)
			{
				_out |= bits_in_file(f);
				if (_file == File::a)
				{
					break;
				};
			};
			return _out;
		};
		constexpr inline BitBoard bits_left_of_file(File _file)
		{
			// TODO: Precompute this at compile time
			BitBoard _out{};
			for (File f = File::a; f != _file; ++f)
			{
				_out |= bits_in_file(f);
			};
			return _out;
		};

		constexpr inline BitBoard bits_outside_range(File _file1, File _file2)
		{
			return	bits_left_of_file(std::min(_file1, _file2)) |
					bits_right_of_file(std::max(_file1, _file2));
		};
		constexpr inline BitBoard bits_outside_range(Rank _rank1, Rank _rank2)
		{
			return	bits_below_rank(std::min(_rank1, _rank2)) |
					bits_above_rank(std::max(_rank1, _rank2));
		};

		constexpr inline BitBoard bits_inside_range_inclusive(File _file1, File _file2)
		{
			return ~bits_outside_range(_file1, _file2);
		};
		constexpr inline BitBoard bits_inside_range_inclusive(Rank _rank1, Rank _rank2)
		{
			return ~bits_outside_range(_rank1, _rank2);
		};

		constexpr inline BitBoard bits_inside_range(File _file1, File _file2)
		{
			return bits_inside_range_inclusive(_file1, _file2) & ~(bits_in_file(_file1) | bits_in_file(_file2));
		};
		constexpr inline BitBoard bits_inside_range(Rank _rank1, Rank _rank2)
		{
			return bits_inside_range_inclusive(_rank1, _rank2) & ~(bits_in_rank(_rank1) | bits_in_rank(_rank2));
		};

		constexpr inline BitBoard bits_outside_range_inclusive(File _file1, File _file2)
		{
			return	bits_outside_range(_file1, _file2) |
					bits_in_file(_file1) |
					bits_in_file(_file2);
		};
		constexpr inline BitBoard bits_outside_range_inclusive(Rank _rank1, Rank _rank2)
		{
			return	bits_outside_range(_rank1, _rank2) |
					bits_in_rank(_rank1) |
					bits_in_rank(_rank2);
		};

		constexpr inline BitBoard make_bitboard_with_path(File _file, Rank _rank1, Rank _rank2)
		{
			// Mask allows only bits in the file within the range [_rank1, _rank2]
			const auto _bitsWithinRange = bits_inside_range_inclusive(_rank1, _rank2);
			return bits_in_file(_file) & _bitsWithinRange;
		};
		constexpr inline BitBoard make_bitboard_with_path(Rank _rank, File _file1, File _file2)
		{
			// Mask allows only bits in the file within the range [_file1, _file2]
			const auto _bitsWithinRange = bits_inside_range_inclusive(_file1, _file2);
			return bits_in_rank(_rank) & _bitsWithinRange;
		};
			


		// validation for a rook move, color independent
		inline MoveValidity validate_move_rook_common(const PieceBoard& _board, const Move& _move, const Color& _player)
		{
			auto& _to = _move.to;
			auto& _from = _move.from;

			// Make ideal path bitboard
			BitBoard _idealPath{};
			const auto _movementType = classify_movement(_move.from, _move.to);
			switch (_movementType)
			{
			case MovementClass::file:
				_idealPath = make_bitboard_with_path(_move.from.rank(), _move.from.file(), _move.to.file());
				break;
			case MovementClass::rank:
				_idealPath = make_bitboard_with_path(_move.from.file(), _move.from.rank(), _move.to.rank());
				break;
			default:
				return MoveValidity::illegal_piece_movement;
				break;
			};

			const auto _rookMoveBits = get_rook_movement_bits(_move.from);
			
			// Remove the from and to pos pieces from the set
			auto _pieces = _board.as_bits_with_pieces();
			_pieces.reset(_move.from);
			_pieces.reset(_move.to);
			
			// Branch shouldn't result in instructions needing to be loaded
			const auto _piecesInPath = _pieces & _idealPath;
			if (_piecesInPath.none() && _rookMoveBits[_move.to])
			{
				return MoveValidity::valid;
			}
			else
			{
				return MoveValidity::other_piece_in_the_way;
			};
		};

		inline MoveValidity validate_move_rook_white(const PieceBoard& _board, const Move& _move, const Color& _player)
		{
			return validate_move_rook_common(_board, _move, _player);
		};
		inline MoveValidity validate_move_rook_black(const PieceBoard& _board, const Move& _move, const Color& _player)
		{
			return validate_move_rook_common(_board, _move, _player);
		};

		// validation for a bishop move, color independent
		inline MoveValidity validate_move_bishop_common(const PieceBoard& _board, const Move& _move, const Color& _player)
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

		inline MoveValidity validate_move_bishop_white(const PieceBoard& _board, const Move& _move, const Color& _player)
		{
			return validate_move_bishop_common(_board, _move, _player);
		};
		inline MoveValidity validate_move_bishop_black(const PieceBoard& _board, const Move& _move, const Color& _player)
		{
			return validate_move_bishop_common(_board, _move, _player);
		};

		inline MoveValidity validate_move_queen_common(const PieceBoard& _board, const Move& _move, const Color& _player)
		{
			auto& _to = _move.to;
			auto& _from = _move.from;

			const auto _movementType = classify_movement(_from, _to);

			switch (_movementType)
			{
			case MovementClass::diagonal:
				// Diagonal movement, reuse bishop validator
				if (_player == Color::white)
				{
					return validate_move_bishop_white(_board, _move, _player);
				}
				else
				{
					return validate_move_bishop_black(_board, _move, _player);
				};
				break;

			case MovementClass::file: [[fallthrough]];
			case MovementClass::rank:
			
				// Horizontal or vertical movement, reuse rook validator
				return validate_move_rook_common(_board, _move, _player);
			
			default:
				// Illegal move
				return MoveValidity::illegal_piece_movement;
			};

			return MoveValidity::valid;
		};
		inline MoveValidity validate_move_knight_white(const PieceBoard& _board, const Move& _move, const Color& _color)
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
		inline MoveValidity validate_move_knight_black(const PieceBoard& _board, const Move& _move, const Color& _color)
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

		inline MoveValidity validate_move_king_common(const BoardWithState& _board, const Move& _move, const Color& _color)
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
				// Check for castling
				switch (_color)
				{
				case Color::white:
				{
					if (_to == (File::g, Rank::r1) && _board.white_can_castle_kingside)
					{
						return MoveValidity::valid;
					}
					else if (_to == (File::c, Rank::r1) && _board.white_can_castle_queenside)
					{
						return MoveValidity::valid;
					};
				};
				break;
				case Color::black:
				{
					if (_to == (File::g, Rank::r8) && _board.black_can_castle_kingside)
					{
						return MoveValidity::valid;
					}
					else if (_to == (File::c, Rank::r8) && _board.black_can_castle_queenside)
					{
						return MoveValidity::valid;
					};
				};
				break;
				};


				// King can only move 1 square at a time
				return MoveValidity::illegal_piece_movement;
			};

			return MoveValidity::valid;
		};
		inline MoveValidity validate_move_king_white(const BoardWithState& _board, const Move& _move, const Color& _color)
		{
			return validate_move_king_common(_board, _move, _color);
		};
		inline MoveValidity validate_move_king_black(const BoardWithState& _board, const Move& _move, const Color& _color)
		{
			return validate_move_king_common(_board, _move, _color);
		};

		inline MoveValidity validate_move_pawn_white(const BoardWithState& _board, const Move& _move, const Color& _color)
		{
			const auto& _from = _move.from;
			const auto& _to = _move.to;

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

					if (_board[(_from.rank() + 1, _to.file())] != Piece::empty)
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
				if (_board[_to] == Piece::empty || get_color(_board[_to]) == _color)
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

			return MoveValidity::valid;
		};
		inline MoveValidity validate_move_pawn_black(const BoardWithState& _board, const Move& _move, const Color& _color)
		{
			const auto& _from = _move.from;
			const auto& _to = _move.to;
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

					if (_board[(_from.rank() - 1, _to.file())] != Piece::empty)
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
				if (_board[_to] == Piece::empty || get_color(_board[_to]) == _color)
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
			
			return MoveValidity::valid;
		};
	};

	/**
	 * @brief Determines if a piece is being immediately threatened by an enemy piece
	 * @param _board Board to check on
	 * @param _position Position of the piece to check for threat
	 * @return Position of the enemy piece that is directly threatening this piece, or nullopt if there isnt one
	*/
	std::optional<PositionPair> is_piece_threatened(const PieceBoard& _board, PositionPair _position)
	{
		const auto _piece = _board[_position];
		JCLIB_ASSERT(_piece != Piece::empty);
		const auto _pieceColor = get_color(_piece);


		struct FoundPiece
		{
			PositionPair pos;
			
			/**
			 * @brief The piece type, always white
			*/
			Piece piece;
		};

		// Buffer for holding where the opponent's pieces are 
		std::array<FoundPiece, 16> _opponentPiecesBuffer{};
		size_t _opponentPieceCount = 0;
		
		// Find opponent pieces before doing threat checks
		{
			auto it = _opponentPiecesBuffer.begin();
			for (Position _squarePos{}; _squarePos != Position::end(); ++_squarePos)
			{
				auto& _square = _board[_squarePos];
				if (_square != Piece::empty && _pieceColor != get_color(_square))
				{
					it->piece = _square;
					it->pos = _squarePos;
					++it;
					++_opponentPieceCount;
				};
			};
		};

		const auto _opponentPieces = std::span<FoundPiece>
		{
			_opponentPiecesBuffer.data(), _opponentPieceCount
		};

		for (auto& _opponentPiece : _opponentPieces)
		{
			auto& _squarePiece = _opponentPiece.piece;
			const auto _squarePosPair = PositionPair{ _opponentPiece.pos };

			switch (_squarePiece)
			{
			case Piece::bishop:
			{
				const auto _movementType = classify_movement(_squarePosPair, _position);
				if (_movementType == MovementClass::diagonal)
				{
					// Determine if a piece is in the path
					auto _collidePos = find_piece_in_path(_board, _squarePosPair, _position, _movementType);
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
				const auto _movementType = classify_movement(_squarePosPair, _position);
				if (_movementType != MovementClass::invalid)
				{
					// Determine if a piece is in the path
					auto _collidePos = find_piece_in_path(_board, _squarePosPair, _position, _movementType);
					if (!_collidePos)
					{
						// Found threat!
						return _squarePosPair;
					};
				};
			};
			break;
			case Piece::rook:
			{
				const auto _movementType = classify_movement(_squarePosPair, _position);
				if (_movementType == MovementClass::rank || _movementType == MovementClass::file)
				{
					// Determine if a piece is in the path
					auto _collidePos = find_piece_in_path(_board, _squarePosPair, _position, _movementType);
					if (!_collidePos)
					{
						// Found threat!
						return _squarePosPair;
					};
				};
			};
			break;
			case Piece::knight:
			{
				// The position offsets for where the knight can move
				constexpr auto _offendingOffsets = std::array
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
					
				// Determine if the pieces are offset such that the knight can hit the piece
				const auto _offset = _squarePosPair - _position;
				if (jc::contains(_offendingOffsets, _offset))
				{
					return _opponentPiece.pos;
				};
			};
			break;
			case Piece::pawn:
			{
				const auto _pawnColor = !_pieceColor;
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
						// Look for diagonal + 2 capture, make sure to check for blocking pieces
						if ((_squarePosPair.file() != File::h) &&
							(_position == (_squarePosPair.rank() + 2, _squarePosPair.file() + 1)) &&
							(_board[(_squarePosPair.rank() + 1, _squarePosPair.file())] == Piece::empty))
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
					return _opponentPiece.pos;
				};
			};
			break;
			default:
				break;
			}
		};

		return std::nullopt;
	};

	/**
	 * @brief Checks if a move is a valid move
	 * @param _board Board the move is being played on
	 * @param _move Move to check
	 * @param _player Player making the move
	 * @return True if move is valid, false otherwise
	*/
	MoveValidity is_move_valid(const BoardWithState& _board, const Move& _move, const Color& _player)
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
			if (const auto _result = move_check::validate_move_pawn_black(_board, _move, _player);
				_result != MoveValidity::valid)
			{
				return _result;
			};
			break;
		case Piece::pawn_white:
			if (const auto _result = move_check::validate_move_pawn_white(_board, _move, _player);
				_result != MoveValidity::valid)
			{
				return _result;
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
		auto _checkTestBoard{ _board };
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
