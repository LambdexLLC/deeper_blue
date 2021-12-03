#pragma once

/*
	Contains all of the functionality needed to interact with the remote lichess chess API
*/

#include <jclib/memory.h>

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

};
