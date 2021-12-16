#include "baby_engine.hpp"

#include "utility/io.hpp"

#include <lambdex/chess/chess.hpp>
#include <lambdex/chess/fen.hpp>

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
	 * @brief Rates a board based on if castling is possible for the players.
	*/
	struct BoardRater_CastleOpportunity
	{
	public:

		/**
		 * @brief The rating value for being able to castle kingside.
		*/
		Rating kingside_value = 1;

		/**
		 * @brief The rating value for being able to castle queenside.
		*/
		Rating queenside_value = 1;

		/**
		 * @brief Rates a board based on if castling is possible for the players.
		 * 
		 * @param _board Board to rate.
		 * @param _player Player whose POV to rate from.
		 * 
		 * @return Rating based on opportunity to castle.
		*/
		Rating rate(const BoardWithState& _board, Color _player) const
		{
			const auto _myPoints =
				(this->queenside_value * _board.can_player_castle_queenside(_player)) +
				(this->kingside_value * _board.can_player_castle_kingside(_player));
			const auto _opponentPoints =
				(this->queenside_value * _board.can_player_castle_queenside(!_player)) +
				(this->kingside_value * _board.can_player_castle_kingside(!_player));
			return _myPoints - _opponentPoints;
		};
	};
	static_assert(cx_board_rater<BoardRater_CastleOpportunity>);

	/**
	 * @brief Rates a board based on if a player is in checkmate.
	*/
	struct BoardRater_Checkmate
	{
	public:

		/**
		 * @brief The rating value for putting the opponent in checkmate.
		*/
		Rating checkmate_value = 1000000;

		/**
		 * @brief Rates a board based on if any of the players are in checkmate.
		 *
		 * @param _board Board to rate.
		 * @param _player Player whose POV to rate from.
		 *
		 * @return Rating based on checkmates.
		*/
		Rating rate(const BoardWithState& _board, Color _player) const
		{
			if (is_checkmate(_board, _player))
			{
				return -this->checkmate_value;
			}
			else if (is_checkmate(_board, !_player))
			{
				return this->checkmate_value;
			}
			else
			{
				return 0;
			};
		};

	};
	static_assert(cx_board_rater<BoardRater_Checkmate>);

	struct BoardRater_Complete
	{
		int rate(const BoardWithState& _board, Color _player) const
		{
			const auto _materialRating = chess::rate<BoardRater_Material>(_board, _player);
			const auto _checkmateRating = chess::rate<BoardRater_Checkmate>(_board, _player);
			const auto _castleOpportunityRating = chess::rate<BoardRater_CastleOpportunity>(_board, _player);

			const auto _final = _materialRating + _checkmateRating + _castleOpportunityRating;
			return _final;
		};
	};
	static_assert(cx_board_rater<BoardRater_Complete>);

	std::vector<RatedMove> ChessEngine_Baby::rank_possible_moves(const BoardWithState& _board, Color _player)
	{
		// Generate possible boards from 1 move
		auto _randomMoves = this->random_fallback_.calculate_multiple_moves(_board, _player);
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


	void ChessEngine_Baby::calculate_move_tree_node_responses(const BoardWithState& _board, MoveTree::Node* _previous)
	{
		if (_previous->get_rating() < -10000 || _previous->get_rating() > 10000)
		{
			return;
		}
		else
		{
			auto _moves = this->rank_possible_moves(_board, _board.turn);
			_previous->set_responses(_moves);
		};
	};
	void ChessEngine_Baby::calculate_move_tree_node_responses(const BoardWithState& _board, MoveTree::Node* _previous, size_t _depth)
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

	MoveTree ChessEngine_Baby::make_move_tree(const BoardWithState& _board, size_t _depth)
	{
		MoveTree _out{};
		_out.initial_board_ = _board;

		auto _moves = this->rank_possible_moves(_board, _board.turn);
		_out.moves_.resize(_moves.size());
		std::ranges::copy(_moves, _out.moves_.begin());

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

	const MoveTree::Node* ChessEngine_Baby::pick_best_from_tree(const MoveTree::Node& _node, RatedLine& _line)
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
	inline int last_move_rating(const ChessEngine_Baby::RatedLine& _line, Color _player)
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




	std::vector<ChessEngine_Baby::RatedLine> ChessEngine_Baby::pick_best_from_tree(const MoveTree& _tree)
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



	std::vector<Move> ChessEngine_Baby::calculate_multiple_moves(const BoardWithState& _board, Color _player)
	{
		jc::timer _tm{};
		_tm.start();

		jc::timer _turnTime{};
		_turnTime.start();

		// Get number of pieces, this can be used to help tune the search depth
		auto _pieceCount = _board.count_pieces();

		// How many turns to search down
		size_t _treeDepth = 3;

		if (_pieceCount <= 4)
		{
			_treeDepth = 6;
		}
		else if (_pieceCount <= 7)
		{
			_treeDepth = 5;
		}
		else if (_pieceCount <= 12)
		{
			_treeDepth = 4;
		}
		else
		{
			_treeDepth = 4;
		};


		auto _moveTree = this->make_move_tree(_board, _treeDepth);
		const auto _treeTime = _tm.elapsed();
		_tm.start();

		auto _lines = this->pick_best_from_tree(_moveTree);
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
					writeln(f, "\t{} ({}) (final = {})", m.front().get_move(), m.front().get_rating(), last_move_rating(m, _player));
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

					_lineBoard = _board;
					for (auto& m : _bestLine)
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

		return _final;
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


	ChessEngine_Baby::ChessEngine_Baby()
	{};
};
