#include "baby_engine.hpp"

#include "utility/io.hpp"

#include <lambdex/chess/chess.hpp>
#include <lambdex/chess/fen.hpp>
#include <lambdex/chess/piece_movement.hpp>

#include <jclib/guard.h>
#include <jclib/timer.h>
#include <jclib/ranges.h>
#include <jclib/functional.h>

#include <mutex>
#include <array>
#include <vector>
#include <ranges>
#include <fstream>
#include <algorithm>

#include <barrier>


namespace lbx::chess
{
	constexpr auto dc = [](const auto& v) { return std::chrono::duration_cast<std::chrono::duration<double>>(v); };


	/**
	 * @brief Determines the search depth to use for a give board state.
	 * @param _board Chess board to get search depth for.
	 * @return Search depth.
	*/
	size_t ChessEngine_Baby::determine_search_depth(const BoardWithState& _board) const
	{
		const auto _complexity = rate_complexity(_board);
		size_t _treeDepth = 3;

		if (_complexity <= 50)
		{
			_treeDepth = 7;
		}
		else if (_complexity <= 150)
		{
			_treeDepth = 6;
		}
		else if (_complexity <= 250)
		{
			_treeDepth = 5;
		}
		else if (_complexity <= 500)
		{
			_treeDepth = 4;
		}
		else
		{
			_treeDepth = 3;
		};

		return _treeDepth;
	};

	/**
	 * @brief Constructs a move tree for a chess board.
	 *
	 * This will use the thread pool to parralelize the construction of the tree.
	 *
	 * @param _board Chess board initial state.
	 * @param _depth Depth for the tree.
	 * @return Constructed move tree.
	*/
	MoveTree ChessEngine_Baby::construct_move_tree(const BoardWithState& _board, size_t _depth)
	{
		TreeBuilder _builder{};

		// Create the initial set of responses
		auto _moveTree = _builder.make_move_tree(_board);

		// Fill out branches
		{
			auto& _buildThreads = this->build_threads_;

			auto _buildThreadIt = _buildThreads.begin();
			const auto _buildThreadsEnd = _buildThreads.end();

			// Assign nodes out to the threads
			for (auto& m : _moveTree)
			{
				auto _board = _moveTree.initial_board_;
				apply_move(_board, m.get_move());
				TreeBuildTask _task{ _board, m, _depth };

				// Keep going until we've assigned the task to a thread
				while (true)
				{
					const auto _assignedWork = !_buildThreadIt->is_working();
					if (_assignedWork)
					{
						_buildThreadIt->assign_work(_task);
					};

					++_buildThreadIt;
					if (_buildThreadIt == _buildThreadsEnd)
					{
						_buildThreadIt = _buildThreads.begin();
						std::this_thread::yield();
					};

					if (_assignedWork)
					{
						break;
					};
				};
			};

			// Make sure they've all finished
			for (auto& _thread : _buildThreads)
			{
				_thread.wait_until_finished();
			};
		};

		// Return finished tree
		return _moveTree;
	};

	/**
	 * @brief Determines the best move to play.
	 *
	 * @param _board The state of the chess board.
	 * @param _player The player who we are playing as.
	 * @return The best move in our opinion.
	*/
	Move ChessEngine_Baby::determine_best_move(const BoardWithState& _board, Color _player)
	{
		jc::timer _tm{};
		_tm.start();

		jc::timer _turnTime{};
		_turnTime.start();

		
		const auto _treeDepth = this->determine_search_depth(_board);


		const auto _treeTime = _tm.elapsed();
		_tm.start();
		auto _moveTree = this->construct_move_tree(_board, _treeDepth);

		TreeBuilder _builder{};

		auto _lines = _builder.pick_best_from_tree(_moveTree);
		const auto _pickTime = _tm.elapsed();
		_tm.start();



		// If we did not find a line to play, go ahead and return nothing
		if (_lines.empty())
		{
			return {};
		};

		// Grab the individual moves
		std::vector<Move> _final(_lines.size());
		auto it = _final.begin();
		for (auto& m : _lines)
		{
			if (m.empty()) { __debugbreak(); };
			*it = m.front().get_move();
			++it;
		};

		// How long it took to play the turn
		const auto _fullTurnTime = _turnTime.elapsed();

		// Logging for the selected line to play
		if (auto& _logger = this->logger_; _logger)
		{
			auto& _bestLine = _lines.front();
			std::ofstream f = _logger->start_logging_move();
		
			{
				f << "Stats:\n";
				writeln(f, "full turn time  = {}s", dc(_fullTurnTime).count());
				writeln(f, "tree build time = {}s", dc(_treeTime).count());
				writeln(f, "pick time       = {}s", dc(_pickTime).count());

				writeln(f,
R"(
==================================================
				Inputs and ideas
==================================================
)");

				bool _myTurn = true;
				writeln(f, "\ninitial : fen = {}\n", chess::get_board_fen(_board));
				f << _board << '\n';
				f << "possible moves:\n";
				for (auto& m : _lines)
				{
					write(f, "\t{{ ");
					
					_myTurn = true;
					for (auto& _move : m)
					{
						auto _rating = _move.get_rating();
						if (!_myTurn)
						{
							_rating *= -1;
						};

						write(f, "{}({}) ", _move.get_move(), _rating);
						_myTurn = !_myTurn;
					};
					writeln(f, " }}");
				};

				BoardWithState _lineBoard{ _board };
				for (size_t n = 0; n != _lines.size() && n != 3; ++n)
				{
					writeln(f,
R"(
==================================================
					Move line {}
==================================================
)", n);
					// The line we will be logging
					auto& _line = _lines[n];
					
					// Reset the board
					_lineBoard = _board;
					_myTurn = true;

					// Loop through move in the move line
					for (auto& m : _line)
					{
						if (_myTurn)
						{
							writeln(f, "\nme : {}\n", m.get_rating());
						}
						else
						{
							writeln(f, "\nopponent : {}\n", m.get_rating());
						};

						apply_move(_lineBoard, m.get_move());
						f << m.get_move().to_string() << " fen = " << chess::get_board_fen(_lineBoard) << '\n' << _lineBoard;

						_myTurn = !_myTurn;
					};
				};
			};

			f.flush(); 
		}

		return _final.front();
	};




	void ChessEngine_Baby::play_turn(IGameInterface& _game)
	{
		if (!this->logger_)
		{
			const auto _name = _game.get_game_name();
			if (!_name.empty())
			{
				this->logger_ = Logger{ _game.get_game_name() };
			};
		};

		println("playing turn for game {}", _game.get_game_name());

		const auto _move = this->determine_best_move(_game.get_board(), _game.get_color());
		if (!_game.submit_move(_move))
		{
			_game.resign();
		};
	};


	ChessEngine_Baby::ChessEngine_Baby()
	{};
};
