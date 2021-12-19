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
		 * @brief Determines the best move to play.
		 * 
		 * @param _board The state of the chess board.
		 * @param _player The player who we are playing as.
		 * @return The best move in our opinion.
		*/
		Move determine_best_move(const BoardWithState& _board, Color _player);

		/**
		 * @brief Determines the search depth to use for a give board state.
		 * @param _board Chess board to get search depth for.
		 * @return Search depth.
		*/
		size_t determine_search_depth(const BoardWithState& _board) const;

		/**
		 * @brief Constructs a move tree for a chess board.
		 * 
		 * This will use the thread pool to parralelize the construction of the tree.
		 * 
		 * @param _board Chess board initial state.
		 * @param _depth Depth for the tree.
		 * @return Constructed move tree.
		*/
		MoveTree construct_move_tree(const BoardWithState& _board, size_t _depth);

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

		std::optional<Logger> logger_{};


		/**
		 * @brief Just the one for now
		*/
		std::array<TreeBuildThread, 8> build_threads_{};

	};
};