#pragma once

#include "utility/json.hpp"

#include <string>
#include <string_view>

namespace lbx::api
{
	struct LichessVariant
	{
		std::string key;
		std::string name;
	};

	struct LichessOpponent
	{
		std::string id;
		std::string username;
		int rating;
	};

	struct LichessGame
	{
		std::string game_id;
		std::string full_id;

		std::string color;
		std::string last_move;

		LichessVariant variant;
		std::string speed;
		std::string perf;

		LichessOpponent opponent;

		bool rated;
		bool is_my_turn;
	};

	struct LichessChallenge
	{
		std::string id;
	};
};

// json conversion functions
#pragma region JSON_CONVERSIONS

namespace lbx::api
{
	void to_json(json& _json, const LichessVariant& _value);
	void from_json(const json& _json, LichessVariant& _value);

	void to_json(json& _json, const LichessOpponent& _value);
	void from_json(const json& _json, LichessOpponent& _value);

	void to_json(json& _json, const LichessGame& _value);
	void from_json(const json& _json, LichessGame& _value);

	void to_json(json& _json, const LichessChallenge& _value);
	void from_json(const json& _json, LichessChallenge& _value);
};

#pragma endregion JSON_CONVERSIONS