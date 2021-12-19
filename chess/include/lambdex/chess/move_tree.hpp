#pragma once
#ifndef LAMBDEX_CHESS_MOVE_TREE_HPP
#define LAMBDEX_CHESS_MOVE_TREE_HPP

#include "evaluation.hpp"

#include <jclib/memory.h>
#include <jclib/ranges.h>

#include <vector>

namespace lbx::chess
{
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

			// Standard container aliases

			using value_type = Node;
			using pointer = value_type*;
			using reference = value_type&;
			using const_pointer = const value_type*;
			using const_reference = const value_type&;

			using size_type = size_t;


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


			/*
				Implement comparisons based on the ratings for the nodes' moves
			*/

#if JCLIB_FEATURE_THREE_WAY_COMPARISON_V
			auto operator<=>(const Node& rhs) const noexcept
			{
				return this->get_rating() <=> rhs.get_rating();
			};
#endif
			bool operator==(const Node& rhs) const noexcept
			{
				return this->get_rating() == rhs.get_rating();
			};
			bool operator!=(const Node& rhs) const noexcept
			{
				return this->get_rating() != rhs.get_rating();
			};
			bool operator<(const Node& rhs) const noexcept
			{
				return this->get_rating() < rhs.get_rating();
			};
			bool operator>(const Node& rhs) const noexcept
			{
				return this->get_rating() > rhs.get_rating();
			};
			bool operator<=(const Node& rhs) const noexcept
			{
				return this->get_rating() <= rhs.get_rating();
			};
			bool operator>=(const Node& rhs) const noexcept
			{
				return this->get_rating() >= rhs.get_rating();
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
			 * @brief Gets the size of THIS node.
			 * @return Number of responses for this node.
			*/
			size_type size() const
			{
				size_type _count = 0;
				for (auto p = this->responses_.get(); p->good(); ++p)
				{
					++_count;
				};
				return _count;
			};

			/**
			 * @brief Gets a pointer to the beginning of the responses.
			 * @return Pointer to the first response.
			*/
			pointer data()
			{
				return this->responses_.get();
			};

			/**
			 * @brief Gets a pointer to the beginning of the responses.
			 * @return Pointer to the first response.
			*/
			const_pointer data() const
			{
				return this->responses_.get();
			};

			/**
			 * @brief Gets a view into the responses to the move at this node
			 * @return Responses view (span)
			*/
			std::span<value_type> responses() noexcept
			{
				return std::span<value_type>{ this->data(), this->size() };
			};

			/**
			 * @brief Gets a view into the responses to the move at this node
			 * @return Responses view (span)
			*/
			std::span<const value_type> responses() const noexcept
			{
				return std::span<const value_type>{ this->data(), this->size() };
			};

			/**
			 * @brief Gets the size of this node and child nodes.
			 * @return Number of nodes that are children of this.
			*/
			size_type child_count() const
			{
				if (this->has_responses())
				{
					auto _count = this->size();
					for (auto& r : this->responses())
					{
						_count += r.child_count();
					};
					return _count;
				}
				else
				{
					return 0;
				};
			};

			/**
			 * @brief Sets the responses to this move
			 * @tparam RangeT Type of the range containing responses
			 * @param _range Range containing responses
			*/
			template <typename RangeT> JCLIB_REQUIRES((
				jc::cx_range<RangeT> &&
				jc::cx_convertible_to
				<
					jc::ranges::const_reference_t<RangeT>, // from
					Node								   // to
				>))
			void set_responses(const RangeT& _range)
			{
				const auto _size = jc::ranges::distance(_range) + 1;
				if (_size == 1)
				{
					this->responses_.reset();
					return;
				};

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


		// Iterator support

		auto begin() { return jc::begin(this->moves_); };
		auto begin() const { return jc::begin(this->moves_); };
		auto cbegin() const { return jc::begin(this->moves_); };

		auto end() { return jc::end(this->moves_); };
		auto end() const { return jc::end(this->moves_); };
		auto cend() const { return jc::end(this->moves_); };


		/**
		 * @brief Gets the total size of the tree.
		 * @return Size of the tree in nodes.
		*/
		size_t child_count() const
		{
			auto _size = this->moves_.size();
			for (auto& n : *this)
			{
				_size += n.child_count();
			};
			return _size;
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