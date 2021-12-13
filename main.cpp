#include "api_glue.hpp"

#include "neural/network.hpp"






int main()
{
	using namespace lbx;

	api::set_env_folder_path(SOURCE_ROOT "/env");
	if (!api::load_env())
	{
		return 1;
	};

#if false
	{
		const bool _loadProgress = true;
		const bool _saveProgress = true;
		
		const fs::path _progressFilePath = SOURCE_ROOT "/nnet_dump/best.txt";
		if (!fs::exists(SOURCE_ROOT "/nnet_dump"))
		{
			fs::create_directory(SOURCE_ROOT "/nnet_dump");
		};

		using namespace chess;
		
		std::vector<std::pair<ChessEngine_Neural, float>> _generation(100);
		ChessEngine_Random _randomEngine{};
	
		if (_loadProgress)
		{
			std::vector<int> _genes{};
			std::ifstream _winnerFile(_progressFilePath);
			while (_winnerFile)
			{
				int n = 0;
				_winnerFile >> n;
				_genes.push_back(n);
				_winnerFile.ignore();
			};

			if (_genes.size() >= minimum_genes_to_describe_network(_generation.front().first.net_, 64))
			{
				for (auto& i : _generation | std::views::keys)
				{
					i.genes_ = _genes;
					build_network_from_genetics(i.genes_, i.net_);
				};
			};
		};

		float _lastBest = 0.0f;
		float _lastAverage = 0.0f;
		size_t _generationNumber = 0;

		auto _startTime = std::chrono::steady_clock::now();

		while (true)
		{
			// Calculate fitness for each in the generation
			for (auto& ip : _generation)
			{
				auto& _individual = ip.first;
				auto& _fitness = ip.second;

				// Play a match against the random chess engine
				MatchStats _stats{};
				const MatchVerdict _verdict = chess::play_standard_match(_individual, _randomEngine, &_stats);
				
				// Handle raw outcome of match
				if (_verdict.is_draw())
				{
					// Game drawn
					_fitness += 0.0f;
				}
				else if (_verdict.winner() == Color::white)
				{
					// NN engine won
					switch (_verdict.reason())
					{
					case MatchVerdict::checkmate:
						_fitness += 100.0f;
						break;
					case MatchVerdict::played_illegal_move:
						_fitness += 0.0f;
						break;
					case MatchVerdict::resigned:
						_fitness += 100.0f;
						break;
					case MatchVerdict::time:
						_fitness += 40.0f;
						break;
					default:
						_fitness += 10.0f;
						break;
					};
				}
				else if (_verdict.winner() == Color::black)
				{
					// NN engine lost
					switch (_verdict.reason())
					{
					case MatchVerdict::checkmate:
						_fitness -= 50.0f;
						break;
					case MatchVerdict::played_illegal_move:
						_fitness -= 5.0f;
						break;
					case MatchVerdict::resigned:
						_fitness -= 50.0f;
						break;
					case MatchVerdict::time:
						_fitness -= 20.0f;
						break;
					default:
						_fitness -= 5.0f;
						break;
					};
				};

				// Use the most recent valid move by the nn engine to factor rating into
				// fitness
				if (!_stats.moves_white.empty())
				{
					auto& _lastMove = _stats.moves_white.back();
					_fitness += _lastMove.get_rating();
				};

				// Add fitness points based on number of valid moves made, give 5 free points
				// due to an illegal move almost always ending a run
				_fitness += ((_stats.moves_white.size() + 1) * 5.0f);
			};

			// Sort by fitness
			std::ranges::sort(_generation, [](auto& lhs, auto& rhs) -> bool
				{
					return lhs.second > rhs.second;
				});

			// Log best fitness
			if ((_generationNumber % 1000) == 0 || _lastBest < _generation.front().second)
			{
				if (_lastBest < _generation.front().second)
				{
					_lastBest = _generation.front().second;
				};

				// Calculate average fitness
				float _avg = 0.0f;
				for (auto& f : _generation | std::views::values)
				{
					_avg += f;
				};
				_avg /= _generation.size();

				const auto _dur = std::chrono::steady_clock::now() - _startTime;
				const auto _durd = std::chrono::duration_cast<std::chrono::duration<double>>(_dur);
				println("Fitness [t={}] :\n best = {} avg = {} davg = {}", _durd, _generation.front().second, _avg, _avg - _lastAverage);
				_lastAverage = _avg;

				if (_saveProgress)
				{
					std::ofstream _file{ _progressFilePath };
					for (auto& g : _generation.front().first.genes_)
					{
						_file << g << ',';
					};
				};
			};


			// Randomly generate new generation if no individuals actually worked
			if (_generation.front().second <= 0.0f)
			{
				for (auto& i : _generation)
				{
					i.first = ChessEngine_Neural{};
					i.second = 0.0f;
				};
			}
			else
			{
				// Otherwise, select best and then mutate
				const auto _all = std::span{ _generation };
				for (auto& f : _all | std::views::values)
				{
					f = 0.0f;
				};

				const auto _best = _all.first(10);
				const auto _worst = _all.last(_all.size() - _best.size());

				// Create copies and mutate
				auto _bestIter = _best.begin();
				for (auto& w : _worst | std::views::keys)
				{
					w = _bestIter->first;
					
					// Mutate copy and rebuild network
					w.mutate(10000);

					// Basically a ring buffer
					++_bestIter;
					if (_bestIter == _best.end())
					{
						_bestIter = _best.begin();
					};
				};
			};

			// Next generation time!
			++_generationNumber;
		};

	};
#endif

	AccountAPI _accountAPI{};
	api::set_account_api(&_accountAPI);

	while (true)
	{
		api::forward_events();
		jc::sleep(0.1f);
	};

	return 0;
};
