#include <lambdex/chess/fen.hpp>

#include <jclib/algorithm.h>

#include <numeric>

namespace lbx::chess
{
	/**
	 * @brief Helper type for reading out substrings split on a delimeter character
	*/
	struct string_split_reader
	{
	public:

		constexpr std::string_view next()
		{
			const auto _beginPos = this->offset_;
			const auto _endPos = std::min(this->str_.find(this->delim_, _beginPos), this->str_.size());
			this->offset_ = std::min(_endPos + 1, this->str_.size());
			return this->str_.substr(_beginPos, _endPos - _beginPos);
		};

		constexpr string_split_reader(std::string_view _str, char _delim) :
			str_{ _str }, delim_{ _delim }, offset_{ 0 }
		{};

	private:
		std::string_view str_;
		size_t offset_;
		char delim_;
	};

	/**
	 * @brief Creates a board with state from a fen string
	 * @param _fen Fen string, this is not validated!
	 * @return Board with state as specified by the fen string
	*/
	BoardWithState create_board_from_fen(const std::string_view _fen)
	{
		BoardWithState _board{};

		// Parse out the sections
		string_split_reader _sectionReader{ _fen, ' ' };

		// Parse out the piece positions
		{
			string_split_reader _reader{ _sectionReader.next(), '/' };
			std::string_view _part{};
			auto _rank = Rank::END;
			while (!(_part = _reader.next()).empty())
			{
				--_rank;
				auto _file = File::a;
				for (auto& c : _part)
				{
					// Determine which piece to place, or skip
					// spaces as appropriate
					Piece _piece = Piece::empty;
					switch (c)
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

					case '1': [[fallthrough]];
					case '2': [[fallthrough]];
					case '3': [[fallthrough]];
					case '4': [[fallthrough]];
					case '5': [[fallthrough]];
					case '6': [[fallthrough]];
					case '7': [[fallthrough]];
					case '8':
					{
						uint8_t _count = c - '0';
						_file += _count;
					};
					continue;
					default:
						JCLIB_ABORT();
						break;
					};

					// Place piece on board
					_board[(_file, _rank)] = _piece;
					++_file;
				};
			};
		};

		// Parse out the active color
		{
			auto _activeColor = _sectionReader.next();
			switch (_activeColor.front())
			{
			case 'w': // white's turn
				_board.turn = Color::white;
				break;
			case 'b': // black's turn
				_board.turn = Color::black;
				break;
			default:
				JCLIB_ABORT();
				break;
			};
		};

		// Parse out castling availablility
		{
			std::string_view _castlingStr = _sectionReader.next();

			// Default to no castling possible
			_board.black_can_castle_kingside = false;
			_board.black_can_castle_queenside = false;
			_board.white_can_castle_kingside = false;
			_board.white_can_castle_queenside = false;

			// Check if any castling moves are available
			if (_castlingStr.front() != '-')
			{
				if (jc::contains(_castlingStr, 'K'))
				{
					_board.white_can_castle_kingside = true;
				};
				if (jc::contains(_castlingStr, 'Q'))
				{
					_board.white_can_castle_queenside = true;
				};
				if (jc::contains(_castlingStr, 'k'))
				{
					_board.black_can_castle_kingside = true;
				};
				if (jc::contains(_castlingStr, 'q'))
				{
					_board.black_can_castle_queenside = true;
				};
			};
		};

		// Parse out en passant
		{
			std::string_view _enPassantStr = _sectionReader.next();

			// Default to no en passant available
			_board.clear_en_passant();
			if (_enPassantStr.front() != '-')
			{
				// En passant possible, TAKE IT
				PositionPair _enPassantPos{};
				chess::from_chars(_enPassantStr.data(), _enPassantStr.data() + _enPassantStr.size(), _enPassantPos);
				_board.set_en_passant(_enPassantPos);
			};
		};

		// Parse out half move counter
		{
			std::string_view _halfMoveStr = _sectionReader.next();
			std::from_chars(_halfMoveStr.data(), _halfMoveStr.data() + _halfMoveStr.size(), _board.half_move_counter);
		};

		// Parse out full move counter
		{
			std::string_view _fullMoveStr = _sectionReader.next();
			std::from_chars(_fullMoveStr.data(), _fullMoveStr.data() + _fullMoveStr.size(), _board.full_move_counter);
		};

		return _board;
	};
}