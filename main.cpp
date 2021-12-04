#include "chess/chess.hpp"

#include "utility/json.hpp"
#include "utility/http.hpp"

#include "api/env.hpp"
#include "api/api.hpp"

#include <jclib/thread.h>
#include <jclib/timer.h>

#include <random>
#include <ranges>
#include <iterator>
#include <algorithm>
#include <iostream>


struct AccountAPI final : public lbx::api::LichessAccountAPI
{
public:
	/**
	 * @brief Invoked when a player challenges you
	*/
	void on_challenge(const lbx::json& _event) final
	{
		const std::string_view _challengeID = _event.at("challenge").at("id");

		// Of course we accept it
		this->accept_challenge(_challengeID);
	};
};

int main()
{
	using namespace lbx;

	api::set_env_folder_path(SOURCE_ROOT "/env");
	if (!api::load_env())
	{
		return 1;
	};

	AccountAPI _accountAPI{};
	api::set_account_api(&_accountAPI);
	
	while (true)
	{
		api::forward_events();
		jc::sleep(0.1f);
	};

	return 0;
};
