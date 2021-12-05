#pragma once

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

		/**
		 * @brief Gets the "material" value of a piece
		 * @return Value
		*/
		int get_piece_value(Piece _piece) const;

		/**
		 * @brief Gets the total piece value of a player's existing pieces
		 * @param _board Board to get pieces from
		 * @param _player Player to get value of
		 * @return Total value
		*/
		int get_player_material(const Board& _board, Color _player) const;


		struct RankedMove
		{
			Board board;
			int value;
			Move move;
		};
		std::vector<RankedMove> find_best_moves(const Board& _board, Color _player);

	public:

		bool is_stateless() const final { return false; };

		Move calculate_move(const Board& _board, Color _player) final
		{
			return this->calculate_multiple_moves(_board, _player).front();
		};

		std::vector<Move> calculate_multiple_moves(const Board& _board, Color _player) final;

	private:

		/**
		 * @brief Have the random chess engine around as a fallback
		*/
		ChessEngine_Random random_fallback_{};

	};
};