#include "lambdex/chess/board.hpp"

#include <vector>
#include <string>
#include <sstream>
#include <ostream>

namespace lbx::chess
{
	std::ostream& operator<<(std::ostream& _ostr, const PieceBoard& _board)
	{
		std::vector<std::string> _lines{};
		std::stringstream _sstr{};
		size_t n = 0;
		for (auto& p : _board)
		{
			_sstr << p;
			++n;
			if (n == 8)
			{
				n = 0;
				_lines.push_back(_sstr.str());
				_sstr.str({});
			};
		};
		for (auto& l : _lines | std::views::reverse)
		{
			_ostr << l << '\n';
		};
		return _ostr;
	};
};