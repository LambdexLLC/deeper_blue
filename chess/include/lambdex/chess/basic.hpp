#pragma once
#ifndef LAMBDEX_CHESS_BASIC_HPP
#define LAMBDEX_CHESS_BASIC_HPP

#include <jclib/concepts.h>
#include <jclib/type_traits.h>

#include <iosfwd>
#include <format>
#include <string>
#include <cstdint>
#include <compare>
#include <charconv>
#include <string_view>

namespace lbx::chess
{
	using Square = uint8_t;

	/**
	 * @brief Named board rank values (row)
	*/
	enum class Rank : uint8_t
	{
		r1,
		r2,
		r3,
		r4,
		r5,
		r6,
		r7,
		r8,
		END
	};

	constexpr inline auto operator<=>(Rank lhs, Rank rhs) noexcept
	{
		return jc::to_underlying(lhs) <=> jc::to_underlying(rhs);
	};

	constexpr inline Rank operator+(Rank lhs, jc::cx_integer auto rhs) noexcept
	{
		const auto _out = Rank(jc::to_underlying(lhs) + rhs);
		JCLIB_ASSERT(_out >= Rank::r1 && _out <= Rank::END);
		return _out;
	};
	constexpr inline Rank& operator+=(Rank& lhs, jc::cx_integer auto rhs) noexcept
	{
		lhs = lhs + rhs;
		return lhs;
	};

	constexpr inline Rank& operator++(Rank& rhs) noexcept
	{
		JCLIB_ASSERT(rhs != Rank::END);
		return rhs += 1;
	};

	constexpr inline Rank operator-(Rank lhs, jc::cx_integer auto rhs) noexcept
	{
		const auto _out = Rank(jc::to_underlying(lhs) - rhs);
		JCLIB_ASSERT(_out >= Rank::r1 && _out <= Rank::END);
		return _out;
	};
	constexpr inline Rank& operator-=(Rank& lhs, jc::cx_integer auto rhs) noexcept
	{
		lhs = lhs - rhs;
		return lhs;
	};

	constexpr inline Rank& operator--(Rank& rhs) noexcept
	{
		JCLIB_ASSERT(rhs != Rank::r1);
		return rhs -= 1;
	};

	constexpr inline uint8_t operator-(Rank lhs, Rank rhs) noexcept
	{
		return jc::to_underlying(lhs) - jc::to_underlying(rhs);
	};
	constexpr inline uint8_t distance(Rank lhs, Rank rhs) noexcept
	{
		return (lhs < rhs) ? (rhs - lhs) : (lhs - rhs);
	};
	constexpr inline int8_t sdistance(Rank lhs, Rank rhs) noexcept
	{
		return (int8_t)lhs - (int8_t)rhs;
	};


	/**
	 * @brief Named board file values (column)
	*/
	enum class File : uint8_t
	{
		a,
		b,
		c,
		d,
		e,
		f,
		g,
		h,
		END,
	};

	constexpr inline auto operator<=>(File lhs, File rhs) noexcept
	{
		return jc::to_underlying(lhs) <=> jc::to_underlying(rhs);
	};

	constexpr inline File operator+(File lhs, jc::cx_integer auto rhs) noexcept
	{
		const auto _out = File(jc::to_underlying(lhs) + rhs);
		JCLIB_ASSERT(_out >= File::a && _out <= File::END);
		return _out;
	};
	constexpr inline File& operator+=(File& lhs, jc::cx_integer auto rhs) noexcept
	{
		lhs = lhs + rhs;
		return lhs;
	};

	constexpr inline File& operator++(File& rhs) noexcept
	{
		JCLIB_ASSERT(rhs != File::END);
		return rhs += 1;
	};

	constexpr inline File operator-(File lhs, jc::cx_integer auto rhs) noexcept
	{
		const auto _out = File(jc::to_underlying(lhs) - rhs);
		JCLIB_ASSERT(_out >= File::a && _out <= File::END);
		return _out;
	};
	constexpr inline File& operator-=(File& lhs, jc::cx_integer auto rhs) noexcept
	{
		lhs = lhs - rhs;
		return lhs;
	};

	constexpr inline File& operator--(File& rhs) noexcept
	{
		JCLIB_ASSERT(rhs != File::a);
		return rhs -= 1;
	};

	constexpr inline uint8_t operator-(File lhs, File rhs) noexcept
	{
		return jc::to_underlying(lhs) - jc::to_underlying(rhs);
	};
	constexpr inline uint8_t distance(File lhs, File rhs) noexcept
	{
		return (lhs < rhs) ? (rhs - lhs) : (lhs - rhs);
	};
	constexpr inline int8_t sdistance(File lhs, File rhs) noexcept
	{
		return (int8_t)lhs - (int8_t)rhs;
	};


	/**
	 * @brief Enumerates the colors assigned to a piece or player
	*/
	enum class Color : bool
	{
		white = 0,
		black = 1,
	};

