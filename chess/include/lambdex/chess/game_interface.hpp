#pragma once
#ifndef LAMBDEX_CHESS_GAME_API_HPP
#define LAMBDEX_CHESS_GAME_API_HPP

/*
	Defines the interface that chess engines can communicate
	through to play a game of chess.
*/

#include "move.hpp"
#include "board.hpp"

namespace lbx::chess
{
	/**
	 * @brief Interface for chess engines to interact with the game through
	*/
	class IGameInterface
	{
	public:

		/**
		 * @brief Resigns from the game
		*/
		virtual void resign() = 0;

		/**
		 * @brief Offers a draw
		 * @return True if the draw was accepted, false otherwise
		*/
		virtual bool offer_draw() = 0;

		/**
		 * @brief Submits a move for the player
		 * @return True if the move was valid, false otherwise
		*/
		virtual bool submit_move(Move _move) = 0;

		/**
		 * @brief Gets the current chess board
		 * @return Chess board with state
		*/
		virtual BoardWithState get_board() = 0;

		/**
		 * @brief Gets the color for the engine
		 * @return Color of the engine
		*/
		virtual Color get_color() = 0;

		/**
		 * @brief Optional method allowing the interface to provide the name of the game
		 *
		 * This is mostly for logging purposes.
		 *
		 * @return The name of the game, or an empty string if this is unimplemented (default behavior).
		*/
		virtual std::string get_game_name() { return std::string{}; };

	protected:

		// Disallow deletion through pointer to base
		~IGameInterface() = default;

	};
};

#endif // LAMBDEX_CHESS_GAME_API_HPP