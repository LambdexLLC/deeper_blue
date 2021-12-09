#include "baby_engine.hpp"

#include "utility/io.hpp"

#include <lambdex/chess/chess.hpp>

#include <jclib/timer.h>
#include <jclib/ranges.h>
#include <jclib/functional.h>

#include <array>
#include <vector>
#include <ranges>
#include <fstream>
#include <algorithm>

namespace lbx::chess
{
	constexpr auto dc = [](const auto& v) { return std::chrono::duration_cast<std::chrono::duration<double>>(v); };

	/**
	 * @brief Checks if the given board is checkmate
	 * @param _board Board to check
	 * @param _player Player to test for checkmate of
	 * @return True if checkmate
	*/
	inline bool is_checkmate(const BoardWithState& _board, Color _player)
	{
		const auto _kingPosOpt = _board.find(Piece::king | _player);
		if (!_kingPosOpt)
		{
			return true;
		};

		const auto _kingPos = *_kingPosOpt;
		if (is_piece_threatened(_board, _kingPos))
		{
			// Check if there are any possible moves
			ChessEngine_Random _random{};
			const auto _possibleMoves = _random.calculate_multiple_moves(_board, _player);
			if (_possibleMoves.empty())
			{
				return true;
			}
			else
			{
				return false;
			};
		}
		else
		{
			return false;
		};
	};

	struct SmartRater
	{
		int rate(const BoardWithState& _state, Color _player) const
		{
			auto _base = SimpleRater{}.rate(_state, _player);
			const auto _playerName = (_player == Color::white) ? "white" : "black";
			const auto _notPlayerName = (_player == Color::white) ? "black" : "white";

			if (is_checkmate(_state, !_player))
			{
				_base += 1000000;
			}
			else if (is_checkmate(_state, _player))
			{
				_base -= 1000000;
			};
			return _base;
		};
	};

	std::vector<RatedMove> ChessEngine_Baby::rank_possible_moves(const BoardWithState& _board, Color _player)
	{
		SmartRater _rater{};
		std::vector<RatedMove> _rankedMoves{};

		// Generate possible boards from 1 move
		auto _randomMoves = this->random_fallback_.calculate_multiple_moves(_board, _player);
		for (auto& m : _randomMoves)
		{
			auto _rated = rate_move(_board, m, _rater);
			if (_rated.get_rating() < -10000)
			{
				_rankedMoves.push_back(_rated);
			}
			else
			{
				_rankedMoves.push_back(_rated);
			};
		};

		// Sort by value
		std::ranges::sort(_rankedMoves, jc::greater);
		return _rankedMoves;
	};


	void ChessEngine_Baby::calculate_move_tree_node_responses(MoveTree::Node* _previous)
	{
		if (_previous->move_.get_rating() < -10000 || _previous->move_.get_rating() > 10000)
		{
			return;
		}
		else
		{
			const auto _intialBoard = _previous->move_.get_outcome_board();
			auto _moves = this->rank_possible_moves(_intialBoard, _intialBoard.turn);
			auto& _prevResponses = _previous->responses_;
			_prevResponses.resize(_moves.size());
			std::ranges::copy(_moves, _prevResponses.begin());
		};
	};
	void ChessEngine_Baby::calculate_move_tree_node_responses(MoveTree::Node* _previous, size_t _depth)
	{
		this->calculate_move_tree_node_responses(_previous);
		if (_depth != 0)
		{
			--_depth;
			for (auto& r : _previous->responses_)
			{
				this->calculate_move_tree_node_responses(&r, _depth);
			};
		};
	};

	MoveTree ChessEngine_Baby::make_move_tree(const BoardWithState& _board, size_t _depth)
	{
		MoveTree _out{};
		_out.initial_board_ = _board;

		jc::timer _tm{};
		_tm.start();

		auto _moves = this->rank_possible_moves(_board, _board.turn);
		_out.moves_.resize(_moves.size());
		std::ranges::copy(_moves, _out.moves_.begin());

		println("finding possible moves = {}", dc(_tm.elapsed()).count());

		if (_depth != 0)
		{
			--_depth;
			for (auto& r : _out.moves_)
			{
				this->calculate_move_tree_node_responses(&r, _depth);
			};
		};

		return _out;
	};