	/**
	 * @brief Converts white to black and vice-versa
	 * @param c Color to convert
	 * @return Inverted color
	*/
	constexpr inline Color operator!(Color c)
	{
		return Color(!jc::to_underlying(c));
	};


	/**
	 * @brief Describes the state a chess square may be in
	*/
	enum class Piece : uint8_t
	{
		empty = 0,

		pawn = 0b0010,
		pawn_white = pawn,
		pawn_black = 0b0011,

		knight = 0b0100,
		knight_white = knight,
		knight_black = 0b0101,

		bishop = 0b0110,
		bishop_white = bishop,
		bishop_black = 0b0111,

		rook = 0b1000,
		rook_white = rook,
		rook_black = 0b1001,

		queen = 0b1010,
		queen_white = queen,
		queen_black = 0b1011,

		king = 0b1110,
		king_white = king,
		king_black = 0b1111,
	};

	constexpr inline Piece operator|(Piece piece, Color color)
	{
		return Piece(jc::to_underlying(piece) |
			static_cast<std::underlying_type_t<Piece>>(jc::to_underlying(color)));
	};

	/**
	 * @brief Gets the color of a piece
	 * @param piece Piece to get color of
	 * @return Color value
	*/
	constexpr inline Color get_color(Piece _piece)
	{
		return static_cast<Color>(jc::to_underlying(_piece) & 0x1);
	};

	/**
	 * @brief Gets the white piece version of the given piece
	 * @param _piece Piece to get white version of
	 * @return Piece
	*/
	constexpr inline Piece as_white(Piece _piece)
	{
		return Piece(jc::to_underlying(_piece) & ~0b1);
	};

	/**
	 * @brief Gets the black piece version of the given piece
	 * @param _piece Piece to get black version of
	 * @return Piece
	*/
	constexpr inline Piece as_black(Piece _piece)
	{
		return _piece | Color::black;
	};

	/**
	 * @brief Checks if a piece is white, this is purely a convenience function
	 * @param _piece Piece to check on
	 * @return True if piece is white
	*/
	constexpr inline bool is_white(Piece _piece) noexcept
	{
		return get_color(_piece) == Color::white;
	};

	/**
	 * @brief Checks if a piece is black, this is purely a convenience function
	 * @param _piece Piece to check on
	 * @return True if piece is black
	*/
	constexpr inline bool is_black(Piece _piece) noexcept
	{
		return get_color(_piece) == Color::black;
	};

	/**
	 * @brief Gets the color of a square on a chess board when giving its index
	 * @param square Board square index
	 * @return Color of the square
	*/
	constexpr inline Color get_square_color(Square square)
	{
		JCLIB_ASSERT(square < 64);

		auto row = square % 8;
		auto file = square / 8;

		if (file & 1)
		{
			return Color(row & 1);
		}
		else
		{
			return Color(!(row & 1));
		};
	};

};

/*
	Various string conversions for the basic types
*/

#pragma region STRING_CONVERSIONS
namespace lbx::chess
{
	/**
	 * @brief Converts a chess piece to its UTF8 codepoint
	 * @param _piece Piece to convert, MUST NOT BE "Piece::empty"
	 * @return UTF8 codepoint as a series of 3 bytes
	*/
	constexpr inline std::array<uint8_t, 3> to_utf8(const Piece& _piece)
	{
		using type = std::array<uint8_t, 3>;
		using enum Piece;
		switch (_piece)
		{
		case pawn_white: return type{ 0xe2, 0x99, 0x99 };
		case pawn_black: return type{ 0xe2, 0x99, 0x9f };

		case rook_white: return type{ 0xe2, 0x99, 0x96 };
		case rook_black: return type{ 0xe2, 0x99, 0x9c };

		case knight_white: return type{ 0xe2, 0x99, 0x98 };
		case knight_black: return type{ 0xe2, 0x99, 0x9e };

		case bishop_white: return type{ 0xe2, 0x99, 0x97 };
		case bishop_black: return type{ 0xe2, 0x99, 0x9d };

		case queen_white: return type{ 0xe2, 0x99, 0x95 };
		case queen_black: return type{ 0xe2, 0x99, 0x9b };

		case king_white: return type{ 0xe2, 0x99, 0x94 };
		case king_black: return type{ 0xe2, 0x99, 0x9a };

		case empty:
			[[fallthrough]];
		default:
			JCLIB_ABORT();
			return type{};
		};
	};

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
	std::ostream& operator<<(std::ostream& _ostr, const Piece& _piece);

