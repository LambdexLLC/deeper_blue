#pragma once

#include <jclib/config.h>
#include <jclib/type_traits.h>
#include <array>
#include <cstdint>
#include <optional>

namespace lbx {
	namespace chess {
		using Square = uint8_t;
		
		enum class Color : bool {
			white = 0,
			black = 1,
		};
		enum class Piece : uint8_t {
			empty  = 0,
			pawn   = 0b0010,
			knight = 0b0100,
			bishop = 0b0110,
			rook   = 0b1000,
			queen  = 0b1010,
			king   = 0b1110,
		};
		
		constexpr inline Piece operator|(Piece piece, Color color) {
			return Piece(jc::to_underlying(piece) | jc::to_underlying(color));
		}
		
		constexpr inline Color get_square_color(Square square) {
			JCLIB_ASSERT(square < 64);
			auto row  = square % 8;
			auto file = square / 8;
			if (file & 1) {
				return Color(row & 1);
			} else {
				return Color(!(row & 1));
			}
		}
		
		struct Move {
			
		};
		
		struct Board {
			std::array<Piece, 64> board{};
			std::optional<Square> en_passant = std::nullopt;
			bool black_can_castle_kingside  = true;
			bool black_can_castle_queenside = true;
			bool white_can_castle_kingside  = true;
			bool white_can_castle_queenside = true;
			Color turn = Color::white;
			
			constexpr void move() {
				
			}
		};
		
		constexpr inline Board standard_board() {
			const auto default_line = std::array<Piece, 8> {
				Piece::rook, Piece::knight, Piece::bishop, Piece::queen, Piece::king, Piece::bishop, Piece::knight, Piece::rook
			};
			
			Board output{};
			auto& board = output.board;
			
			for (int i = 0; i < 8; i++) {
				board[i + 55] = default_line[i] | Color::black;
				board[i + 47] = Piece::pawn     | Color::black;
				board[i + 8]  = Piece::pawn     | Color::white;
				board[i]      = default_line[i] | Color::white;
			}
			
			return output;
		}
	};
};
