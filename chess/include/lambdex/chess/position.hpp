#pragma once

#include "basic.hpp"

#include <charconv>

namespace lbx::chess
{
	struct Position;

	struct PositionPair;
	struct PositionPair_Offset;



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
	 * @brief Invariant for holding an offset between two position pairs
	*/
	struct PositionPair_Offset
	{
	public:

		/**
		 * @brief Underlying value type for holding the position offset values
		*/
		using value_type = int8_t;

		/**
		 * @brief Gets the file position offset
		 * @return File offset
		*/
		constexpr value_type file_offset() const noexcept
		{
			return this->file_;
		};

		/**
		 * @brief Gets the rank position offset
		 * @return Rank offset
		*/
		constexpr value_type rank_offset() const noexcept
		{
			return this->rank_;
		};
		

		constexpr bool operator==(const PositionPair_Offset& rhs) const noexcept = default;


		/**
		 * @brief Initializes the offset values
		 * @param _fileOffset Offset between files
		 * @param _rankOffset Offset between ranks
		*/
		constexpr explicit PositionPair_Offset(value_type _fileOffset, value_type _rankOffset) :
			file_{ _fileOffset }, rank_{ _rankOffset }
		{};

	private:

		/**
		 * @brief The file offset
		*/
		value_type file_ : 4;

		/**
		 * @brief The rank offset
		*/
		value_type rank_ : 4;

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

		/**
		 * @brief Checks if this position is not null
		 * @return True if not null, false otherwise
		*/
		constexpr bool good() const noexcept
		{
			return *this != this->end();
		};

		/**
		 * @brief Checks if this position is not null (same as "good()")
		 * @return True if not null, false otherwise
		*/
		constexpr explicit operator bool() const noexcept
		{
			return this->good();
		};



		constexpr auto operator<=>(const PositionPair& rhs) const noexcept = default;


		// Define offset related operator behavior

		friend constexpr inline PositionPair operator+(const PositionPair& lhs, const PositionPair_Offset& rhs)
		{
			PositionPair _out{ lhs };
			_out.file_ += rhs.file_offset();
			_out.rank_ += rhs.rank_offset();
			JCLIB_ASSERT(_out.file_ < 8 && _out.rank_ < 8);
			return _out;
		};
		friend constexpr inline PositionPair operator+(const PositionPair_Offset& lhs, const PositionPair& rhs)
		{
			return rhs + lhs;
		};

		friend constexpr inline PositionPair_Offset operator-(const PositionPair& lhs, const PositionPair& rhs)
		{
			return PositionPair_Offset{ lhs.file_ - rhs.file_, lhs.rank_ - rhs.rank_ };
		};



		/**
		 * @brief Default constructs the pair to hold the position "a1"
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
		constexpr explicit PositionPair(File _file, Rank _rank) noexcept :
			file_{ jc::to_underlying(_file) },
			rank_{ jc::to_underlying(_rank) }
		{};

		/**
		 * @brief Unchecked constructor taking a rank and file value
		 * @param _rank Rank of the position
		 * @param _file File of the position
		*/
		constexpr explicit PositionPair(Rank _rank, File _file) noexcept :
			PositionPair{ _file, _rank }
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

	/**
	 * @brief Creates a position pair value from a rank and file
	 * @param _file File value
	 * @param _rank Rank value
	 * @return Position pair
	*/
	constexpr inline PositionPair operator,(File _file, Rank _rank) noexcept
	{
		return PositionPair{ _file, _rank };
	};


	/**
	 * @brief Same as the PositionPair struct but does not perform bounds checking
	*/
	struct PositionPair_Unbounded
	{
	public:

		/**
		 * @brief Checks if this position is within the bounds of a standard chess board (8x8)
		 * @return True if within bounds, false otherwise
		*/
		constexpr bool is_within_bounds() const noexcept
		{
			return this->file_ < 8 && this->rank_ < 8;
		};

		/**
		 * @brief Converts this to a bounded position pair
		 *
		 * is_within_bounds() MUST RETURN TRUE
		 * 
		 * @return Bounded position pair
		*/
		constexpr PositionPair as_bounded() const noexcept
		{
			JCLIB_ASSERT(this->is_within_bounds());
			return PositionPair{ File(this->file_), Rank(this->rank_) };
		};

