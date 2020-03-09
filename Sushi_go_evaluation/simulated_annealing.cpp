#include "simulated_annealing.h"

#include <random>

void simulated_annealing::simulate()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(0, 1);
	for (std::size_t t = 1; t < maxT; ++t)
	{
		auto T = schedule(t);
		auto next = neighbour();
	//	int next_value = evaluate(next);
		int next_value = 0;

		int E = next_value - value;

		if (E > 0 || dist(gen) < std::exp(E/T))
		{
			state = next;
			value = next_value;
		}
	}
}


float simulated_annealing::schedule(std::size_t t)
{
	return static_cast<float>(maxT) / (t + 1);
}

player_weight_t simulated_annealing::neighbour()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(-5, 5);
	std::uniform_int_distribution<> variable(0, state.size() - 1);

	auto new_state = state;
	auto index = static_cast<std::size_t>(variable(gen));
	new_state[index] += dist(gen);
	if (new_state[index] < 0)
		new_state[index] = 0;

	return new_state;
}