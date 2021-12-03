#include "api.hpp"

#include "client/client.hpp"

namespace lbx::api
{
	namespace
	{
		/**
		 * @brief URL for lichess
		*/
		constexpr inline auto lichess_url_v = "https://lichess.org";
	};

	/**
	 * @brief Destroys and the frees a lichess client
	 * @param _client Client to delete
	*/
	void LichessClientDeleter::operator()(LichessClient* _client)
	{
		delete _client;
	};

	/**
	 * @brief Creates a new lichess client object
	 * @return Owning handle to a new lichess client
	*/
	[[nodiscard]] LichessClientHandle new_lichess_client()
	{
		// Construct client
		LichessClientHandle _client{ new LichessClient{ lichess_url_v } };

		// Close client early if it is not considered "good"
		if (!_client->good())
		{
			_client.reset();
		};

		return _client;
	};
};