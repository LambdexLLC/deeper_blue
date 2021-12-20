#include "baby_engine.hpp"

#include "utility/io.hpp"
#include "utility/json.hpp"

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


namespace lbx::chess
{
	constexpr auto dc = [](const auto& v) { return std::chrono::duration_cast<std::chrono::duration<double>>(v); };



	size_t ChessEngine_Baby::determine_search_depth(const BoardWithState& _board, TurnStats* _stats) const
	{
		const auto _complexity = rate_complexity(_board);
		size_t _treeDepth = 3;

		if (_complexity <= 50)
		{
			_treeDepth = 7;
		}
		else if (_complexity <= 100)
		{
			_treeDepth = 6;
		}
		else if (_complexity <= 150)
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

	MoveTree ChessEngine_Baby::construct_move_tree(const BoardWithState& _board, size_t _depth, TurnStats* _stats)
	{
		TreeBuilder _builder{};

		if (_depth <= 2)
		{
			// Construct tree in this thread
			return _builder.make_move_tree(_board, _depth);
		}
		else
		{
			// Contruct tree using thread pool

			// Create the initial set of responses
			auto _moveTree = _builder.make_move_tree(_board);

			// Fill out branches
			{
				auto& _buildPool = *this->build_pool_;

				// Assign nodes out to the threads
				for (auto& m : _moveTree)
				{
					// Create the task
					auto _board = _moveTree.initial_board_;
					apply_move(_board, m.get_move());
					TreeBuildTask _task{ _board, m, _depth - 1 };

					// Assign work to pool
					_buildPool.assign_work(std::move(_task));
				};

				// Wait until pool is finished
				_buildPool.wait_until_all_finished();
			};

			// Return finished tree
			return _moveTree;
		};
	};

	Move ChessEngine_Baby::determine_best_move(const BoardWithState& _board, Color _player, TurnStats* _stats)
	{
		if (_stats)
		{
			_stats->initial_board = _board;
		};


		jc::timer _tm{};
		_tm.start();

		jc::timer _turnTime{};
		_turnTime.start();
		
		// Determine how deep to search
		const auto _treeDepth = this->determine_search_depth(_board);
		if (_stats)
		{
			_stats->search_depth = _treeDepth;
		};

		// Build our move tree
		auto _moveTree = this->construct_move_tree(_board, _treeDepth);
		const auto _treeTime = _tm.elapsed();

		if (_stats)
		{
			_stats->move_tree_node_count = _moveTree.child_count();
			_stats->tree_build_duration = _treeTime;
		};
		
		_tm.start();


		// Search the move tree to find the set of lines we may play

		TreeBuilder _builder{};

		auto _lines = _builder.pick_best_from_tree(_moveTree);
		const auto _pickTime = _tm.elapsed();
		
		if (_stats)
		{
			_stats->possible_lines = _lines;
			_stats->tree_search_duration = _pickTime;
		};

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

		if (_stats)
		{
			_stats->turn_duration = _fullTurnTime;
		};

		return _final.front();
	};


	void ChessEngine_Baby::Logger::append_log(const TurnStats& _stats)
	{
		std::ofstream f = this->start_logging_move();

		json _json = json::object();
		
		_json["initial_board"] = chess::get_board_fen(_stats.initial_board);

		{
			json _times = json::object();
			_times["turn"] = _stats.turn_duration.count();
			_times["tree_build"] = _stats.tree_build_duration.count();
			_times["tree_search"] = _stats.tree_search_duration.count();
			_json["times"] = _times;
		};

		{
			json _tree = json::object();
			_tree["depth"] = _stats.search_depth;
			_tree["size"] = _stats.move_tree_node_count;
			_json["tree"] = _tree;
		};
		
		{
			json _lines = json::array();
			for (auto& l : _stats.possible_lines)
			{
				json _line = json::array();
				bool _myTurn = true;
				for (auto& m : l)
				{
					json _move = json::object();
					_move["move"] = m.get_move().to_string();
					
					if (_myTurn)
					{
						_move["rating"] = m.get_rating();
					}
					else
					{
						_move["rating"] = -m.get_rating();
					};
					_myTurn = !_myTurn;

					_line.push_back(_move);
				};
				_lines.push_back(_line);
			};
			_json["lines"] = _lines;
		};

		f << _json.dump(1, '\t');
		f.flush();
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

		TurnStats _stats{};
		const auto _move = this->determine_best_move(_game.get_board(), _game.get_color(), &_stats);
		
		if (this->logger_)
		{
			this->logger_->append_log(_stats);
		};

		if (!_game.submit_move(_move))
		{
			_game.resign();
		};
	};


	ChessEngine_Baby::ChessEngine_Baby(std::shared_ptr<TreeBuildPool> _pool) :
		build_pool_{ std::move(_pool) }
	{
		JCLIB_ASSERT(this->build_pool_);
	};
};
