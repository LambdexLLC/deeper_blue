#pragma once

#include "tree_build.hpp"

#include <lambdex/chess/move_tree.hpp>
#include <lambdex/chess/chess_engine.hpp>

#include "utility/format.hpp"
#include "utility/filesystem.hpp"

#include <fstream>


namespace lbx::chess
{

	/**
	 * @brief Babys first bot that isnt random
	*/
	class ChessEngine_Baby : public IChessEngine
	{
	private:

		/**
		 * @brief Contains information about a turn played by this engine.
		*/
		struct TurnStats
		{
		public:

			/**
			 * @brief The full time to play the turn.
			*/
			std::chrono::duration<double> turn_duration{ 0.0f };
			
			/**
			 * @brief The time it took to make the move tree.
			*/
			std::chrono::duration<double> tree_build_duration{ 0.0f };

			/**
			 * @brief The time it took to search through the move tree.
			*/
			std::chrono::duration<double> tree_search_duration{ 0.0f };

			/**
			 * @brief The list of move lines the engine searched down sorted from best to worst.
			*/
			std::vector<RatedLine> possible_lines{};

			/**
			 * @brief The depth of search for the move tree.
			*/
			size_t search_depth = 0;

			/**
			 * @brief The total number of nodes constructed in the move tree.
			*/
			size_t move_tree_node_count = 0;

			/**
			 * @brief The initial board state.
			*/
			BoardWithState initial_board{};
		};

		/**
		 * @brief Determines the best move to play.
		 * 
		 * @param _board The state of the chess board.
		 * @param _player The player who we are playing as.
		 * @param _stats Optional stats object to fill out.
		 * @return The best move in our opinion.
		*/
		Move determine_best_move(const BoardWithState& _board, Color _player, TurnStats* _stats = nullptr);

		/**
		 * @brief Determines the search depth to use for a give board state.
		 * @param _board Chess board to get search depth for.
		 * @param _stats Optional stats object to fill out.
		 * @return Search depth.
		*/
		size_t determine_search_depth(const BoardWithState& _board, TurnStats* _stats = nullptr) const;

		/**
		 * @brief Constructs a move tree for a chess board.
		 * 
		 * This will use the thread pool to parralelize the construction of the tree.
		 * 
		 * @param _board Chess board initial state.
		 * @param _depth Depth for the tree.
		 * @param _stats Optional stats object to fill out.
		 * @return Constructed move tree.
		*/
		MoveTree construct_move_tree(const BoardWithState& _board, size_t _depth, TurnStats* _stats = nullptr);

	public:

		/**
		 * @brief Plays a turn using this chess engine
		 * @param _game The game to play a turn in.
		*/
		void play_turn(IGameInterface& _game) final;



		ChessEngine_Baby();

	private:

		constexpr static std::string_view dump_root_v = SOURCE_ROOT "/dump";

		struct Logger
		{
		public:

			std::ofstream start_logging_move()
			{
				fs::path _movePath{};
				do
				{
					_movePath = this->folder_ / format("move_{}.txt", this->move_number_++);
				}
				while (fs::exists(_movePath));
				return std::ofstream{ _movePath };
			};

			void append_log(const TurnStats& _stats);

			Logger(std::string _gameName)
			{
				this->folder_ = format("{}/{}", dump_root_v, _gameName);
				if (!fs::exists(this->folder_))
				{
					fs::create_directory(this->folder_);
				}
				else
				{
					auto& n = this->move_number_;
					for (n; n != 1000; ++n)
					{
						const auto _moveDumpFileName = this->folder_ / format("move_{}", n);
						if (!fs::exists(_moveDumpFileName))
						{
							// Found our move
							break;
						};
					};

					// Check we found a number
					if (n == 1000)
					{
						JCLIB_ABORT();
					};
				};
			};

		private:
			fs::path folder_;
			int move_number_ = 1;
		};
		friend Logger;

		std::optional<Logger> logger_{};


		/**
		 * @brief Just the one for now
		*/
		basic_worker_pool<TreeBuildTask> build_pool_{ 8 };

	};
};