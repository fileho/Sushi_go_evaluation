#pragma once

#include "game.h"

class individual
{
public:
	explicit individual(player_weight_t weights) : state{ weights } { /*value = evaluate(weights); */ };
	player_weight_t state;
	int value = 0;
};

const int population_size = 64;
const int number_of_generations = 1000;

class genetic_algorithm
{
public:
	genetic_algorithm();
	void simulate();
private:
	std::vector<individual> population{};
	int generation_id{};

	individual select_individual();
	individual create_individual();
	std::pair<individual, individual> reproduce(const individual& x, const individual& y);

};

