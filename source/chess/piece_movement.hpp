#pragma once

/*
	Provides functions for examining piece movement
*/

#include "basic.hpp"

#include "board/bitboard.hpp"

namespace lbx::chess
{
	namespace impl
	{
		consteval inline BitBoard make_bitboard_with_bits_for_file(File _file) noexcept
		{
			std::array<uint8_t, 8> _bytes
			{
				0b00000001,
				0b00000001,
				0b00000001,
				0b00000001,
				0b00000001,
				0b00000001,
				0b00000001,
				0b00000001,
			};
			for (auto& b : _bytes)
			{
				b <<= jc::to_underlying(_file);
			};
			return BitBoard{ _bytes };
		};
		consteval inline BitBoard make_bitboard_with_bits_for_rank(Rank _rank) noexcept
		{
			std::array<uint8_t, 8> _bytes
			{
				0b00000000, // r1
				0b00000000,
				0b00000000,
				0b00000000,
				0b00000000,
				0b00000000,
				0b00000000,
				0b00000000, // r8
			};
			_bytes.at(jc::to_underlying(_rank)) = 0xFF;
			return BitBoard{ _bytes };
		};
	};

	constexpr inline BitBoard bits_in_file(File _file)
	{
		constexpr std::array<BitBoard, 8> _boards
		{
			impl::make_bitboard_with_bits_for_file(File::a),
			impl::make_bitboard_with_bits_for_file(File::b),
			impl::make_bitboard_with_bits_for_file(File::c),
			impl::make_bitboard_with_bits_for_file(File::d),
			impl::make_bitboard_with_bits_for_file(File::e),
			impl::make_bitboard_with_bits_for_file(File::f),
			impl::make_bitboard_with_bits_for_file(File::g),
			impl::make_bitboard_with_bits_for_file(File::h)
		};
		return _boards.at(jc::to_underlying(_file));
	};
	constexpr inline BitBoard bits_in_rank(Rank _rank)
	{
		constexpr std::array<BitBoard, 8> _boards
		{
			impl::make_bitboard_with_bits_for_rank(Rank::r1),
			impl::make_bitboard_with_bits_for_rank(Rank::r2),
			impl::make_bitboard_with_bits_for_rank(Rank::r3),
			impl::make_bitboard_with_bits_for_rank(Rank::r4),
			impl::make_bitboard_with_bits_for_rank(Rank::r5),
			impl::make_bitboard_with_bits_for_rank(Rank::r6),
			impl::make_bitboard_with_bits_for_rank(Rank::r7),
			impl::make_bitboard_with_bits_for_rank(Rank::r8)
		};
		return _boards.at(jc::to_underlying(_rank));
	};

	namespace impl
	{
		consteval inline BitBoard make_white_square_bits()
		{
			BitBoard _out{};
			Position p{ 0 };
			for (p; p != Position::end(); p += 2)
			{
				_out.set(p + 1);
			};
			return _out;
		};
		consteval inline BitBoard make_black_square_bits()
		{
			BitBoard _out{};
			Position p{};
			for (p; p != Position::end(); p += 2)
			{
				_out.set(p);
			};
			return _out;
		};
	};
	
	consteval inline BitBoard white_square_bits()
	{
		constexpr auto _board = impl::make_white_square_bits();
		return _board;
	};
	consteval inline BitBoard black_square_bits()
	{
		constexpr auto _board = impl::make_black_square_bits();
		return _board;
	};




	// Check that it outputs the correct board
	static_assert([]() -> bool
		{
			const auto _moves = bits_in_rank(Rank::r4);

			if (!_moves[(Rank::r4, File::a)])
			{
				return false;
			};
			if (!_moves[(Rank::r4, File::b)])
			{
				return false;
			};
			if (!_moves[(Rank::r4, File::c)])
			{
				return false;
			};
			if (!_moves[(Rank::r4, File::d)])
			{
				return false;
			};
			if (!_moves[(Rank::r4, File::e)])
			{
				return false;
			};
			if (!_moves[(Rank::r4, File::f)])
			{
				return false;
			};
			if (!_moves[(Rank::r4, File::g)])
			{
				return false;
			};
			if (!_moves[(Rank::r4, File::h)])
			{
				return false;
			};

			return true;
		}(), "get_bits_in_rank() is incorrect");
	
