#include "chess/chess.hpp"

#include "utility/json.hpp"
#include "utility/http.hpp"

#include "api/env.hpp"
#include "api/api.hpp"

#include <random>
#include <ranges>
#include <iterator>
#include <algorithm>
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
			const auto _myColor = _game.my_color();
			auto _board = chess::make_standard_board();

			_board.turn = _myColor;

			{
				auto _moves = _game.get_moves();
				for (auto& _move : _moves)
				{
					chess::apply_move(_board, _move);
				};
			};

			uint8_t a = 0;
			uint8_t b = 0;
			uint8_t c = 0;
			uint8_t d = 0;

			chess::Move _move{};
			std::vector<chess::Move> _allPossibleMoves{};

			while (true)
			{
				_move.from = chess::PositionPair{ chess::Rank{ a }, chess::File{ b } };
				_move.to = chess::PositionPair{ chess::Rank{ c }, chess::File{ d } };

				if (chess::is_move_valid(_board, _move, _myColor) == chess::MoveValidity::valid)
				{
					_allPossibleMoves.push_back(_move);
				};

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
			};

			// Shuffle possible moves
			std::random_device rd;
			std::mt19937 g(rd());
			std::ranges::shuffle(_allPossibleMoves, g);

			// Try to use the moves until we find a valid one
			bool _foundMove = false;
			for (auto& _move : _allPossibleMoves)
			{
				auto _moveStr = _move.to_string();
				if (_game.send_move(_moveStr))
				{
					_foundMove = true;
					break;
				};
			};

			if (!_foundMove)
			{
				std::cout << "no valid moves found!\n";
			};
		};
	};

	return 0;
};
