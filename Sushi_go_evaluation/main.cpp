#include "game.h"
#include "player.h"
#include "genetic_algorithm.h"
#include "simulated_annealing.h"

#include <iostream>
#include <chrono>


int main()
{
	player_weight_t weights{};
	std::vector<player> players{};
	for (std::size_t i{}; i < 5; ++i)
		players.emplace_back(player(weights));

	game game(std::move(players));

	auto results = game.play_game();


	for (auto&& x : results)
		std::cout << x << "\n";

	return 0;
}