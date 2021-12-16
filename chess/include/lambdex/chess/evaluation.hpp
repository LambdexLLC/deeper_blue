#pragma once
#ifndef LAMBDEX_CHESS_EVALUATION_HPP
#define LAMBDEX_CHESS_EVALUATION_HPP

/*
	Provides functions for evaluating boards, moves, and more
*/

#include "board/piece_board.hpp"
#include "board/board_with_state.hpp"

#include "apply_move.hpp"
#include "move_validation.hpp"

#include <jclib/type.h>
#include <jclib/concepts.h>

namespace lbx::chess
{
	/**
	 * @brief Checks if the given board is checkmate
	 * @param _board Board to check
	 * @param _player Player to test for checkmate of
	 * @return True if checkmate
	*/
	bool is_checkmate(const BoardWithState& _board, Color _player);

	/**
	 * @brief Type used to hold ratings
	*/
	using Rating = int;


	/**
	 * @brief Defines a board rater type
	 *
	 * The concept is fufilled by types that can have an "rate" function
	 * defined that takes a chess board and a player and returns an
	 * integer representation of the "rating" of the board's position from
	 * the POV of the given player.
	 *
	 * See below to see what that means:
	*/
	template <typename T>
	concept cx_board_rater = requires(const T & _rater, const BoardWithState & _board, Color _player)
	{
		{ _rater.rate(_board, _player) } -> jc::cx_same_as<Rating>;
	};

	/**
	 * @brief Simple board rater taking only material into account
	*/
	struct BoardRater_Material
	{
	public:
		
		/**
		 * @brief Gets the value of a piece
		 * @return Abstract value
		*/
		int get_piece_value(Piece _piece)const
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
		int get_player_material(const PieceBoard& _board, Color _player) const
		{
			int _value = 0;
			for (auto& s : _board)
			{
				if (s != Piece::empty && get_color(s) == _player)
				{
					_value += this->get_piece_value(s);
				};
			};
			return _value;
		};

		/**
		 * @brief Rates the board using only material value
		 * @param _board Board to get pieces from
		 * @param _player Player to get value of
		 * @return Total value
		*/
		Rating rate(const PieceBoard& _board, Color _player) const
		{
			return this->get_player_material(_board, _player) - this->get_player_material(_board, !_player);
		};
	};

	// Check that the concept was fufilled
	static_assert(cx_board_rater<BoardRater_Material>);

	/**
	 * @brief Rates a board.
	 * 
	 * @param _board The board to rate.
	 * @param _player The player whose POV we are rating the board from.
	 * @param _rater Board rater.
	*/
	template <cx_board_rater RaterT = BoardRater_Material>
	inline Rating rate(const BoardWithState& _board, const Color _player,  const RaterT& _rater = RaterT{})
	{
		return _rater.rate(_board, _player);
	};

	/**
	 * @brief Represents a move that has been given a ranking
	*/
	class RatedMove
	{
	public:

		/**
		 * @brief Gets the rating for this move
		 * @return Rating for the move
		*/
		constexpr Rating get_rating() const noexcept
		{
			return this->rating_;
		};

		constexpr auto operator<=>(const RatedMove& rhs) const noexcept
		{
			return this->get_rating() <=> rhs.get_rating();
		};

		/**
		 * @brief Gets the move
		 * @return Move value
		*/
		constexpr Move get_move() const noexcept
		{
			return this->move_;
		};

		constexpr RatedMove() = default;

		/**
		 * @brief Constructs the rated move with the board it was rated on
		 * @param _board Board state PRIOR to the move
		 * @param _move Move that is being rated
		 * @param _rating Rating for the board
		*/
		constexpr explicit RatedMove(Move _move, Rating _rating) :
			move_{ _move },
			rating_{ _rating }
		{};

	private:
		Move move_;
		Rating rating_;
	};

	/**
	 * @brief Constructs a rated move
	 * @param _initialBoard Board state PRIOR to the move
	 * @param _move Move that is being rated
	 * @param _rater Board rater, this is given the board AFTER applying the move
	*/
	template <cx_board_rater RaterT = BoardRater_Material>
	inline RatedMove rate_move(const BoardWithState& _initialBoard, const Move& _move, const RaterT& _rater = RaterT{})
	{
		auto _board = _initialBoard;
		apply_move(_board, _move, _board.turn);
		const auto _rating = _rater.rate(_board, _initialBoard.turn);
		return RatedMove{ _move, _rating };
	};

};

#endif // LAMBDEX_CHESS_EVALUATION_HPP