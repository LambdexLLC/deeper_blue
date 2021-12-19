#include "tree_build.hpp"

#include "chess/engines/random_engine.hpp"

#include <span>


namespace lbx::chess
{

	std::vector<RatedMove> TreeBuilder::rank_possible_moves(const BoardWithState& _board)
	{
		// Generate possible boards from 1 move
		auto _randomMoves = ChessEngine_Random{}.calculate_multiple_moves(_board, _board.turn);
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

	
	/**
	 * @brief Find the best response from a move tree node's responses.
	 * 
	 * @param _toNode Node to find best response to.
	 * @return Pointer to the best response, or nullptr if none were found.
	*/
	RatedNode find_best_response(const MoveTree::Node& _toNode, RatedLine* _line)
	{
		// Return null if no responses exist
		if (!_toNode.has_responses())
		{
			return RatedNode{};
		};

		// Kill me
		struct RatedNodeWithLine : public RatedNode
		{
			RatedLine line;
		};

		// The responses to the node
		const auto _responses = _toNode.responses();

		std::vector<RatedNodeWithLine> _deepRatedResponses{};
		{
			RatedNodeWithLine _fillval{};
			if (_line)
			{
				_fillval.line = *_line;
			};
			_deepRatedResponses.resize(_responses.size(), _fillval);
		};

		// Loop over the possible responses we could make
		auto it = _deepRatedResponses.begin();
		for (auto& _response : _responses)
		{
			// Set the deep rated response's pointer
			it->node = &_response;

			// Determine what the opponent's best response would be to this response
			const auto _opponentBestResponse = find_best_response(_response, (_line)? &it->line : nullptr );

			// If there are no responses to this one, then we use the initial response's rating
			if (!_opponentBestResponse.node)
			{
				it->rating = _response.get_rating();
			}
			else
			{
				it->rating = -_opponentBestResponse.rating;
			};

			++it;
		};

		// Sort the deep rated responses
		std::ranges::sort(_deepRatedResponses, [](auto& lhs, auto& rhs)
			{
				if (lhs.rating == rhs.rating)
				{
					return lhs.line.size() > rhs.line.size();
				}
				else
				{
					return lhs.rating > rhs.rating;
				};
			});
		auto& _bestDeepResponse = _deepRatedResponses.front();

		// If we are tracking the move line, append the moves we found to the line
		if (_line)
		{
			_line->push_back(*_bestDeepResponse.node);
			_line->insert(_line->end(), _bestDeepResponse.line.begin(), _bestDeepResponse.line.end());
		};

		// Return the highest deep rated response's node
		return _deepRatedResponses.front();
	};





	std::vector<RatedLine> TreeBuilder::pick_best_from_tree(const MoveTree& _tree)
	{
		const auto& _baseBoard = _tree.initial_board_;


		std::vector<std::pair<RatedLine, RatedNode>> _lines(_tree.moves_.size());
		auto it = _lines.begin();
		for (auto& _move : _tree.moves_)
		{
			RatedLine _line{};
			const auto _myMove = find_best_response(_move, &_line);
			it->first.push_back(_move);
			if (_myMove.node)
			{
				it->first.insert(it->first.end(), _line.begin(), _line.end());
				it->second = _myMove;
				++it;
			}
			else
			{
				it->second.node = &_move;
				it->second.rating = -_move.get_rating();
				++it;
			};
		};


		// Sort by the best evaluated move
		std::ranges::sort(_lines, [](auto& lhs, auto& rhs) -> bool
			{
				return lhs.second.rating < rhs.second.rating;
			});

		std::vector<RatedLine> _out(_lines.size());
		auto _outIt = _out.begin();
		for (auto& l : _lines)
		{
			*_outIt = l.first;
			++_outIt;
		};

		return _out;
	};


}