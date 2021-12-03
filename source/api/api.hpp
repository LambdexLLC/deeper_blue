#pragma once

/*
	Contains all of the functionality needed to interact with the remote lichess chess API
*/

#include <jclib/memory.h>

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


	void test(LichessClient& _client);
};
