#include "api_glue.hpp"

#include "controller/controller_host.hpp"
#include "application/application.hpp"

#include "cpu_profiling.hpp"

#include <lambdex/chess/fen.hpp>


int main(int _nargs, char* _vargs[])
{
	using namespace lbx;

	// Echo arguements for now
	for (int n = 0; n != _nargs; ++n)
	{
		println("arg {} = {}", n, _vargs[n]);
	};

	// Path to this executable
	const fs::path _executablePath = _vargs[0];

	// Path to the executables folder
	const auto _rootPath = _executablePath.parent_path();

	// Set the application's root path
	chess::set_application_root_path(_rootPath);

	AccountAPI _accountAPI{};
	api::set_account_api(&_accountAPI);

	while (true)
	{
		api::forward_events();
		jc::sleep(0.1f);
	};

	return 0;
};
