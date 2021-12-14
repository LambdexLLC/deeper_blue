#include "api_glue.hpp"


namespace lbx
{
	// Called when the object is initialized but no games are being played
	void AccountAPI::on_no_current_games()
	{
		this->challenge_ai(3);
	};
};


namespace lbx
{
	/**
	 * @brief Assigns a chess engine to a game
	 * @param _gameID ID of the game to assign the engine to
	 * @param _engine Engine to assign to the game
	*/
	void AccountAPI::assign_to_game(const std::string& _gameID, std::unique_ptr<chess::IChessEngine> _engine)
	{
		// Create the API to manage the engine
		auto _gameAPI = jc::make_unique<GameAPI>(_gameID, std::move(_engine));
		
		// Assign the API to the game
		api::set_game_api(_gameID, _gameAPI.get());
		
		// Add the API to our container
		this->games_.push_back(std::move(_gameAPI));

		// Log game link
		println("Assigned engine to game https://lichess.org/{}", _gameID);
	};

	/**
	 * @brief Invoked when a player challenges you
	*/
	void AccountAPI::on_challenge(const lbx::json& _event)
	{
		const std::string_view _challengeID = _event.at("challenge").at("id");

		// Of course we accept it
		this->accept_challenge(_challengeID);
	};

	/**
	 * @brief Invoked when a game is started
	*/
	void AccountAPI::on_game_start(const lbx::json& _event)
	{
		// Get the ID of the game
		const std::string _gameID = _event.at("game").at("id");

		// Assign a new engine to the game
		this->assign_to_game(_gameID, jc::make_unique<chess::ChessEngine_Baby>((size_t)4));
	};

	/**
	 * @brief Invoked when a game finishes
	*/
	void AccountAPI::on_game_finish(const lbx::json& _event)
	{
		auto _dmp = _event.dump(1, '\t');
		println("game finished \n{}", _dmp);

		auto _games = this->get_current_games();
		if (_games.empty())
		{
			//this->on_no_current_games();
		};
	};

	AccountAPI::AccountAPI()
	{
		// Create a game API for each of the current games
		const auto _games = this->get_current_games();
		for (auto& _game : _games)
		{
			this->assign_to_game(_game, jc::make_unique<chess::ChessEngine_Baby>((size_t)4));
		};

		if (_games.empty())
		{
			this->on_no_current_games();
		};
	};
};
