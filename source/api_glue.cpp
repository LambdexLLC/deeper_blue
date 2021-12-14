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
	 * @brief Challenges a user on lichess to a chess match.
	 * @param _username The name of the user to challenge.
	*/
	ControllerAPI::Result ControllerAPI::challenge_lichess_user(const std::string& _username)
	{
		Result _result{};
		_result.content = json::object();

		if (this->account_->challenge_user(_username))
		{
			_result.status = 200;
			_result.content["result"] = "good challenge";
		}
		else
		{
			_result.status = 400;
			_result.content["error"] = "something failed i guess lol";
		};

		return _result;
	};

	/**
	 * @brief Challenges a bot on lichess to a chess match.
	 * @param _level Stockfish level to challenge.
	*/
	ControllerAPI::Result ControllerAPI::challenge_lichess_bot(int _level)
	{
		Result _result{};
		_result.content = json::object();

		if (this->account_->challenge_ai(_level))
		{
			_result.status = 200;
			_result.content["result"] = "good challenge";
		}
		else
		{
			_result.status = 400;
			_result.content["error"] = "something failed i guess lol";
		};
		
		return _result;
	};



	/**
	 * @brief Constructs the controller API referencing the account API for callbacks.
	*/
	ControllerAPI::ControllerAPI(jc::reference_ptr<AccountAPI> _account) :
		account_{ _account }
	{};

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
		this->assign_to_game(_gameID, jc::make_unique<chess::ChessEngine_Baby>(this->tree_build_pool_));
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


	AccountAPI::AccountAPI() :
		controller_
		{
			jc::make_unique<ControllerAPI>(jc::reference_ptr{ *this }),
			"localhost", 42069
		}
	{
		// Nothing for now
	};
};
