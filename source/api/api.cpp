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


	/**
	 * @brief Follows another player
	 * @param _client Lichess client object
	 * @param _playerName Name of the player to follow
	 * @return True on good follow, false otherwise
	*/
	bool follow_player(LichessClient& _client, std::string_view _playerName)
	{
		return _client.follow_player(_playerName);
	};

	void test(LichessClient& _client)
	{
		{
			auto _challenges = _client.list_challenges();
			for (auto& c : _challenges)
			{
				_client.accept_challenge(c);
			};
		};

		{
			auto _games = _client.list_games();
			for (auto& g : _games)
			{
				LichessGameStateFull _state{};
				_client.get_game_state(g.game_id, _state);
				std::cout << _state.black.name << '\n';
			};
		};
	};
};