#include "api.hpp"

#include "client/client.hpp"

#include <jclib/ranges.h>
#include <jclib/algorithm.h>

#include <ranges>
#include <algorithm>

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



	std::vector<GameStream> open_active_games(LichessClient& _client)
	{
		auto _challenges = _client.list_challenges();
		for (auto& c : _challenges)
		{
			_client.accept_challenge(c.id);
		};

		std::vector<GameStream> _out{};
		auto _games = _client.list_games();
		for (auto& _game : _games)
		{
			_out.push_back(GameStream{ _client, _game });
		};
		return _out;
	};



	void GameStream::GameStateDeleter::operator()(LichessGameStateFull* _ptr)
	{
		delete _ptr;
	};


	bool GameStream::send_move(std::string_view _move)
	{
		std::string _errmsg{};
		if (this->client_->send_move(this->state_->game_id, _move, &_errmsg))
		{
			this->state_->state.moves.push_back(' ');
			this->state_->state.moves.append(_move);
			this->is_my_turn_ = false;
			return true;
		}
		else
		{
			std::cout << "Failed to send move : " << _errmsg << '\n';
			return false;
		};
	};

	std::vector<chess::Move> GameStream::get_moves() const
	{
		auto& _moveString = this->state_->state.moves;
		std::vector<chess::Move> _out{};
		char _buffer[5]{};
		for (auto v : _moveString | std::views::split(' '))
		{
			chess::Move _move{};
			std::ranges::copy(v, _buffer);
			const auto _result = chess::from_chars(_buffer, _buffer + 5, _move);
			JCLIB_ASSERT(_result.ec == std::errc{});
			_out.push_back(_move);
		};
		return _out;
	};

	/**
	 * @brief Gets our piece color
	 * @return Color
	*/
	chess::Color GameStream::my_color() const
	{
		return this->my_color_;
	};

	std::string GameStream::id() const
	{
		return this->state_->game_id;
	};


	GameStream::GameStream(LichessClient& _client, std::string_view _gameID, bool _isMyTurn, chess::Color _myColor) :
		client_{ &_client },
		state_{ new LichessGameStateFull{} },
		my_color_{ _myColor },
		is_my_turn_{ _isMyTurn }
	{
		_client.get_game_state(_gameID, *this->state_);
	};
	GameStream::GameStream(LichessClient& _client, const LichessGame& _game) :
		client_{ &_client },
		state_{ new LichessGameStateFull{} },
		my_color_{},
		is_my_turn_{ _game.is_my_turn }
	{
		_client.get_game_state(_game.game_id, *this->state_);
		if (_game.color == "white")
		{
			this->my_color_ = chess::Color::white;
		}
		else if (_game.color == "black")
		{
			this->my_color_ = chess::Color::black;
		};
	};

};