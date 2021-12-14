#pragma once

#include <span>
#include <vector>
#include <cstdint>
#include <numbers>
#include <concepts>

namespace lbx::chess
{
	template <std::floating_point T>
	inline T sigmoid(T x)
	{
		constexpr T one_v{ 1 };
		return one_v / (one_v + std::pow(std::numbers::e_v<T>, -x));
	};


	struct Neuron
	{
		std::vector<float> weights{};
		float bias;
		float value;
	};


	struct Layer
	{
		void update(std::span<const Neuron> _previous)
		{
			for (auto& n : this->neurons)
			{
				n.value = 0.0f;
				auto _weightIter = n.weights.begin();
				for (auto& p : _previous)
				{
					n.value += (p.value * *_weightIter);
					++_weightIter;
				};
				n.value = sigmoid(n.value + n.bias);
			};
		};

		/**
		 * @brief Sets the number of neurons in this layer
		 * @param _count Number of neurons
		 * @param _fill Value to fill the container with
		*/
		void resize(size_t _count, const Neuron& _fill)
		{
			this->neurons.resize(_count, _fill);
		};

		std::vector<Neuron> neurons{};
	};

	struct Network
	{
		void update(std::span<const Neuron> _inputLayer)
		{
			auto it = this->layers.begin();

			// Update first layer
			it->update(_inputLayer);
			++it;

			// Propogate the rest
			for (it; it != this->layers.end(); ++it)
			{
				const auto& _previous = *(it - 1);
				it->update(_previous.neurons);
			};
		};

		auto& layer(size_t n)
		{
			return this->layers.at(n);
		};
		const auto& layer(size_t n) const
		{
			return this->layers.at(n);
		};
		
		auto& operator[](size_t n)
		{
			return this->layer(n);
		};
		const auto& operator[](size_t n) const
		{
			return this->layer(n);
		};

		/**
		 * @brief Sets the number of layers held by this network
		 * @param _nLayers Number of layers
		*/
		void resize(size_t _nLayers)
		{
			this->layers.resize(_nLayers);
		};

		std::vector<Layer> layers{};
	};

	/**
	 * @brief Defines a network as a series of layers
	 * @param _inputNodes Number of nodes in the input layer
	 * @param _layers Number of nodes in each subsequent layer of the network
	 * @return Network with defined layers
	*/
	inline Network define_network(size_t _inputNodes, std::span<const size_t> _layers)
	{
		Network _out{};
		_out.resize(_layers.size());

		// Resize each layer
		size_t _previousLayerSize = _inputNodes;
		for (size_t n = 0; n != _layers.size(); ++n)
		{
			_out[n].resize(_layers[n], Neuron{ std::vector<float>(_previousLayerSize, 0.0f) });
			_previousLayerSize = _layers[n];
		};

		return _out;
	};

	//
	// Genes are held in integers and values set by performing (n / 100000)
	// 
	// Returns iterator to last parsed gene
	inline auto build_network_from_genetics(std::span<const int32_t> _genes, Network& _outNetwork)
	{
		auto _geneIter = _genes.begin();
		for (auto& l : _outNetwork.layers)
		{
			for (auto& n : l.neurons)
			{
				for (auto& w : n.weights)
				{
					w = static_cast<float>(*_geneIter) / 100000.0f;
					++_geneIter;
				};

				n.bias = static_cast<float>(*_geneIter) / 100000.0f;
				++_geneIter;
			};
		};
		return _geneIter;
	};

	inline size_t minimum_genes_to_describe_network(const Network& _network, size_t _nInputs)
	{
		size_t _total = 0;
		for (auto& l : _network.layers)
		{
			_total += (l.neurons.size() * _nInputs) + l.neurons.size();
		};
		return _total;
	};

};