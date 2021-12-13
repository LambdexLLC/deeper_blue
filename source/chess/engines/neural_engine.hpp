#pragma once

/*
	An attempt at a neural network based chess engine
*/

#include "utility/random.hpp"
#include "neural/network.hpp"

#include <lambdex/chess/chess_engine.hpp>

namespace lbx::chess
{
	/**
	 * @brief An attempt at a neural network based chess engine
	*/
	class ChessEngine_Neural : public IChessEngine
	{
	public:

		inline Network new_chess_network()
		{
			Network _out{};
			_out.layers.resize(4, Layer{});

			// input layer is implied
																							// 64
			_out.layers[0].neurons.resize(32, Neuron{ std::vector<float>(64, 0.0f) });		// 128
			_out.layers[1].neurons.resize(64, Neuron{ std::vector<float>(32, 0.0f) });		// 64
			_out.layers[2].neurons.resize(32, Neuron{ std::vector<float>(64, 0.0f) });		// 32
			_out.layers[3].neurons.resize(4, Neuron{ std::vector<float>(32, 0.0f) });		// 4
			
			// output layer, format is (from<0,1> to<2,3>)

			return _out;
		};

		/**
		 * Calculates a move using the neural network
		*/
		Move calculate_move(const BoardWithState& _board, Color _player);

		void play_turn(IGameInterface& _game) final;

		
		void sync_genes()
		{
			build_network_from_genetics(this->genes_, this->net_);
		};
		void mutate(int _factor = 1000)
		{
			std::uniform_int_distribution<int> _dist{ -_factor, _factor };
			for (auto& g : this->genes_)
			{
				g += rand<int>(_dist);
			};
			this->sync_genes();
		};

		void randomize_genes()
		{
			static std::uniform_int_distribution _dist{ -1000000, 1000000 };
			this->genes_.resize(chess::minimum_genes_to_describe_network(this->net_, 64));
			for (auto& g : this->genes_)
			{
				g = rand<int>(_dist);
			};
			this->sync_genes();
		};


		ChessEngine_Neural()
		{
			this->randomize_genes();
		};
		ChessEngine_Neural(std::vector<int> _genes) :
			genes_{ std::move(_genes) }
		{
			this->sync_genes();
		};

		std::array<Neuron, 64> input_layer_{};
		Network net_ = new_chess_network();
		std::vector<int> genes_{};
	};

};

