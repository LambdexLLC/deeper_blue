#include "api_glue.hpp"

#include <lambdex/chess/fen.hpp>

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
