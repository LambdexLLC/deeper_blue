#pragma once

/*
	Contains all of the functionality needed to interact with the remote lichess chess API
*/

#include "chess/move.hpp"

#include <jclib/memory.h>

#include <vector>
#include <string_view>

namespace lbx::api
{
	/**
	 * @brief Client data storage
	*/
	struct LichessClient;

	/**
	 * @brief Deleter function object type for the lichess client type
	*/
	struct LichessClientDeleter
	{
		/**
		 * @brief Destroys and the frees a lichess client
		 * @param _client Client to delete
		*/
		void operator()(LichessClient* _client);
	};

	/**
	 * @brief RAII owning handle to a lichess client object
	*/
	using LichessClientHandle = std::unique_ptr<LichessClient, LichessClientDeleter>;

	/**
	 * @brief Creates a new lichess client object
	 * @return Owning handle to a new lichess client
	*/
	[[nodiscard]] LichessClientHandle new_lichess_client();

	/**
	 * @brief Follows another player
	 * @param _client Lichess client object
	 * @param _playerName Name of the player to follow
	 * @return True on good follow, false otherwise
	*/
	bool follow_player(LichessClient& _client, std::string_view _playerName);



	// Forward decl for game stream
	struct LichessGame;
	struct LichessGameStateFull;
	


	/**
	 * @brief Provides an interface for an open chess game
	*/
	class GameStream
	{
	private:
		
		struct GameStateDeleter
		{
			void operator()(LichessGameStateFull* _ptr);
		};
		using GameStateHandle = std::unique_ptr<LichessGameStateFull, GameStateDeleter>;

	public:

		bool is_my_turn() const
		{
			return this->is_my_turn_;
		};

		bool send_move(std::string_view _move);

		std::vector<chess::Move> get_moves() const;

		/**
		 * @brief Gets our piece color
		 * @return Color
		*/
		chess::Color my_color() const;


		explicit GameStream(LichessClient& _client, std::string_view _gameID, bool _isMyTurn, chess::Color _myColor);
		explicit GameStream(LichessClient& _client, const LichessGame& _game);

	private:
		LichessClient* client_{};
		GameStateHandle state_{};
		chess::Color my_color_{};
		bool is_my_turn_ = false;
	};



	std::vector<GameStream> open_active_games(LichessClient& _client);


};
