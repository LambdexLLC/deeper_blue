#include "api_glue.hpp"


namespace lbx
{
	// Called when the object is initialized but no games are being played
	void AccountAPI::on_no_current_games()
	{
		this->challenge_ai(2);
	};
};


namespace lbx
{
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
		const std::string _gameID = _event.at("game").at("id");
#if true
		this->games_.push_back(jc::make_unique<GameAPI>(std::shared_ptr<chess::IChessEngine>
			(
				new chess::ChessEngine_Baby{}
		)));
#else
		this->games_.push_back(jc::make_unique<Neuron_GameAPI>());
#endif
		api::set_game_api(_gameID, this->games_.back().get());
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
#if true
			this->games_.push_back(jc::make_unique<GameAPI>(std::shared_ptr<chess::IChessEngine>
				(
					new chess::ChessEngine_Baby{}
			)));
#else
			this->games_.push_back(jc::make_unique<Neuron_GameAPI>());
#endif

			api::set_game_api(_game, this->games_.back().get());
		};

		if (_games.empty())
		{
			this->on_no_current_games();
		};
	};

};
