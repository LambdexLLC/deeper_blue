#pragma once

#include "chess/move_tree.hpp"

#include "chess/chess_engine.hpp"
#include "chess/engines/random_engine.hpp"

namespace lbx::chess
{
	/**
	 * @brief Babys first bot that isnt random
	*/
	class ChessEngine_Baby : public IChessEngine
	{
	private:

		std::vector<RatedMove> rank_possible_moves(const BoardWithState& _board, Color _player);
		
		void calculate_move_tree_node_responses(MoveTree::Node* _previous, size_t _depth);
		void calculate_move_tree_node_responses(MoveTree::Node* _previous);

		MoveTree make_move_tree(const BoardWithState& _board, size_t _depth);



		using RatedLine = std::vector<RatedMove>;
		std::vector<RatedLine> pick_best_from_tree(const MoveTree& _tree);


	public:

		bool is_stateless() const final { return false; };

		Move calculate_move(const BoardWithState& _board, Color _player) final
		{
			return this->calculate_multiple_moves(_board, _player).front();
		};

		std::vector<Move> calculate_multiple_moves(const BoardWithState& _board, Color _player) final;

	private:

		/**
		 * @brief Have the random chess engine around as a fallback
		*/
		ChessEngine_Random random_fallback_{};

	};
};