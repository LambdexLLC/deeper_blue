#include "lichess.hpp"

#pragma region JSON_CONVERSIONS

namespace lbx::api
{
	void to_json(json& _json, const LichessVariant& _value)
	{
		_json = json{ {"key", _value.key}, {"name", _value.name} };
	};
	void from_json(const json& _json, LichessVariant& _value)
	{
		_json.at("key").get_to(_value.key);
		_json.at("name").get_to(_value.name);
	};

	void to_json(json& _json, const LichessOpponent& _value)
	{
		_json = json{ {"id", _value.id}, {"rating", _value.rating} , {"username", _value.username} };
	};
	void from_json(const json& _json, LichessOpponent& _value)
	{
		_json.at("id").get_to(_value.id);
		_json.at("rating").get_to(_value.rating);
		_json.at("username").get_to(_value.username);
	};

	void from_json(const json& _json, LichessGame& _value)
	{
		_json.at("gameId").get_to(_value.game_id);
		_json.at("fullId").get_to(_value.full_id);
		_json.at("lastMove").get_to(_value.last_move);
		_json.at("color").get_to(_value.color);
		_json.at("speed").get_to(_value.speed);
		_json.at("perf").get_to(_value.perf);
		_json.at("rated").get_to(_value.rated);
		_json.at("variant").get_to(_value.variant);
		_json.at("opponent").get_to(_value.opponent);
		_json.at("isMyTurn").get_to(_value.is_my_turn);
	};

	void from_json(const json& _json, LichessChallenge& _value)
	{
		_json.at("id").get_to(_value.id);
	};

	void from_json(const json& _json, LichessGameState& _value)
	{
		_json.at("moves").get_to(_value.moves);
		_json.at("wtime").get_to(_value.wtime);
		_json.at("btime").get_to(_value.btime);
		_json.at("winc").get_to(_value.winc);
		_json.at("binc").get_to(_value.binc);
		_json.at("status").get_to(_value.status);
	};

	void from_json(const json& _json, LichessGamePlayer& _value)
	{
		_json.at("id").get_to(_value.id);
		_json.at("name").get_to(_value.name);
		_json.at("provisional").get_to(_value.provisional);
		_json.at("rating").get_to(_value.rating);
		if (_json.at("title").is_string())
		{
			_json.at("title").get_to(_value.title);
		}
		else
		{
			_value.title = std::string{};
		};
	};

	void from_json(const json& _json, LichessGameStateFull& _value)
	{
		_json.at("id").get_to(_value.game_id);
		_json.at("rated").get_to(_value.rated);
		_json.at("variant").get_to(_value.variant);

		if (const auto& _clock = _json.at("clock"); _clock.is_object())
		{
			_clock.at("initial").get_to(_value.clock.initial);
			_clock.at("increment").get_to(_value.clock.increment);
		};

		_json.at("speed").get_to(_value.speed);
		_json.at("perf").at("name").get_to(_value.perf_name);
		_json.at("createdAt").get_to(_value.created_at);

		_json.at("white").get_to(_value.white);
		_json.at("black").get_to(_value.black);

		_json.at("initialFen").get_to(_value.initial_fen);
		_json.at("state").get_to(_value.state);
	};
};

#pragma endregion JSON_CONVERSIONS