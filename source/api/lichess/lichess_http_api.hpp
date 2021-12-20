#pragma once

/*
	Helper functions for interacting with the
	https://lichess.org api.

	See https://lichess.org/arg for docs.
*/

#include "utility/http.hpp"

#include <jclib/maybe.h>

#include <vector>
#include <string>
#include <chrono>
#include <variant>
#include <string_view>

namespace lbx::api::lichess
{
	/**
	 * @brief Tagged union for holding time control settings.
	*/
	struct TimeControl
	{
	public:

		/**
		 * @brief Time control for correspondence games.
		*/
		struct Correspondence
		{
			/**
			 * @brief Time limit for each player in days.
			 *
			 * Value should be in the range [1, 15]
			*/
			std::chrono::days limit{ 1 };
		};

		/**
		 * @brief Time control for standard clocked games.
		*/
		struct RealTime
		{
			/**
			 * @brief The clock limit for each player.
			*/
			std::chrono::seconds limit{ 0 };

			/**
			 * @brief The clock increment for each player.
			*/
			std::chrono::seconds increment{ 0 };
		};

		/**
		 * @brief Type for unlimited time games.
		*/
		struct Unlimited {};

		/**
		 * @brief Types of time control objects
		*/
		enum class Type
		{
			unlimited = 0,
			real_time = 1,
			correspondence = 2
		};

	private:

		/**
		 * @brief Variant type for holding the time control object.
		*/
		using variant_type = std::variant<Unlimited, RealTime, Correspondence>;

		/**
		 * @brief Creates the default time control object.
		 * @return Time control object.
		*/
		constexpr static auto default_time_control()
		{
			Correspondence _tc{};
			_tc.limit = std::chrono::days{ 1 };
			return _tc;
		};

	public:

		/**
		 * @brief Gets the type of time control this is.
		 * @return Time control type.
		*/
		Type type() const noexcept
		{
			return Type(this->vt_.index());
		};

		/**
		 * @brief Gets the time control object.
		 * @return Time control object.
		*/
		template <Type T>
		auto& get() JCLIB_NOEXCEPT
		{
			return std::get<static_cast<size_t>(T)>(this->vt_);
		};

		/**
		 * @brief Gets the time control object.
		 * @return Time control object.
		*/
		template <Type T>
		const auto& get() const JCLIB_NOEXCEPT
		{
			return std::get<static_cast<size_t>(T)>(this->vt_);
		};

		/**
		 * @brief Constructs the default time control.
		*/
		TimeControl() :
			vt_{ default_time_control() }
		{};

		/**
		 * @brief Constructs the time control variant.
		 * @param _tc Time control object
		*/
		template <typename T>
		JCLIB_REQUIRES((jc::cx_element_of<T, variant_type>))
			TimeControl(T _tc) :
			vt_{ _tc }
		{};

	private:

		/**
		 * @brief The variant for holding the time control object.
		*/
		variant_type vt_;

	};

	/**
	 * @brief Settings for a chess game
	*/
	struct GameSettings
	{
		/**
		 * @brief The time controls for the game.
		*/
		TimeControl time_control{};

		/**
		 * @brief The chess variant to use.
		*/
		std::string variant = "standard";

		/**
		 * @brief True if the match will affect the ratings of players.
		*/
		bool rated = false;

		/**
		 * @brief Constructs the game settings for standard bullet chess.
		 * @param _rated If the match is rated or not.
		 * @return Game settings.
		*/
		static GameSettings bullet_chess(bool _rated = false);

		/**
		 * @brief Constructs the default game settings.
		 * @param _rated If the match is rated or not.
		 * @return Game settings.
		*/
		static GameSettings default_settings(bool _rated = false);

	};
	




	// Returns an ok boolean, or an error string
	jc::maybe<bool, std::string> accept_challenge(http::Client& _client, std::string_view _challengeID);

	// Returns vector of game ID strings
	// https://lichess.org/api#operation/apiAccountPlaying
	std::vector<std::string> get_current_games(http::Client& _client);
	
	// Returns a true or an error string on failure
	// https://lichess.org/api#operation/boardGameMove
	jc::maybe<bool, std::string> send_move(http::Client& _client, std::string_view _gameID, std::string_view _move);


	/**
	 * @brief Challenges a user to a match.
	 *
	 * See https://lichess.org/api#operation/challengeCreate
	 *
	 * @param _client HTTP client to make request with.
	 * @param _username Name of the user to challenge.
	 * @param _settings Game settings for the challenge.
	 *
	 * @return True on success, error string otherwise.
	*/
	jc::maybe<bool, std::string> challenge_user(http::Client& _client, std::string_view _username, GameSettings _settings = GameSettings::default_settings());

	/**
	 * @brief Challenges the stockfish AI to a game.
	 * 
	 * See https://lichess.org/api#operation/challengeAi
	 * 
	 * @param _level Stockfish level.
	 * @param _settings Game settings for the challenge.
	 * 
	 * @return True on success, error string otherwise.
	*/
	jc::maybe<bool, std::string> challenge_ai(http::Client& _client, int _level, GameSettings _settings = GameSettings::default_settings());


	// Resigns from the game
	// https://lichess.org/api#operation/botGameResign
	jc::maybe<bool, std::string> resign_game(http::Client& _client, std::string_view _gameID);


};
