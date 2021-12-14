#pragma once

#include "chess/engines/random_engine.hpp"

#include <lambdex/chess/move_tree.hpp>

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

		std::vector<RatedMove> rank_possible_moves(const BoardWithState& _board, Color _player);

		void calculate_move_tree_node_responses(const BoardWithState& _board, MoveTree::Node* _previous, size_t _depth);
		void calculate_move_tree_node_responses(const BoardWithState& _board, MoveTree::Node* _previous);

		MoveTree make_move_tree(const BoardWithState& _board, size_t _depth);



		using RatedLine = std::vector<RatedMove>;

		const MoveTree::Node* pick_best_from_tree(const MoveTree::Node& _node, RatedLine& _line);
		std::vector<RatedLine> pick_best_from_tree(const MoveTree& _tree);


		Move calculate_move(const BoardWithState& _board, Color _player)
		{
			return this->calculate_multiple_moves(_board, _player).front();
		};

		std::vector<Move> calculate_multiple_moves(const BoardWithState& _board, Color _player);

	public:

		void play_turn(IGameInterface& _game) final;





		ChessEngine_Baby(size_t _depth = 3, bool _doLogging = true) :
			search_depth_{ _depth },
			do_logging_{ _doLogging }
		{};

	private:

		/**
		 * @brief Have the random chess engine around as a fallback
		*/
		ChessEngine_Random random_fallback_{};


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

		bool do_logging_ = false;

		/**
		 * @brief How many turns to search down (2 = my turn, your turn)
		*/
		size_t search_depth_;
	};
};