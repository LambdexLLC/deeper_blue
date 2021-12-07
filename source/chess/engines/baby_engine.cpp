#include "chess/board/bitboard.hpp"

#include "baby_engine.hpp"

#include "chess/chess.hpp"

#include "utility/io.hpp"

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
				continue;
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
		const auto _intialBoard = _previous->move_.get_outcome_board();
		auto _moves = this->rank_possible_moves(_intialBoard, _intialBoard.turn);
		for (auto& m : _moves)
		{
			_previous->responses_.push_back(std::unique_ptr<MoveTree::Node>{
				new MoveTree::Node{ _previous, m, {} }
			});
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
				this->calculate_move_tree_node_responses(r.get(), _depth);
			};
		};
	};

	MoveTree ChessEngine_Baby::make_move_tree(const BoardWithState& _board, size_t _depth)
	{
		MoveTree _out{};
		_out.initial_board_ = _board;

		auto _moves = this->rank_possible_moves(_board, _board.turn);
		for (auto& m : _moves)
		{
			_out.moves_.push_back(std::unique_ptr<MoveTree::Node>{ new MoveTree::Node { nullptr, m, {} } });
		};

		if (_depth != 0)
		{
			--_depth;
			for (auto& r : _out.moves_)
			{
				this->calculate_move_tree_node_responses(r.get(), _depth);
			};
		};

		return _out;
	};


	std::vector<ChessEngine_Baby::RatedLine> ChessEngine_Baby::pick_best_from_tree(const MoveTree& _tree)
	{
		using Node = MoveTree::Node;
		Node* _at = {};

		std::vector<RatedLine> _finalPicks{};
		for (auto& r : _tree.moves_)
		{
			RatedLine _line{};
			_at = r.get();
			_line.push_back(_at->move_);

			while (true)
			{
				Node* _opponentResponse{};
				if (auto& _rsrs = _at->responses_; !_rsrs.empty())
				{
					_opponentResponse = _rsrs.front().get();
				}
				else
				{
					break;
				};

				if (auto& _rsrs = _opponentResponse->responses_; !_rsrs.empty())
				{
					_line.push_back(_opponentResponse->move_);
					_at = _opponentResponse->responses_.front().get();
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
		int _pieceCount = 0;
		for (auto& p : _board)
		{
			if (p != Piece::empty)
			{
				++_pieceCount;
			};
		};

		size_t _treeDepth{};
		if (_pieceCount > 6)
		{
			_treeDepth = 3;
		}
		else
		{
			_treeDepth = 5;
		};

		auto _moveTree = this->make_move_tree(_board, _treeDepth);
		auto _lines = this->pick_best_from_tree(_moveTree);
		
		static int move_n = 0;

		{
			auto& _bestLine = _lines.front();
			
			std::string fp = SOURCE_ROOT "/game4/move_" + std::to_string(move_n++) + ".txt";
			std::ofstream f{ fp };
			
			bool _myTurn = true;
			f << "\ninitial:\n" << _board << '\n';

			auto _possibleMoves = this->rank_possible_moves(_board, _player);
			f << "possible moves:\n";
			for (auto& m : _possibleMoves)
			{
				writeln(f, "\t{} ({})", m.get_move(), m.get_rating());
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

				f << m.get_outcome_board();
				_myTurn = !_myTurn;
			};

			f.flush();
		}
		
	
		// Grab the individual moves
		std::vector<Move> _final(_lines.size());
		auto it = _final.begin();
		for (auto& m : _lines)
		{
			*it = m.front().get_move();
			++it;
		};
		
		return _final;
	};

};
