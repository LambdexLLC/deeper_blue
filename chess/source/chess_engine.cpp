#include "lambdex/chess/chess_engine.hpp"
#include "lambdex/chess/chess.hpp"

#include <jclib/memory.h>

namespace lbx::chess
{
	namespace
	{
		/**
		 * @brief State shared between game instances for a game run in the local running program
		*/
		struct LocalGameState
		{
			/**
			 * @brief The current chess board state
			*/
			BoardWithState board;

			/**
			 * @brief The most recent move played, rated
			*/
			RatedMove last_move;

			/**
			 * @brief If an illegal move was played, this will contain which player made the illegal move
			*/
			std::optional<Color> illegal_move_played{ std::nullopt };

			/**
			 * @brief If a player resigned, this will contain which player is resigning
			*/
			std::optional<Color> player_resigned{ std::nullopt };
		};

		/**
		 * @brief Interface for a game run within the local running program
		*/
		class LocalGameInterface : public IGameInterface
		{
		public:

			/**
			 * @brief Resigns from the game
			*/
			void resign() final
			{
				this->state_->player_resigned = this->get_color();
			};

			/**
			 * @brief Offers a draw
			 * @return True if the draw was accepted, false otherwise
			*/
			bool offer_draw() final
			{
				// TODO: Interface for engines to accept/decline a draw
				return false;
			};

			/**
			 * @brief Submits a move for the player
			 * @return Always true, an invalid move causes the engine to lose
			*/
			bool submit_move(Move _move) final
			{
				const auto _myColor = this->get_color();
				auto& _board = this->state_->board;

				// Check that it is actually our turn
				JCLIB_ASSERT(_board.turn == _myColor);

				// Check for valid move
				if (chess::is_move_valid(_board, _move, _myColor) != MoveValidity::valid)
				{
					// Engine played an illegal move !
					this->state_->illegal_move_played = _myColor;
				}
				else
				{
					// Update last move
					this->state_->last_move = rate_move(_board, _move);
					
					// Apply move
					chess::apply_move(_board, _move);
				};
				
				return true;
			};

			/**
			 * @brief Gets the current chess board
			 * @return Chess board with state
			*/
			BoardWithState get_board() final
			{
				return this->state_->board;
			};

			/**
			 * @brief Gets the color for the engine
			 * @return Color of the engine
			*/
			Color get_color() final
			{
				return this->my_color_;
			};

			// Constructs the interface for an engine playing a local game
			LocalGameInterface(jc::reference_ptr<LocalGameState> _state, Color _myColor) :
				state_{ _state }, my_color_{ _myColor }
			{};

		private:

			/**
			 * @brief Shared game state pointer
			*/
			jc::reference_ptr<LocalGameState> state_;
			
			/**
			 * @brief Color that the engine using this interface will play as
			*/
			Color my_color_;
		};
	};

	/**
	 * @brief Plays through a standard chess match between two engines
	 * @param _white White player engine
	 * @param _black Black player engine
	 * @param _stats Optional match stats output parameter, defaults to nullptr
	 * @return Match verdict
	*/
	MatchVerdict play_standard_match(IChessEngine& _white, IChessEngine& _black, MatchStats* _stats)
	{
		// Create a new game state with standard board
		LocalGameState _state{};
		_state.board = make_standard_board();

		// Create our interfaces
		LocalGameInterface _whiteInterface{ _state, Color::white };
		LocalGameInterface _blackInterface{ _state, Color::black };

		// Play the game until finished
		while (true)
		{
			// Play white move
			_white.play_turn(_whiteInterface);
			
			// Check for flags set
			if (_state.illegal_move_played)
			{
				// Lost due to illegal move
				return MatchVerdict{ MatchVerdict::played_illegal_move, Color::black };
			}
			else if (_state.player_resigned)
			{
				// Lost due to resigning
				return MatchVerdict{ MatchVerdict::resigned, Color::black };
			}
			else if (_stats)
			{
				// Handle stats
				_stats->moves_white.push_back(_state.last_move);
			};

			// Play black move
			_black.play_turn(_blackInterface);

			// Check for flags set
			if (_state.illegal_move_played)
			{
				// Lost due to illegal move
				return MatchVerdict{ MatchVerdict::played_illegal_move, Color::white };
			}
			else if (_state.player_resigned)
			{
				// Lost due to resigning
				return MatchVerdict{ MatchVerdict::resigned, Color::white };
			}
			else if (_stats)
			{
				// Handle stats
				_stats->moves_black.push_back(_state.last_move);
			};
		};

		// Return a draw as default outcome
		JCLIB_ASSERT(false);
		return MatchVerdict{ MatchVerdict::draw };
	};
};