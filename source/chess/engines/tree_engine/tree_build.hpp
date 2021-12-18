#pragma once

#include "utility/io.hpp"

#include <lambdex/chess/move_tree.hpp>

#include <jclib/guard.h>

#include <mutex>
#include <atomic>
#include <thread>
#include <vector>
#include <barrier>
#include <sstream>
#include <optional>


namespace lbx::chess
{
	using RatedLine = std::vector<RatedMove>;

	/**
	 * @brief Rates a board based on if castling is possible for the players.
	*/
	struct BoardRater_CastleOpportunity
	{
	public:

		/**
		 * @brief The rating value for being able to castle kingside.
		*/
		Rating kingside_value = 1;

		/**
		 * @brief The rating value for being able to castle queenside.
		*/
		Rating queenside_value = 1;

		/**
		 * @brief Rates a board based on if castling is possible for the players.
		 *
		 * @param _board Board to rate.
		 * @param _player Player whose POV to rate from.
		 *
		 * @return Rating based on opportunity to castle.
		*/
		Rating rate(const BoardWithState& _board, Color _player) const
		{
			const auto _myPoints =
				(this->queenside_value * _board.can_player_castle_queenside(_player)) +
				(this->kingside_value * _board.can_player_castle_kingside(_player));
			const auto _opponentPoints =
				(this->queenside_value * _board.can_player_castle_queenside(!_player)) +
				(this->kingside_value * _board.can_player_castle_kingside(!_player));
			return _myPoints - _opponentPoints;
		};
	};
	static_assert(cx_board_rater<BoardRater_CastleOpportunity>);

	/**
	 * @brief Rates a board based on if a player is in checkmate.
	*/
	struct BoardRater_Checkmate
	{
	public:

		/**
		 * @brief The rating value for putting the opponent in checkmate.
		*/
		Rating checkmate_value = 1000000;

		/**
		 * @brief Rates a board based on if any of the players are in checkmate.
		 *
		 * @param _board Board to rate.
		 * @param _player Player whose POV to rate from.
		 *
		 * @return Rating based on checkmates.
		*/
		Rating rate(const BoardWithState& _board, Color _player) const
		{
			if (is_checkmate(_board, _player))
			{
				return -this->checkmate_value;
			}
			else if (is_checkmate(_board, !_player))
			{
				return this->checkmate_value;
			}
			else
			{
				return 0;
			};
		};

	};
	static_assert(cx_board_rater<BoardRater_Checkmate>);

	struct BoardRater_Complete
	{
		int rate(const BoardWithState& _board, Color _player) const
		{
			const auto _materialRating = chess::rate<BoardRater_Material>(_board, _player);
			const auto _checkmateRating = chess::rate(_board, _player, this->checkmate_rater_);
			const auto _castleOpportunityRating = chess::rate<BoardRater_CastleOpportunity>(_board, _player);

			const auto _final = _materialRating + _checkmateRating + _castleOpportunityRating;
			return std::clamp(_final, -this->checkmate_rater_.checkmate_value, this->checkmate_rater_.checkmate_value);
		};


		BoardRater_Checkmate checkmate_rater_{};
	};
	static_assert(cx_board_rater<BoardRater_Complete>);



	using MoveTreeNode = MoveTree::Node;

	/**
	 * @brief Holds a move tree node and its evaluated rating
	*/
	struct RatedNode
	{
		const MoveTreeNode* node = nullptr;
		Rating rating{};
	};



	/**
	 * @brief Find the best response from a move tree node's responses.
	 *
	 * @param _toNode Node to find best response to.
	 * @param _line Optional rated line to add responses to.
	 * @return Pointer to the best response, or nullptr if none were found.
	*/
	RatedNode find_best_response(const MoveTree::Node& _toNode, RatedLine* _line = nullptr);





	/**
	 * @brief Gets the rating of the last move in a line from the POV of a particular player.
	 *
	 * @param _line Line to get move from.
	 * @param _player Player to get rating of the move for, they must have played the first move in the line.
	 * @return Move rating
	*/
	int last_move_rating(const RatedLine& _line, Color _player);



	struct TreeBuilder
	{
		std::vector<RatedMove> rank_possible_moves(const BoardWithState& _board);

		/**
		 * @brief Fills out the response nodes for a given move tree node
		 *
		 * @param _board Board state after applying the move at _forNode.
		 * @param _forNode Node to fill out the responses to.
		 * @param _depth How deep to fill responses out for. Depth of 0 means just this node.
		*/
		void calculate_move_tree_node_responses(const BoardWithState& _board, MoveTree::Node* _forNode, size_t _depth);
		