	const MoveTree::Node* ChessEngine_Baby::pick_best_from_tree(const MoveTree::Node& _node, RatedLine& _line)
	{
		// Add the current move to the line
		_line.push_back(_node.move_);

		// Look at opponent's responses to this move
		std::vector<std::pair<RatedLine, const MoveTree::Node*>> _responseLines(_node.responses_.size());
		auto _responseIt = _responseLines.begin();
		for (auto& m : _node.responses_)
		{
			// Get their best response
			RatedLine _responseLine{};
			this->pick_best_from_tree(m, _responseLine);
			_responseIt->second = &m;
			_responseIt->first = std::move(_responseLine);
			++_responseIt;
		};

		// Determine our best response to their best response
		if (!_responseLines.empty())
		{
			// Determine which of thier responses is their best
			std::ranges::sort(_responseLines, [](const auto& lhs, const auto& rhs) -> bool
				{
					return lhs.first.front() > rhs.first.front();
				});
			auto& _theirBest = _responseLines.front();

			// Determine our best response to it
			
		}
		else
		{
			// This move is as good as it gets, return null
			return nullptr;
		};
	};

	std::vector<ChessEngine_Baby::RatedLine> ChessEngine_Baby::pick_best_from_tree(const MoveTree& _tree)
	{
		using Node = MoveTree::Node;
		const Node* _at = {};

		std::vector<RatedLine> _finalPicks{};
		for (auto& r : _tree.moves_)
		{
			RatedLine _line{};
			_at = &r;
			_line.push_back(_at->move_);

			while (true)
			{
				const Node* _opponentResponse{};
				if (auto& _rsrs = _at->responses_; !_rsrs.empty())
				{
					// Build up look-ahead rating for opponent response
					std::vector<std::pair<const Node*, int>> _responseToResponseRatings{};
					for (auto& _rsrsrs : _rsrs)
					{
						if (!_rsrsrs.responses_.empty())
						{
							_responseToResponseRatings.push_back(std::pair<const Node*, int>{ &_rsrsrs, -_rsrsrs.responses_.front().move_.get_rating()});
						}
						else
						{
							_responseToResponseRatings.push_back({ &_rsrsrs, _rsrsrs.move_.get_rating() });
						};
					};

					// Sort look-ahead responses
					std::ranges::sort(_responseToResponseRatings, [](auto& lhs, auto& rhs)
						{
							return lhs.second > rhs.second;
						});
					_opponentResponse = _responseToResponseRatings.front().first;
				}
				else
				{
					break;
				};

				if (auto& _rsrs = _opponentResponse->responses_; !_rsrs.empty())
				{
					_line.push_back(_opponentResponse->move_);
					_at = &_opponentResponse->responses_.front();
					_line.push_back(_at->move_);
				}
				else
				{
					break;
				};
			};

			_finalPicks.push_back(_line);
		};

		std::ranges::sort(_finalPicks, [](const RatedLine& lhs, const RatedLine& rhs)
			{
				return lhs.back() > rhs.back();
			});
		return _finalPicks;
	};



	std::vector<Move> ChessEngine_Baby::calculate_multiple_moves(const BoardWithState& _board, Color _player)
	{
		jc::timer _tm{};
		_tm.start();

		int _pieceCount = 0;
		for (auto& p : _board)
		{
			if (p != Piece::empty)
			{
				++_pieceCount;
			};
		};

		size_t _treeDepth = 3;

		auto _moveTree = this->make_move_tree(_board, _treeDepth);
		const auto _treeTime = _tm.elapsed();
		_tm.start();

		auto _lines = this->pick_best_from_tree(_moveTree);
		const auto _pickTime = _tm.elapsed();
		_tm.start();

		static int move_n = 0;

		if (_lines.empty())
		{
			return {};
		};

		{
			auto& _bestLine = _lines.front();
			std::ofstream f = this->logger_.start_logging_move();
			
			bool _myTurn = true;
			f << "\ninitial:\n" << _board << '\n';
			f << "possible moves:\n";
			for (auto& m : _lines)
			{
				writeln(f, "\t{} ({}) (final = {})", m.front().get_move(), m.front().get_rating(), m.back().get_rating());
			};

			for (auto& m : _bestLine)
			{
				if (_myTurn)
				{
					f << "\nme:\n";
				}
				else
				{
					f << "\nopponent:\n";
				};
				
				f << m.get_move().to_string() << '\n' << m.get_outcome_board();
				_myTurn = !_myTurn;
			};

			f.flush();
		}
		
		const auto _logTime = _tm.elapsed();

		// Grab the individual moves
		std::vector<Move> _final(_lines.size());
		auto it = _final.begin();
		for (auto& m : _lines)
		{
			*it = m.front().get_move();
			++it;
		};
		
		println("tree = {} pick = {} log = {}", dc(_treeTime).count(), dc(_pickTime).count(), dc(_logTime).count());

		return _final;
	};


	void ChessEngine_Baby::play_turn(IGameInterface& _game)
	{
		const auto _moves = this->calculate_multiple_moves(_game.get_board(), _game.get_color());
		for (auto& m : _moves)
		{
			if (_game.submit_move(m))
			{
				return;
			};
		};
		_game.resign();
	};

};
