#include <lambdex/chess/chess.hpp>
#include <lambdex/chess/piece_movement.hpp>

#include <jclib-test.hpp>

using namespace lbx::chess;

inline std::ostream& operator<<(std::ostream& ostr, const BitBoard& _board)
{
	auto _rank = Rank::END;
	for (int n = 0; n != 8; ++n)
	{
		--_rank;
		for (auto _file = File::a; _file != File::END; ++_file)
		{
			if (_board[(_file, _rank)])
			{
				ostr << '1';
			}
			else
			{
				ostr << '0';
			};
		};
		ostr << '\n';
	};
	return ostr;
};

int subtest_move()
{
	NEWTEST();

	PASS();
};

int main()
{
	NEWTEST();
	SUBTEST(subtest_move);
	PASS();
};