		/**
		 * @brief Fills out the response nodes for a given move tree node
		 * 
		 * @param _board Board state after applying the move at _forNode.
		 * @param _forNode Node to fill out the responses to.
		*/
		void calculate_move_tree_node_responses(const BoardWithState& _board, MoveTree::Node* _forNode);

		MoveTree make_move_tree(const BoardWithState& _board);
		MoveTree make_move_tree(const BoardWithState& _board, size_t _depth);

		Move calculate_move(const BoardWithState& _board, Color _player)
		{
			return this->calculate_multiple_moves(_board, _player).front();
		};

		std::vector<Move> calculate_multiple_moves(const BoardWithState& _board, Color _player);







		std::vector<RatedLine> pick_best_from_tree(const MoveTree& _tree);
	};





	class TreeBuildTask
	{
	public:

		void invoke()
		{
			TreeBuilder _builder{};
			auto& _board = this->board_;
			_builder.calculate_move_tree_node_responses(_board, this->node_.get(), this->depth_);
		};

		TreeBuildTask(BoardWithState _board, jc::reference_ptr<MoveTree::Node> _node, size_t _depth) :
			board_{ _board },
			node_{ _node },
			depth_{ _depth }
		{};

	private:

		/**
		 * @brief The state of the board after applying the move this is building off of.
		*/
		BoardWithState board_;

		/**
		 * @brief The node to start building from
		*/
		jc::reference_ptr<MoveTree::Node> node_;

		/**
		 * @brief How deep to build
		*/
		size_t depth_;
	};

	/**
	 * @brief Provides an interface around a thread for running TreeBuildTask(s)
	*/
	class TreeBuildThread
	{
	private:

		struct DropBarrierGuarded
		{
		public:

			void reset()
			{
				this->barrier_->barrier_.arrive_and_drop();
			};

			DropBarrierGuarded(jc::reference_ptr<TreeBuildThread> _barrier) :
				barrier_{ _barrier }
			{};

		private:
			jc::reference_ptr<TreeBuildThread> barrier_;
		};
		friend DropBarrierGuarded;

		using DropBarrierGuard = jc::guard<DropBarrierGuarded>;



		/**
		 * @brief The actual main function for the thread
		 * @param _stop Stop token
		 * @param _thread The thread object that owns this thread
		*/
		static void thread_main(std::stop_token _stop, jc::reference_ptr<TreeBuildThread> _thread)
		{
			auto& _barrier = _thread->barrier_;
			auto& _task = _thread->task_;

			// Ensures that the barrier is dropped when exiting the thread
			DropBarrierGuard _barrierGuard{ _thread };

			std::string _threadID{};
			{
				std::stringstream _sstr{};
				_sstr << std::this_thread::get_id();
				_threadID = _sstr.str();
			};

			// Run until done
			while (!_stop.stop_requested())
			{
				// Wait until a task was set or we are being queued to exit
				_barrier.arrive_and_wait();

				// Skip if no task was set
				{
					// Lock our mutex
					auto _lck = std::unique_lock{ _thread->mtx_ };

					if (!_task.has_value())
					{
						continue;
					};
					
					// Run the task
					_task->invoke();

					// Remove task
					_task.reset();
				};

			};

		};

	public:

		void assign_work(TreeBuildTask _task)
		{
			// Set the task
			{
				auto _lck = std::unique_lock{ this->mtx_ };
				this->task_ = std::move(_task);
			};

			// Allow thread to continue
			this->barrier_.arrive_and_wait();
		};

		bool is_working() const
		{
			return this->task_.has_value();
		};

		void wait_until_finished()
		{
			this->barrier_.arrive_and_wait();
		};





		TreeBuildThread() :
			barrier_{ 2, {} },
			thread_{ thread_main, jc::reference_ptr{ *this } },
			task_{ std::nullopt }
		{};

		~TreeBuildThread()
		{
			// Activate stop token
			if (this->thread_.request_stop())
			{
				// Remove task
				{
					auto _lck = std::unique_lock{ this->mtx_ };
					this->task_.reset();
				};

				// Allow thread to continue
				this->barrier_.arrive_and_drop();
			};
		};

	private:

		/**
		 * @brief Barrier used to hold the thread when no task is assigned
		*/
		mutable std::barrier<> barrier_;

		/**
		 * @brief The actual thread object
		*/
		std::jthread thread_;

		/**
		 * @brief Mutex for our data
		*/
		mutable std::mutex mtx_;

		/**
		 * @brief The task assigned to this thread
		*/
		std::optional<TreeBuildTask> task_;
		
		// Prevent move/copy

		TreeBuildThread(const TreeBuildThread& other) = delete;
		TreeBuildThread& operator=(const TreeBuildThread& other) = delete;

		TreeBuildThread(TreeBuildThread&& other) noexcept = delete;
		TreeBuildThread& operator=(TreeBuildThread&& other) noexcept = delete;
	};


}