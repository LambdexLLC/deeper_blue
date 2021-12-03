#include "utility/json.hpp"
#include "utility/http.hpp"

#include "api/env.hpp"

#include <iostream>

int main()
{
	using namespace lbx;

	api::set_env_folder_path(SOURCE_ROOT "/env");
	if (!api::load_env())
	{
		return 1;
	};

	return 0;
};
