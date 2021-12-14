#pragma once
#ifndef LAMBDEX_CHESS_MOVE_TREE_HPP
#define LAMBDEX_CHESS_MOVE_TREE_HPP

#include "basic.hpp"
#include "chess.hpp"

#include <jclib/type.h>
#include <jclib/memory.h>
#include <jclib/ranges.h>
#include <jclib/concepts.h>

#include <vector>
#include <memory>

namespace lbx::chess
{
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
		{ _rater.rate(_board, _player) } -> jc::cx_same_as<int>;
	};

	/**
	 * @brief Simple board rater taking only material into account
	*/
	struct SimpleRater
	{
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
		int rate(const PieceBoard& _board, Color _player) const
		{
			return this->get_player_material(_board, _player) - this->get_player_material(_board, !_player);
		};
	};

	// Check that the concept was fufilled
	static_assert(cx_board_rater<SimpleRater>);

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
		constexpr int get_rating() const noexcept
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
		constexpr explicit RatedMove(Move _move, int _rating) :
			move_{ _move },
			rating_{ _rating }
		{};

	private:
		Move move_;
		int rating_;
	};

	/**
	 * @brief Constructs a rated move
	 * @param _initialBoard Board state PRIOR to the move
	 * @param _move Move that is being rated
	 * @param _rater Board rater, this is given the board AFTER applying the move
	*/
	template <cx_board_rater RaterT = SimpleRater>
	inline RatedMove rate_move(const BoardWithState& _initialBoard, const Move& _move, const RaterT& _rater = SimpleRater{})
	{
		auto _board = _initialBoard;
		apply_move(_board, _move, _board.turn);
		const auto _rating = _rater.rate(_board, _initialBoard.turn);
		return RatedMove{ _move, _rating };
	};

	/**
	 * @brief Tree structure for holding many possible moves over time, ie. simulated games
	*/
	struct MoveTree
	{
	public:

		/**
		 * @brief Type used to represent each move in the tree
		*/
		struct Node
		{
		public:

			/**
			 * @brief Checks if this node is not null
			 * @return True if not null, false otherwise
			*/
			bool good() const
			{
				return this->get_move().from.good();
			};

			/**
			 * @brief Allow conversion to rated move
			 * @return This node as a rated move
			*/
			operator RatedMove() const noexcept
			{
				return this->move_;
			};

			/**
			 * @brief Gets the rating of the move for this node
			 * @return Move rating value
			*/
			auto get_rating() const noexcept
			{
				return this->move_.get_rating();
			};

			/**
			 * @brief Gets the move associated with this node
			 * @return Chess move
			*/
			Move get_move() const noexcept
			{
				return this->move_.get_move();
			};

			/**
			 * Three-way comparison based on ratings of node moves
			*/
			auto operator<=>(const Node& rhs) const noexcept
			{
				return this->get_rating() <=> rhs.get_rating();
			};

			/**
			 * @brief Checks if there are moves assigned to this node as responses
			 * @return True if one or more responses have been set, false otherwise
			*/
			bool has_responses() const
			{
				return this->responses_ != nullptr;
			};

			/**
			 * @brief Gets a view into the responses to the move at this node
			 * @return Responses view (span)
			*/
			std::span<Node> responses() noexcept
			{
				size_t _count = 0;
				for (auto p = this->responses_.get(); p->good(); ++p)
				{
					++_count;
				};
				return std::span{ this->responses_.get(), _count };
			};

			/**
			 * @brief Gets a view into the responses to the move at this node
			 * @return Responses view (span)
			*/
			std::span<const Node> responses() const noexcept
			{
				size_t _count = 0;
				for (auto p = this->responses_.get(); p->good(); ++p)
				{
					++_count;
				};
				return std::span{ this->responses_.get(), _count };
			};

			/**
			 * @brief Sets the responses to this move
			 * @tparam RangeT Type of the range containing responses
			 * @param _range Range containing responses
			*/
			template <jc::cx_range RangeT> requires jc::cx_convertible_to
				<
					jc::ranges::const_reference_t<RangeT>, // from
					Node								   // to
				>
			void set_responses(const RangeT& _range)
			{
				const auto _size = jc::ranges::distance(_range) + 1;
				this->responses_ = std::unique_ptr<Node[]>( new Node[_size] );
				this->responses_.get()[_size - 1] = jc::null;
				std::ranges::copy(_range, this->responses_.get());
			};



			// Null constructor
			Node(jc::null_t) :
				move_{ Move(PositionPair::end(), PositionPair::end()), 0 }
			{};
			
			// Invokes null constructor
			Node() :
				Node(jc::null)
			{};

			Node(const RatedMove& _move) :
				move_{ _move }
			{}
			Node& operator=(const RatedMove& _move)
			{
				this->move_ = _move;
				return *this;
			};

			Node(jc::borrow_ptr<Node> _previous, const RatedMove& _move) :
				Node{ _move }
			{}

		private:

			/**
			 * @brief The moves that could be made in response to this move in the tree
			*/
			std::unique_ptr<Node[]> responses_;

			/**
			 * @brief The move for this node in the tree
			*/
			RatedMove move_;

		};

		/**
		 * @brief The initial board state
		*/
		BoardWithState initial_board_;

		/**
		 * @brief Possible moves that can be made from the initial board state
		*/
		std::vector<Node> moves_;
	};

};

#endif // LAMBDEX_CHESS_MOVE_TREE_HPP