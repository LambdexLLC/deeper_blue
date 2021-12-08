#pragma once

#include "utility/format.hpp"

#include <jclib/concepts.h>
#include <jclib/type_traits.h>

#include <iosfwd>

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
		return Color( !jc::to_underlying(c) );
	};


	/**
	 * @brief Describes the state a chess square may be in
	*/
	enum class Piece : uint8_t
	{
		empty = 0,

		pawn		= 0b0010,
		pawn_white  = pawn,
		pawn_black	= 0b0011,
		
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
		return Piece( jc::to_underlying(_piece) & ~0b1 );
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

	/**
	 * @brief Integer invariant holding a board position as a single index onto a board
	*/
	struct Position
	{
	public:

		/**
		 * @brief Internal type used to hold the position
		*/
		using value_type = uint8_t;

		/**
		 * @brief Gets the raw value for this position
		 * @return Position value
		*/
		constexpr value_type get() const noexcept
		{
			return this->value_;
		};

		/**
		 * @brief Gets the raw value for this position
		 * @return Position value
		*/
		constexpr explicit operator value_type() const noexcept
		{
			return this->get();
		};

		/**
		 * @brief Gets 1 + the maximum value a position may have
		 * @return Position{ 64 };
		*/
		constexpr static Position end() noexcept
		{
			return Position{ 64 };
		};


		// Three-way-compare lets goo
		constexpr auto operator<=>(const Position& rhs) const noexcept = default;
		constexpr auto operator<=>(const value_type& rhs) const noexcept
		{
			return this->get() <=> rhs;
		};

		constexpr Position& operator+=(value_type inc) noexcept
		{
			this->value_ += inc;
			JCLIB_ASSERT(this->get() <= 64);
			return *this;
		};
		constexpr Position& operator-=(value_type inc) noexcept
		{
			this->value_ -= inc;
			JCLIB_ASSERT(this->get() <= 64);
			return *this;
		};
		
		constexpr Position& operator++()
		{
			return (*this) += 1;
		};
		constexpr Position& operator--()
		{
			return (*this) -= 1;
		};

		friend constexpr inline Position operator+(const Position& lhs, value_type rhs) noexcept
		{
			auto _out = lhs;
			_out += rhs;
			return _out;
		};
		friend constexpr inline Position operator+(value_type lhs, const Position& rhs) noexcept
		{
			return rhs + lhs;
		};
		friend constexpr inline Position operator-(const Position& lhs, value_type rhs) noexcept
		{
			auto _out = lhs;
			_out -= rhs;
			return _out;
		};
		

		/**
		 * @brief Constructs the position as square "a1"
		*/
		constexpr Position() noexcept = default;
		
		/**
		 * @brief Non-checking construction of position value
		 * @param _position Square position value
		*/
		constexpr explicit Position(value_type _position) noexcept :
			value_{ _position }
		{};

	private:
		value_type value_ = 0;
	};

	/**
	 * @brief Holds a board position as a (file, column) pair
	*/
	struct PositionPair
	{
	public:

		/**
		 * @brief Underlying value type for holding the position values
		*/
		using value_type = uint8_t;

		/**
		 * @brief Gets the file held by this position
		 * @return File number
		*/
		constexpr File file() const noexcept
		{
			return static_cast<File>(this->file_);
		};

		/**
		 * @brief Gets the rank held by this position
		 * @return Rank value
		*/
		constexpr Rank rank() const noexcept
		{
			return static_cast<Rank>(this->rank_);
		};
		
		/**
		 * @brief Allows implicit conversion to a position value
		 * @return Position as an index into the board
		*/
		constexpr operator Position() const noexcept
		{
			// Writing this out the long way as the compiler will compress it down
			Position::value_type _val = jc::to_underlying(this->rank());
			_val *= 8;
			_val += jc::to_underlying(this->file());
			return Position{ _val };
		};

		/**
		 * @brief Gets 1 + the maximum value a position may have
		 * @return Position{ 64 };
		*/
		constexpr static PositionPair end() noexcept
		{
			return PositionPair{ Position::end() };
		};



		constexpr auto operator<=>(const PositionPair& rhs) const noexcept = default;


		/**
		 * @brief Default constructs the pair to hold the position a1
		*/
		constexpr PositionPair() noexcept :
			file_{ 0 },
			rank_{ 0 }
		{};

		/**
		 * @brief Converts a board index into a position pair
		 * @param Board position as an index
		*/
		constexpr PositionPair(const Position& _pos) noexcept :
			file_{ static_cast<value_type>(_pos.get() % 8) },
			rank_{ static_cast<value_type>(_pos.get() / 8) }
		{};

		/**
		 * @brief Unchecked constructor taking a rank and file value
		 * @param _rank Rank of the position
		 * @param _file File of the position
		*/
		constexpr explicit PositionPair(Rank _rank, File _file) noexcept :
			file_{ jc::to_underlying(_file) },
			rank_{ jc::to_underlying(_rank) }
		{};
		
	private:
		uint8_t file_ : 4, rank_ : 4;
	};

	/**
	 * @brief Creates a position pair value from a rank and file
	 * @param _rank Rank value
	 * @param _file File value
	 * @return Position pair
	*/
	constexpr inline PositionPair operator,(Rank _rank, File _file) noexcept
	{
		return PositionPair{ _rank, _file };
	};

#pragma region COMPILE_TIME_TESTS
	static_assert(distance(Rank::r1, Rank::r5) == distance(Rank::r5, Rank::r1));
	static_assert(distance(File::f, File::a) == distance(File::a, File::f));

	static_assert((Position)PositionPair{ Rank::r1, File::a } == Position{ 0 });
	
	static_assert((Position)PositionPair{ Rank::r1, File::b } == Position{ 1 });
	static_assert(PositionPair{ Rank::r1, File::b } == PositionPair{ Position{ 1 } });
	
	static_assert(PositionPair{ Rank::r8, File::h } == PositionPair{ Position{ 63 } });
	static_assert((Position)PositionPair{ Rank::r8, File::h } == Position{ 63 });

	static_assert(PositionPair{ Rank::r1, File::h } == PositionPair{ Position{ 7 } });
	static_assert((Position)PositionPair{ Rank::r1, File::h } == Position{ 7 });

	static_assert(PositionPair{ Rank::r8, File::a } == PositionPair{ Position{ 56 } });
	static_assert((Position)PositionPair { Rank::r8, File::a } == Position{ 56 });
#pragma endregion COMPILE_TIME_TESTS
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

	/**
	 * @brief Parses a chess square from a string formatted like "a1"
	 * @param _begin Beginning of string section to parse
	 * @param _end End of string section to parse
	 * @param _value Output value reference
	 * @return std::from_chars result type
	*/
	constexpr inline std::from_chars_result from_chars(const char* _begin, const char* _end, PositionPair& _value)
	{
		// Check for valid sized input string
		const auto _distance = _end - _begin;
		if (_distance < 2)
		{
			// Invalid input
			return std::from_chars_result{ .ptr = _begin, .ec = std::errc::invalid_argument };
		};

		const auto _file = static_cast<uint8_t>(_begin[0] - 'a');
		const auto _rank = static_cast<uint8_t>(_begin[1] - '1');

		if (_file < 0 || _file > 7 || _rank < 0 || _rank > 7)
		{
			// Invalid input
			return std::from_chars_result{ .ptr = _begin + 2, .ec = std::errc::invalid_argument };
		}
		else
		{
			_value = PositionPair{ Rank { _rank }, File{ _file } };
			return std::from_chars_result{ .ptr = _begin + 2, .ec = std::errc{} };
		};
	};
	constexpr inline std::to_chars_result to_chars(char* _begin, char* _end, const PositionPair& _value)
	{
		if (_end - _begin < 2)
		{
			return std::to_chars_result{ .ptr = _begin, .ec = std::errc::no_buffer_space };
		}
		else
		{
			_begin[0] = 'a' + jc::to_underlying(_value.file());
			_begin[1] = '1' + jc::to_underlying(_value.rank());
			return std::to_chars_result{ .ptr = _begin + 2, .ec = std::errc{} };
		};
	};

	// Shorthand version of to_chars
	constexpr inline std::string to_string(const PositionPair& _value)
	{
		std::string _out(2, '\0');
		
		// Convert each component and set their characters for the output string
		const auto _file = to_string(_value.file());
		const auto _rank = to_string(_value.rank());
		_out[0] = _file[0];
		_out[1] = _file[2];

		return _out;
	};


	/**
	 * @brief Parses a chess square from a string formatted like "a1"
	 * @param _begin Beginning of string section to parse
	 * @param _end End of string section to parse
	 * @param _value Output value reference
	 * @return std::from_chars result type
	*/
	constexpr inline std::from_chars_result from_chars(const char* _begin, const char* _end, Position& _value)
	{
		PositionPair _pair{};
		auto _out = from_chars(_begin, _end, _pair);
		_value = _pair;
		return _out;
	};
	constexpr inline std::from_chars_result from_chars(std::string_view _str, Position& _value)
	{
		return from_chars(_str.data(), _str.data() + _str.size(), _value);
	};

	constexpr inline std::to_chars_result to_chars(char* _begin, char* _end, const Position& _value)
	{
		return to_chars(_begin, _end, PositionPair{ _value });
	};

	// Shorthand version of to_chars, calls the to_string for PositionPair
	constexpr inline std::string to_string(const Position& _value)
	{
		return to_string(PositionPair{ _value });
	};



	static_assert([]() {
		Position _pos{};
		from_chars(std::string_view{ "a6" }, _pos);
		return _pos == (Position)PositionPair { Rank::r6, File::a };
		}());

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
	template <>
	struct formatter<lbx::chess::PositionPair> : public formatter<std::string, char>
	{
		auto format(const lbx::chess::PositionPair& _value, auto& _ctx)
		{
			const auto _str = lbx::chess::to_string(_value);
			return formatter<std::string, char>::format(_str, _ctx);
		};
	};
	template <>
	struct formatter<lbx::chess::Position> : public formatter<std::string, char>
	{
		auto format(const lbx::chess::Position& _value, auto& _ctx)
		{
			const auto _str = lbx::chess::to_string(_value);
			return formatter<std::string, char>::format(_str, _ctx);
		};
	};
};
#pragma endregion BASIC_TYPE_FORMATTERS
