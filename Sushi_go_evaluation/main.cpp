#include "game.h"
#include "player.h"
#include "genetic_algorithm.h"
#include "simulated_annealing.h"

#include <iostream>
#include <chrono>
#include <thread>


int main()
{
	player_weight_t weights{};
	std::vector<player> players{};
	for (std::size_t i{}; i < 5; ++i)
		players.emplace_back(player(weights));

	game game(std::move(players));

	auto t1 = std::chrono::high_resolution_clock::now();
	auto results = game.play_game();
	auto t2 = std::chrono::high_resolution_clock::now();
	std::cout << std::pow(10,9)/(t2 - t1).count() << "\n";

	for (auto&& x : results)
		std::cout << x << "\n";

	return 0;
}