	/**
	 * @brief Gets the SAN character for a piece
	 * @param _piece Piece to get SAN for, MUST NOT BE EMPTY
	 * @return SAN character
	*/
	constexpr inline char to_san(const Piece& _piece)
	{
		switch (_piece)
		{
		case Piece::king_black:
			return 'k';
		case Piece::queen_black:
			return 'q';
		case Piece::bishop_black:
			return 'b';
		case Piece::knight_black:
			return 'n';
		case Piece::rook_black:
			return 'r';
		case Piece::pawn_black:
			return 'p';

		case Piece::king_white:
			return 'K';
		case Piece::queen_white:
			return 'Q';
		case Piece::bishop_white:
			return 'B';
		case Piece::knight_white:
			return 'N';
		case Piece::rook_white:
			return 'R';
		case Piece::pawn_white:
			return 'P';

		default:
			JCLIB_ABORT();
			return '\0';
		};
	};

	/**
	 * @brief Converts a SAN character into its piece value
	 * @param _san SAN character to convert
	 * @param _piece Piece to write conversion into (on good convert)
	 * @return True if SAN character was valid, false otherwise
	*/
	constexpr inline bool from_san(char _san, Piece& _piece)
	{
		switch (_san)
		{
		case 'r': // black rook
			_piece = Piece::rook_black;
			break;
		case 'n': // black knight
			_piece = Piece::knight_black;
			break;
		case 'b': // black bishop
			_piece = Piece::bishop_black;
			break;
		case 'q': // black queen
			_piece = Piece::queen_black;
			break;
		case 'k': // black king
			_piece = Piece::king_black;
			break;
		case 'p': // black pawn
			_piece = Piece::pawn_black;
			break;

		case 'R': // white rook
			_piece = Piece::rook_white;
			break;
		case 'N': // white knight
			_piece = Piece::knight_white;
			break;
		case 'B': // white bishop
			_piece = Piece::bishop_white;
			break;
		case 'Q': // white queen
			_piece = Piece::queen_white;
			break;
		case 'K': // white king
			_piece = Piece::king_white;
			break;
		case 'P': // white pawn
			_piece = Piece::pawn_white;
			break;

		default:
			return false;
		};

		// Good convert
		return true;
	};

	/**
	 * @brief Gets the converted string form of a rank
	 * @param _rank Rank to convert
	 * @return String
	*/
	constexpr inline std::string_view to_string(const Rank& _rank) noexcept
	{
		switch (_rank)
		{
		case Rank::r1:
			return "1";
		case Rank::r2:
			return "2";
		case Rank::r3:
			return "3";
		case Rank::r4:
			return "4";
		case Rank::r5:
			return "5";
		case Rank::r6:
			return "6";
		case Rank::r7:
			return "7";
		case Rank::r8:
			return "8";
		};
	};
	constexpr inline char to_char(const Rank& _rank) noexcept
	{
		return to_string(_rank).front();
	};

	/**
	 * @brief Converts a character into a rank
	 * @param c Character to convert from
	 * @param _outvalue Value to write the converted rank to
	 * @return True if character was valid, false otherwise
	*/
	constexpr inline bool from_char(const char c, Rank& _outvalue) noexcept
	{
		if (c >= '1' && c <= '8')
		{
			_outvalue = Rank::r1 + (c - '1');
			return true;
		}
		else
		{
			return false;
		};
	};


	/**
	 * @brief Gets the converted string form of a file
	 * @param _file File to convert
	 * @return String
	*/
	constexpr inline std::string_view to_string(const File& _file) noexcept
	{
		switch (_file)
		{
		case File::a:
			return "a";
		case File::b:
			return "b";
		case File::c:
			return "c";
		case File::d:
			return "d";
		case File::e:
			return "e";
		case File::f:
			return "f";
		case File::g:
			return "g";
		case File::h:
			return "h";
		};
	};
	constexpr inline char to_char(const File& _file) noexcept
	{
		return to_string(_file).front();
	};

	/**
	 * @brief Converts a character into a file
	 * @param c Character to convert from
	 * @param _outvalue Value to write the converted file to
	 * @return True if character was valid, false otherwise
	*/
	constexpr inline bool from_char(const char c, File& _outvalue) noexcept
	{
		if (c >= 'a' && c <= 'h')
		{
			_outvalue = File::a + (c - 'a');
			return true;
		}
		else
		{
			return false;
		};
	};

};
#pragma endregion STRING_CONVERSIONS


/*
	Formatters for the basic types
*/

#pragma region BASIC_TYPE_FORMATTERS
namespace std
{
	template <>
	struct formatter<lbx::chess::Rank> : public formatter<std::string_view, char>
	{
		auto format(const lbx::chess::Rank& _value, auto& _ctx)
		{
			const auto _str = lbx::chess::to_string(_value);
			return formatter<std::string_view, char>::format(_str, _ctx);
		};
	};
	template <>
	struct formatter<lbx::chess::File> : public formatter<std::string_view, char>
	{
		auto format(const lbx::chess::File& _value, auto& _ctx)
		{
			const auto _str = lbx::chess::to_string(_value);
			return formatter<std::string_view, char>::format(_str, _ctx);
		};
	};
};
#pragma endregion BASIC_TYPE_FORMATTERS

#endif // LAMBDEX_CHESS_BASIC_HPP