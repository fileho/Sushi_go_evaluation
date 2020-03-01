#include "genetic_algorithm.h"

#include <random>

genetic_algorithm::genetic_algorithm()
{
	for (int i{}; i < population_size; ++i)
		population.emplace_back(create_individual());
}

individual genetic_algorithm::create_individual()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, 100);

	player_weight_t weights{};
	for (int i{}; i < number_of_rules; ++i)
		weights.push_back(dist(gen));

	return individual(weights);
}

void genetic_algorithm::simulate()
{
	while (generation_id < number_of_generations)
	{
		++generation_id;
		
		std::vector<individual> new_population{};

		for (int i{}; i < population_size; i += 2)
		{
			auto x = select_individual();
			auto y = select_individual();

			auto res = reproduce(x, y);
			//ADD mutation
			new_population.emplace_back(res.first);
			new_population.emplace_back(res.second);
		}

		population = new_population;
	
	}
}

individual genetic_algorithm::select_individual()
{
	//Fitness based selection
	return population[0];
}

std::pair<individual, individual> genetic_algorithm::reproduce(const individual& x, const individual& y)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, x.state.size() - 2);

	unsigned c = static_cast<unsigned>(dist(gen));
	
	player_weight_t i1 = player_weight_t(x.state.cbegin(), x.state.cbegin() + static_cast<player_weight_t::difference_type>(c));
	player_weight_t i2 = player_weight_t(y.state.cbegin(), y.state.cbegin() + static_cast<player_weight_t::difference_type>(c));

	for (std::size_t i{c + 1}; i < x.state.size(); ++i)
	{
		i1.push_back(y.state[i]);
		i2.push_back(x.state[i]);
	}

	return std::make_pair(individual{ i1 }, individual{ i2 });
}