#pragma once

#include <jclib/type_traits.h>

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

	constexpr inline Rank operator+(Rank lhs, uint8_t rhs) noexcept
	{
		const auto _out = Rank(jc::to_underlying(lhs) + rhs);
		JCLIB_ASSERT(_out >= Rank::r1 && _out <= Rank::END);
		return _out;
	};
	constexpr inline Rank operator-(Rank lhs, uint8_t rhs) noexcept
	{
		const auto _out = Rank(jc::to_underlying(lhs) - rhs);
		JCLIB_ASSERT(_out >= Rank::r1 && _out <= Rank::END);
		return _out;
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

	constexpr inline File operator+(File lhs, uint8_t rhs) noexcept
	{
		const auto _out = File(jc::to_underlying(lhs) + rhs);
		JCLIB_ASSERT(_out >= File::a && _out <= File::END);
		return _out;
	};
	constexpr inline File operator-(File lhs, uint8_t rhs) noexcept
	{
		const auto _out = File(jc::to_underlying(lhs) - rhs);
		JCLIB_ASSERT(_out >= File::a && _out <= File::END);
		return _out;
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
		return Piece(jc::to_underlying(piece) | jc::to_underlying(color));
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
		constexpr explicit PositionPair(Rank _rank, File _file) :
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

};

#pragma region STRING_CONVERSIONS
namespace lbx::chess
{
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


	static_assert([]() {
		Position _pos{};
		from_chars(std::string_view{ "a6" }, _pos);
		return _pos == (Position)PositionPair { Rank::r6, File::a };
		}());

};
#pragma endregion STRING_CONVERSIONS