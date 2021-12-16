#pragma once
#ifndef LAMBDEX_CHESS_CHESS_ENGINE_HPP
#define LAMBDEX_CHESS_CHESS_ENGINE_HPP

/*
	Defines the interface for a chess engine or processor - think of it as
	the interface for individual chess AI implementations.

	Engine implementations should be places in the source directory at path

			"source/chess/engines/*"

*/

#include "board/board_with_state.hpp"

#include "move_tree.hpp"
#include "game_interface.hpp"

#include <string>
#include <vector>

namespace lbx::chess
{
	/**
	 * @brief Interface for chess engine implementations
	*/
	class IChessEngine
	{
	public:

		/**
		 * @brief Implement this method as the main engine "thinking" routine. Engine implementations
		 * are expected to take in a board and return the best possible move (in their humble opinion).
		 *
		 * It can be assumed that this method will only ever be called when it is the engine's turn, so
		 * do not worry about checking for turn order.
		 *
		 * @param _game Interface to the current game
		*/
		virtual void play_turn(IGameInterface& _game) = 0;

		// Allow polymorphic destruction
		virtual ~IChessEngine() = default;
	};


	/**
	 * @brief Data struture for holding the result of a chess match
	*/
	struct MatchVerdict
	{
	public:

		/**
		 * @brief Possible reasons why a player won or lost a match
		*/
		enum class Reason
		{
			/**
			 * @brief Player lost due to checkmate
			*/
			checkmate,

			/**
			 * @brief Player ran out of time
			*/
			time,

			/**
			 * @brief Player resigned from the game
			*/
			resigned,

			/**
			 * @brief Player had no legal moves to make
			*/
			stalemate,

			/**
			 * @brief Players agreed to a draw
			*/
			draw,

			/**
			 * @brief Player played an illegal move and was disqualifed
			*/
			played_illegal_move,

			/**
			 * @brief The maximum number of moves that could be played for the match were played.
			*/
			max_moves_reached,
		};

		// Allow more natural access to the enum
		using enum Reason;

		/**
		 * @brief Gets the reason why the game was won/lost
		 * @return Reason for the outcome
		*/
		constexpr Reason reason() const noexcept
		{
			return this->reason_;
		};

		/**
		 * @brief Checks if the match was a draw or not
		 * @return True if draw, false otherwise
		*/
		constexpr bool is_draw() const noexcept
		{
			return	this->reason() == Reason::draw ||
					this->reason() == Reason::stalemate ||
					this->reason() == Reason::max_moves_reached;
		};

		/**
		 * @brief Gets the winning player's color
		 * 
		 * DO NOT CALL THIS IF ITS A DRAW!!!
		 * 
		 * @return Winning player's color
		*/
		constexpr Color winner() const noexcept
		{
			JCLIB_ASSERT(!this->is_draw());
			return this->winner_;
		};

		/**
		 * @brief Creates a drawn match verdict
		 * @param _reason Why the match was a draw
		*/
		constexpr explicit MatchVerdict(Reason _reason) :
			winner_{}, reason_{ _reason }
		{};

		/**
		 * @brief Creates a winning/losing match verdict
		 * @param _reason Reason for the win/loss
		 * @param _winner Which player won the match
		*/
		constexpr explicit MatchVerdict(Reason _reason, Color _winner) noexcept :
			winner_{ _winner }, reason_{ _reason }
		{};

	private:

		/**
		 * @brief Which player won the match if this wasnt a draw
		*/
		Color winner_;

		/**
		 * @brief Reason why the game was won/lost if it wasn't a draw
		*/
		Reason reason_;
	};

	/**
	 * @brief Structure for storing statistics about a chess match
	*/
	struct MatchStats
	{
		/**
		 * @brief The series of moves black played in the match, rated
		*/
		std::vector<RatedMove> moves_black;

		/**
		 * @brief The series of moves white played in the match, rated
		*/
		std::vector<RatedMove> moves_white;
	};

	/**
	 * @brief Options used to define how a chess match can be played
	*/
	struct MatchOptions
	{
		/**
		 * @brief Creates the default match options
		 * 
		 * @param _gameName optional name for the game, defaults to "local_game"
		 * @return Match options
		*/
		static MatchOptions make_default(const std::string_view _gameName = "local_game")
		{
			MatchOptions _out{};
			_out.initial_board = make_standard_board();
			_out.game_name = std::string{ _gameName };
			_out.max_moves_per_player = 0;
			return _out;
		};

		/**
		 * @brief Initial board state
		*/
		BoardWithState initial_board;


		std::string game_name;

		/**
		 * @brief Defines the max moves to play for each player.
		 * 
		 * Set this to 0 for no max value.
		*/
		uint32_t max_moves_per_player;
	};
	
	/**
	 * @brief Plays through a standard chess match between two engines with default match options.
	 * 
	 * @param _white White player engine
	 * @param _black Black player engine
	 * @param _stats Optional match stats output parameter, defaults to nullptr
	 * @return Match verdict
	*/
	MatchVerdict play_standard_match(IChessEngine& _white, IChessEngine& _black, MatchStats* _stats = nullptr);

	/**
	 * @brief Plays through a standard chess match between two engines.
	 * 
	 * @param _white White player engine
	 * @param _black Black player engine
	 * @param _options Match options object
	 * @param _stats Optional match stats output parameter, defaults to nullptr
	 * @return Match verdict
	*/
	MatchVerdict play_standard_match(IChessEngine& _white, IChessEngine& _black, MatchOptions _options, MatchStats* _stats = nullptr);

};

#endif // LAMBDEX_CHESS_CHESS_ENGINE_HPP