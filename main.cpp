#include "utility/json.hpp"
#include "utility/http.hpp"

#include "api/env.hpp"
#include "api/api.hpp"

#include <iostream>

int main()
{
	using namespace lbx;

	api::set_env_folder_path(SOURCE_ROOT "/env");
	if (!api::load_env())
	{
		return 1;
	};

	auto _client = api::new_lichess_client();
	auto _games = api::open_active_games(*_client);

	

	for (auto& _game : _games)
	{
		if (_game.is_my_turn())
		{
			uint8_t a = 0;
			uint8_t b = 0;
			uint8_t c = 0;
			uint8_t d = 0;

			std::string _move = "a1a1";
			do
			{
				_move[0] = 'a' + a;
				_move[1] = '1' + b;
				_move[2] = 'a' + c;
				_move[3] = '1' + d;

				++a;
				if (a == 8)
				{
					a = 0;
					++b;
					if (b == 8)
					{
						++c;
						b = 0;
						if (c == 8)
						{
							++d;
							c = 0;
							if (d == 8)
							{
								break;
							};
						};
					};
				};
			}
			while(!_game.send_move(_move));
		};
	};

	return 0;
};