		/**
		 * @brief Converts this to a bounded position pair, same as "as_bounded()"
		 *
		 * is_within_bounds() MUST RETURN TRUE
		 *
		 * @return Bounded position pair
		*/
		constexpr operator PositionPair() const noexcept
		{
			return this->as_bounded();
		};

		/**
		 * @brief Converts this to a bounded position
		 *
		 * is_within_bounds() MUST RETURN TRUE
		 *
		 * @return Bounded position
		*/
		constexpr operator Position() const noexcept
		{
			return this->as_bounded();
		};



		// Define offset related operator behavior

		friend constexpr inline PositionPair_Unbounded operator+(const PositionPair_Unbounded& lhs, const PositionPair_Offset& rhs)
		{
			PositionPair_Unbounded _out{ lhs };
			_out.file_ += rhs.file_offset();
			_out.rank_ += rhs.rank_offset();
			return _out;
		};
		friend constexpr inline PositionPair_Unbounded operator+(const PositionPair_Offset& lhs, const PositionPair_Unbounded& rhs)
		{
			return rhs + lhs;
		};

		friend constexpr inline PositionPair_Offset operator-(const PositionPair_Unbounded& lhs, const PositionPair_Unbounded& rhs)
		{
			return PositionPair_Offset{ lhs.file_ - rhs.file_, lhs.rank_ - rhs.rank_ };
		};



		/**
		 * @brief Initializes the position with unbounded rank and file
		 * @param _file Unbounded file
		 * @param _rank Unbounded rank
		*/
		constexpr explicit PositionPair_Unbounded(uint8_t _file, uint8_t _rank) noexcept :
			file_{ _file },
			rank_{ _rank }
		{};

		/**
		 * @brief Unchecked constructor taking a rank and file value
		 * @param _rank Rank of the position
		 * @param _file File of the position
		*/
		constexpr explicit PositionPair_Unbounded(File _file, Rank _rank) noexcept :
			PositionPair_Unbounded{ jc::to_underlying(_file), jc::to_underlying(_rank) }
		{};

		/**
		 * @brief Unchecked constructor taking a rank and file value
		 * @param _rank Rank of the position
		 * @param _file File of the position
		*/
		constexpr explicit PositionPair_Unbounded(Rank _rank, File _file) noexcept :
			PositionPair_Unbounded{ _file, _rank }
		{};

		/**
		 * @brief Unchecked constructor taking a bounded position pair
		 * @param _pos Bounded position pair
		*/
		constexpr PositionPair_Unbounded(PositionPair _pos) noexcept :
			PositionPair_Unbounded{ _pos.file(), _pos.rank() }
		{};

	private:

		/**
		 * @brief The unbounded file for this position
		*/
		uint8_t file_ : 4;

		/**
		 * @brief The unbounded rank for this position
		*/
		uint8_t rank_ : 4;

	};


#pragma region COMPILE_TIME_TESTS
	static_assert(distance(Rank::r1, Rank::r5) == distance(Rank::r5, Rank::r1));
	static_assert(distance(File::f, File::a) == distance(File::a, File::f));

	static_assert((Position)PositionPair { Rank::r1, File::a } == Position{ 0 });

	static_assert((Position)PositionPair { Rank::r1, File::b } == Position{ 1 });
	static_assert(PositionPair{ Rank::r1, File::b } == PositionPair{ Position{ 1 } });

	static_assert(PositionPair{ Rank::r8, File::h } == PositionPair{ Position{ 63 } });
	static_assert((Position)PositionPair { Rank::r8, File::h } == Position{ 63 });

	static_assert(PositionPair{ Rank::r1, File::h } == PositionPair{ Position{ 7 } });
	static_assert((Position)PositionPair { Rank::r1, File::h } == Position{ 7 });

	static_assert(PositionPair{ Rank::r8, File::a } == PositionPair{ Position{ 56 } });
	static_assert((Position)PositionPair { Rank::r8, File::a } == Position{ 56 });
#pragma endregion COMPILE_TIME_TESTS
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


	static_assert([]()
	{
		Position _pos{};
		from_chars(std::string_view{ "a6" }, _pos);
		return _pos == (Position)PositionPair { Rank::r6, File::a };
	}());

};
#pragma endregion STRING_CONVERSIONS

#pragma region FORMATTERS

namespace std
{
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

#pragma endregion FORMATTERS
