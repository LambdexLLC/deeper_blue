#include "tree_build.hpp"

#include <span>


namespace lbx::chess
{

	std::vector<RatedMove> TreeBuilder::rank_possible_moves(const BoardWithState& _board)
	{
		// Generate possible boards from 1 move
		auto _randomMoves = chess::find_possible_moves(_board);
		std::vector<RatedMove> _rankedMoves(_randomMoves.size());
		auto _it = _rankedMoves.begin();

		for (auto& m : std::span{ _randomMoves.data(), _randomMoves.size() })
		{
			*_it = rate_move<BoardRater_Complete>(_board, m);
			++_it;
		};

		// Sort by value
		std::ranges::sort(_rankedMoves, jc::greater);
		return _rankedMoves;
	};


	void TreeBuilder::calculate_move_tree_node_responses(const BoardWithState& _board, MoveTree::Node* _previous)
	{
		if (_previous->get_rating() < -10000 || _previous->get_rating() > 10000)
		{
			return;
		}
		else
		{
			auto _moves = this->rank_possible_moves(_board);
			_previous->set_responses(_moves);
		};
	};
	void TreeBuilder::calculate_move_tree_node_responses(const BoardWithState& _board, MoveTree::Node* _previous, size_t _depth)
	{
		this->calculate_move_tree_node_responses(_board, _previous);
		if (_depth != 0 && _previous->has_responses())
		{
			--_depth;
			for (auto& r : _previous->responses())
			{
				BoardWithState _newBoard{ _board };
				apply_move(_newBoard, r.get_move());
				this->calculate_move_tree_node_responses(_newBoard, &r, _depth);
			};
		};
	};

	MoveTree TreeBuilder::make_move_tree(const BoardWithState& _board)
	{
		MoveTree _out{};
		_out.initial_board_ = _board;

		auto _moves = this->rank_possible_moves(_board);
		_out.moves_.resize(_moves.size());
		std::ranges::copy(_moves, _out.moves_.begin());

		return _out;
	};
	MoveTree TreeBuilder::make_move_tree(const BoardWithState& _board, size_t _depth)
	{
		auto _out = this->make_move_tree(_board);
		if (_depth != 0)
		{
			--_depth;
			for (auto& r : _out.moves_)
			{
				BoardWithState _newBoard{ _board };
				apply_move(_newBoard, r.get_move());
				this->calculate_move_tree_node_responses(_newBoard, &r, _depth);
			};
		};
		return _out;
	};


	const MoveTree::Node* TreeBuilder::pick_best_from_tree(const MoveTree::Node& _node, RatedLine& _line)
	{
		// Add the current move to the line
		_line.push_back(_node);

		// Look at opponent's responses to this move
		const auto _responses = _node.responses();
		std::vector<std::pair<RatedLine, const MoveTree::Node*>> _responseLines(_responses.size());
		auto _responseIt = _responseLines.begin();
		for (auto& m : _responses)
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


	/**
	 * @brief Gets the rating of the last move in a line from the POV of a particular player.
	 *
	 * @param _line Line to get move from.
	 * @param _player Player to get rating of the move for, they must have played the first move in the line.
	 * @return Move rating
	*/
	int last_move_rating(const RatedLine& _line, Color _player)
	{
		const auto _rawRating = _line.back().get_rating();
		if ((_line.size() % 2) == 0)
		{
			return -_rawRating;
		}
		else
		{
			return _rawRating;
		};
	};






	std::vector<RatedLine> TreeBuilder::pick_best_from_tree(const MoveTree& _tree)
	{
		using Node = MoveTree::Node;
		const Node* _at = {};

		std::vector<RatedLine> _finalPicks{};
		for (auto& r : _tree.moves_)
		{
			RatedLine _line{};
			_at = &r;
			_line.push_back(*_at);

			while (true)
			{
				// Determine the opponent's best move
				const Node* _opponentResponse{};

				if (_at->has_responses())
				{
					// Look through opponent's responses to our move
					auto _rsrs = _at->responses();

					// Only look if there are responses to our move possible
					if (!_rsrs.empty())
					{
						// Build up look-ahead rating for opponent response
						std::vector<std::pair<const Node*, int>> _responseToResponseRatings{};
						for (auto& _rsrsrs : _rsrs)
						{
							if (_rsrsrs.has_responses())
							{
								_responseToResponseRatings.push_back(std::pair<const Node*, int>{ &_rsrsrs, -_rsrsrs.responses().front().get_rating()});
							}
							else
							{
								_responseToResponseRatings.push_back({ &_rsrsrs, _rsrsrs.get_rating() });
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
						// There are no possible responses to our move
						break;
					};
				}
				else
				{
					// There are no possible responses to our move
					break;
				};

				// If the opponent has a response to our move, look into our responses to its
				// response and determine the best
				if (_opponentResponse)
				{
					// Check that we can response to it
					if (_opponentResponse->has_responses())
					{
						// Add their response to the line
						_line.push_back(*_opponentResponse);
						_at = &_opponentResponse->responses().front();
						_line.push_back(*_at);
					}
					else
					{
						_line.push_back(*_opponentResponse);
						break;
					};
				};
			};

			// Add the line to the final set of lines to evaluate
			_finalPicks.push_back(_line);
		};

		// My side's color
		const auto _myColor = _tree.initial_board_.turn;

		// Sort lines based on final outcome so that the first line in the container
		// is the one with the best outcome for us
		std::ranges::sort(_finalPicks, [_myColor](const RatedLine& lhs, const RatedLine& rhs)
			{
				// Make sure we get the rating of the last move from OUR POV
				const auto _lhsRating = last_move_rating(lhs, _myColor);
				const auto _rhsRating = last_move_rating(rhs, _myColor);
				return _lhsRating > _rhsRating;
			});

		// Return our sorted lines
		return _finalPicks;
	};


}