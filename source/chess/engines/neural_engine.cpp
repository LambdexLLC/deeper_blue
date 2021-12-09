#include "neural_engine.hpp"

#include <numbers>
#include <concepts>

namespace lbx::chess
{

	inline float get_square_neuron_value(Piece _piece, Color _player)
	{
		// Gets the raw value, always from the view of black
		const auto _rawValue = [](Piece p) -> float
		{
			switch (p)
			{
			case Piece::pawn_black:		return 0.2f;
			case Piece::pawn_white:		return -0.2f;

			case Piece::bishop_black:	return 0.4f;
			case Piece::bishop_white:	return -0.4f;
			
			case Piece::rook_black:		return 0.6f;
			case Piece::rook_white:		return -0.7f;
			
			case Piece::knight_black:	return 0.2f;
			case Piece::knight_white:	return -0.2f;
			
			case Piece::queen_black:	return 0.8f;
			case Piece::queen_white:	return -0.8f;
			
			case Piece::king_black:		return 1.0f;
			case Piece::king_white:		return -1.0f;
			
			case Piece::empty:			return 0.0f;
			};
		}(_piece);

		// Flip the values if required
		if (_player == Color::white)
		{
			return -_rawValue;
		}
		else
		{
			return _rawValue;
		};
	};




	Move ChessEngine_Neural::calculate_move(const BoardWithState& _board, Color _player)
	{
		// Fill the input nodes using the board
		{
			auto it = this->input_layer_.begin();
			for (auto& p : _board)
			{
				const auto _value = get_square_neuron_value(p, _player);
				it->value = _value;
				++it;
			};
		};

		// Update neurons
		this->net_.update(this->input_layer_);
		
		// Parse out the move
		auto& _output = this->net_.layers.back();

		// From (Rank, File)
		Rank _fromRank = (Rank)(_output.neurons[0].value * 7);
		File _fromFile = (File)(_output.neurons[1].value * 7);

		// To (Rank, File)
		Rank _toRank = (Rank)(_output.neurons[2].value * 7);
		File _toFile = (File)(_output.neurons[3].value * 7);

		// Combine move
		Move _out{};
		_out.from = (_fromRank, _fromFile);
		_out.to = (_toRank, _toFile);
		
		return _out;
	};




	void ChessEngine_Neural::play_turn(IGameInterface& _game)
	{
		const auto _move = this->calculate_move(_game.get_board(), _game.get_color());
		if (!_game.submit_move(_move))
		{
			_game.resign();
		};
	};

};