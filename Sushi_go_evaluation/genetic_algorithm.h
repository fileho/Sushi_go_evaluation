#pragma once

#include "game.h"

static player_weight_t opponent(16, 5);

class individual
{
public:
	explicit individual(player_weight_t weights) : state{ weights } { value = evaluate(weights, opponent); };
	individual(player_weight_t weigths, double value) : state{ weigths }, value{ value }{};
	player_weight_t state;
	double value = 0;
};

const int population_size = 128;
const int number_of_generations = 300;

const double max_value = 100;

class genetic_algorithm
{
public:
	genetic_algorithm();
	player_weight_t simulate();
private:
	std::vector<individual> population{};
	int generation_id{};
	std::vector<double> importance;

	individual select_individual();
	individual create_individual();
	individual mutate(player_weight_t x);
	std::pair<individual, individual> reproduce(const individual& x, const individual& y);

};

