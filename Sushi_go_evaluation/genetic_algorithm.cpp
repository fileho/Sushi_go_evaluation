#include "genetic_algorithm.h"

#include <random>
#include <iostream>
#include <numeric>

genetic_algorithm::genetic_algorithm()
	: importance(population_size, 0)
{
	for (int i{}; i < population_size; ++i)
		population.emplace_back(create_individual());
}

individual genetic_algorithm::create_individual()
{

	std::uniform_real_distribution<> dist(0, max_value);

	player_weight_t weights{};
	for (int i{}; i < number_of_rules; ++i)
		weights.push_back(dist(random_generator));

	return individual(weights);
}

individual genetic_algorithm::mutate(player_weight_t state)
{
	std::uniform_int_distribution<std::size_t> dist(0, state.size() - 1);
	std::size_t c = dist(random_generator);
	std::uniform_real_distribution<>d2(-5, 5);
	double val = d2(random_generator);
	
	state[c] += val;
	if (state[c] < 0)
		state[c] = 0;
	else if (state[c] > max_value)
		state[c] = max_value;
	return individual{ state };
}

player_weight_t genetic_algorithm::simulate()
{
	while (generation_id < number_of_generations)
	{
		++generation_id;
		
		std::vector<individual> new_population{};

		std::sort(population.begin(), population.end(), [](individual a, individual b) {return a.value > b.value; });

		if(!(generation_id % 5))
			opponent = population[0].state;


		for (int i{}; i < population_size; i += 2)
		{
			auto x = select_individual();
			auto y = select_individual();

			auto res = reproduce(x, y);

			new_population.emplace_back(res.first);
			new_population.emplace_back(res.second);
		}
		
		auto result = evaluate(population[0].state);
		double sum = std::accumulate(population.cbegin(), population.cend(), 0.0, [](double val, individual i) { return val + i.value; });

		std::cout << generation_id << " " << population[0].value << " " << sum / population.size() << " " << result << "\n";
	/*	for (auto&& x : population[0].state)
			std::cout << x << " ";
		std::cout << "\n";*/
		population = new_population;
	
	}
	std::sort(population.begin(), population.end(), [](individual a, individual b) {return a.value > b.value; });

	return population[0].state;
}

individual genetic_algorithm::select_individual()
{
	//Fitness based selection
	const double penalty = 30;

	double sum{};
	for (auto&& i : population)
		sum += std::max(i.value - penalty, 0.0);

	std::uniform_real_distribution<>dist(0, sum);
	auto val = dist(random_generator);

	sum = population[0].value;	
	std::size_t index{};

	while (sum < val)
	{
		++index;
		sum += std::max(population[index].value - penalty, 0.0);
	}

	return population[index];
}

std::pair<individual, individual> genetic_algorithm::reproduce(const individual& x, const individual& y)
{
	std::uniform_int_distribution<std::size_t> dist(1, x.state.size() - 2);

	std::size_t c = static_cast<std::size_t>(dist(random_generator));

	player_weight_t i1 = player_weight_t(x.state.cbegin(), x.state.cbegin() + static_cast<player_weight_t::difference_type>(c));
	player_weight_t i2 = player_weight_t(y.state.cbegin(), y.state.cbegin() + static_cast<player_weight_t::difference_type>(c));

	for (std::size_t i{c}; i < x.state.size(); ++i)
	{
		i1.push_back(y.state[i]);
		i2.push_back(x.state[i]);
	}



	return std::make_pair(mutate(i1), mutate(i2));
}

