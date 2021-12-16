#include <lambdex/chess/board/bit_board.hpp>

#include <ostream>

namespace lbx::chess
{
	/**
	 * @brief Writes a bit board to an output stream as a series of rows containing 1s and 0s
	 * @param _ostr Output stream
	 * @param _board Bit board to write
	 * @return The output stream
	*/
	std::ostream& operator<<(std::ostream& _ostr, const BitBoard& _board)
	{
		auto _rank = Rank::END;
		for (int n = 0; n != 8; ++n)
		{
			--_rank;
			for (auto _file = File::a; _file != File::END; ++_file)
			{
				if (_board[(_file, _rank)])
				{
					_ostr << '1';
				}
				else
				{
					_ostr << '0';
				};
			};

			if (n != 7)
			{
				_ostr << '\n';
			};
		};

		return _ostr;
	};
};