	static_assert([]() -> bool
		{
			const auto _moves = bits_in_file(File::d);

			if (!_moves[(Rank::r1, File::d)])
			{
				return false;
			};
			if (!_moves[(Rank::r2, File::d)])
			{
				return false;
			};
			if (!_moves[(Rank::r3, File::d)])
			{
				return false;
			};
			if (!_moves[(Rank::r4, File::d)])
			{
				return false;
			};
			if (!_moves[(Rank::r5, File::d)])
			{
				return false;
			};
			if (!_moves[(Rank::r6, File::d)])
			{
				return false;
			};
			if (!_moves[(Rank::r7, File::d)])
			{
				return false;
			};
			if (!_moves[(Rank::r8, File::d)])
			{
				return false;
			};

			return true;
		}(), "get_bits_in_file() is incorrect");

	/**
	 * @brief Makes a bitboard containing the squares a rook can move to
	 * 
	 * Does not take pieces blocking movement into account!
	 * 
	 * @param _rookPos Where the rook is on the board
	 * @return Bit board containing moves
	*/
	constexpr inline BitBoard get_rook_movement_bits(const PositionPair& _rookPos)
	{
		return bits_in_file(_rookPos.file()) | bits_in_rank(_rookPos.rank());
	};

	static_assert([]() -> bool
		{
			const auto _bits = get_rook_movement_bits((Rank::r4, File::c));
			if (!_bits[(Rank::r2, File::c)])
			{
				return false;
			};

			return true;
		}(), "get_rook_movement_bits() is incorrect");

	/**
	 * @brief Makes a bitboard containing the squares a knight can move to
	 *
	 * Does not take pieces blocking movement into account!
	 *
	 * @param _pos Where the knight is on the board
	 * @return Bit board containing moves
	*/
	constexpr inline BitBoard get_knight_movement_bits(const PositionPair& _pos)
	{
		const auto _rank = static_cast<int>(_pos.rank());
		const auto _file = static_cast<int>(_pos.file());

		struct PossibleMove
		{
			int rank;
			int file;
		};
		const std::array<PossibleMove, 8> _unboundedMoves
		{
			PossibleMove{ _rank + 1, _file + 2 },
			PossibleMove{ _rank + 2, _file + 1 },
			PossibleMove{ _rank - 1, _file - 2 },
			PossibleMove{ _rank - 2, _file - 1 },
			PossibleMove{ _rank + 1, _file - 2 },
			PossibleMove{ _rank + 2, _file - 1 },
			PossibleMove{ _rank - 1, _file + 2 },
			PossibleMove{ _rank - 2, _file + 1 },
		};

		BitBoard _out{};
		for(auto& m : _unboundedMoves)
		{
			if (m.rank >= 0 && m.rank <= 7 &&
				m.file >= 0 && m.file <= 7)
			{
				_out.set(((Rank)m.rank, (File)m.file));
			};
		};

		return _out;
	};

	/**
	 * @brief Makes a bitboard containing the squares a king can move to
	 *
	 * Does not take pieces blocking movement into account!
	 * Also does not factor in check!
	 *
	 * @param _pos Where the king is on the board
	 * @return Bit board containing moves
	*/
	constexpr inline BitBoard get_king_movement_bits(const PositionPair& _pos)
	{
		BitBoard _out{};

		const auto _rank = static_cast<int>(_pos.rank());
		const auto _file = static_cast<int>(_pos.file());

		struct PossibleMove
		{
			int rank;
			int file;
		};

		const std::array<PossibleMove, 8> _unboundedMoves
		{
			PossibleMove{ _rank + 1, _file + 1 },
			PossibleMove{ _rank + 1, _file + 0 },
			PossibleMove{ _rank + 1, _file - 1 },

			PossibleMove{ _rank + 0, _file + 1 },
			PossibleMove{ _rank + 0, _file - 1 },
			
			PossibleMove{ _rank - 1, _file + 1 },
			PossibleMove{ _rank - 1, _file + 0 },
			PossibleMove{ _rank - 1, _file - 1 },
		};

		for (auto& m : _unboundedMoves)
		{
			if (m.rank >= 0 && m.rank <= 7 &&
				m.file >= 0 && m.file <= 7)
			{
				_out.set(((Rank)m.rank, (File)m.file));
			};
		};

		return _out;
	};

};
