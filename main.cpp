#include "chess/chess.hpp"

#include "utility/json.hpp"
#include "utility/http.hpp"

#include "api/env.hpp"
#include "api/api.hpp"

#include <jclib/thread.h>

#include <random>
#include <ranges>
#include <iterator>
#include <algorithm>
#include <iostream>

int main()
{
	using namespace lbx;

	{
		chess::Move _testMove{};
		chess::from_chars("d3a2", _testMove);
		JCLIB_ASSERT(_testMove.from == (chess::Rank::r3, chess::File::d));
		JCLIB_ASSERT(_testMove.to == (chess::Rank::r2, chess::File::a));
	};

	api::set_env_folder_path(SOURCE_ROOT "/env");
	if (!api::load_env())
	{
		return 1;
	};
	
	while (true)
	{
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
					bool _colorToggle = false;
					for (auto& _move : _moves)
					{
						chess::apply_move(_board, _move, chess::Color(_colorToggle));
						_colorToggle = !_colorToggle;
					};
				};

				{
					auto _str = chess::stringify_board(_board);
					_str.insert(_str.begin() + 64, '\n');
					_str.insert(_str.begin() + 56, '\n');
					_str.insert(_str.begin() + 48, '\n');
					_str.insert(_str.begin() + 40, '\n');
					_str.insert(_str.begin() + 32, '\n');
					_str.insert(_str.begin() + 24, '\n');
					_str.insert(_str.begin() + 16, '\n');
					_str.insert(_str.begin() + 8, '\n');
					std::cout << _str << '\n';
				};


				chess::Position _fromPos{};
				chess::Position _toPos{};

				chess::Move _move{};
				std::vector<chess::Move> _allPossibleMoves{};

				while (true)
				{
					_move.from = _fromPos;
					_move.to = _toPos;

					if (_board.at(_move.from) != chess::Piece::empty &&
						chess::get_color(_board.at(_move.from)) == _myColor)
					{
						if (_move.from == (chess::Rank::r2, chess::File::a))
						{
							if (_move.to == (chess::Rank::r3, chess::File::a))
							{
								_move.promotion = chess::Piece::empty;
							};
						};
						if (chess::is_move_valid(_board, _move, _myColor) == chess::MoveValidity::valid)
						{
							_allPossibleMoves.push_back(_move);
						};
					};
					
					_fromPos = _fromPos + 1;
					if (_fromPos.get() == 64)
					{
						_fromPos = chess::Position{ 0 };
						_toPos = _toPos + 1;
						if (_toPos == chess::Position{ 64 })
						{
							break;
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

		jc::sleep(0.25);
	};

	return 0;
};
