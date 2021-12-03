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
};

#pragma endregion JSON_CONVERSIONS