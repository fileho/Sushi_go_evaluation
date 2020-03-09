#pragma once

#include "game.h"

#include <cmath>

const std::size_t maxT{ static_cast<std::size_t>(std::pow(10,6)) };

class simulated_annealing
{
public:
	explicit simulated_annealing(const player_weight_t& inicial_state) : state{ inicial_state } { /*value = evaluate(inicial_state);*/; }
	void simulate();

private:
	player_weight_t state;
	int value = 0;

	float schedule(std::size_t t);
	player_weight_t neighbour();
};