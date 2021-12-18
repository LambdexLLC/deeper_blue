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

		std::vector<Move> calculate_multiple_moves(const BoardWithState& _board, Color _player);

	public:

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