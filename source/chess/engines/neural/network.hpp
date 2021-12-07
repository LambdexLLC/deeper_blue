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



	constexpr inline size_t max_neuron_inputs_v = 8;

	using NeuronID = uint16_t;
	constexpr inline NeuronID null_neuron_v = 0;

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
		std::vector<Layer> layers{};
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