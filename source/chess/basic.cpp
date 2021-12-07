#include "basic.hpp"

#include <ostream>

namespace lbx::chess
{
	/**
	 * @brief Writes a piece to an output stream.
	 *
	 * Converts the piece into its UTF-8 codepoint and writes it, if the piece is empty then
	 * a single space is written to the output stream
	 *
	 * @param _ostr Output stream to write to
	 * @param _piece Piece to write to the stream
	 *
	 * @return Output stream
	*/
	std::ostream& operator<<(std::ostream& _ostr, const Piece& _piece)
	{
		if (_piece == Piece::empty)
		{
			_ostr.put(0xe2);
			_ostr.put(0x99);
			_ostr.put(0xa4);
		}
		else
		{
			const auto _utfStr = to_utf8(_piece);
			_ostr.put(_utfStr[0]);
			_ostr.put(_utfStr[1]);
			_ostr.put(_utfStr[2]);
		};
		return _ostr;
	};
};