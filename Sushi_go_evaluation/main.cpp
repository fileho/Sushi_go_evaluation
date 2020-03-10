#include "game.h"
#include "player.h"
#include "genetic_algorithm.h"
#include "simulated_annealing.h"
#include "MCTS.h"


#include <iostream>
#include <chrono>


int main()
{
	std::vector<int> rewards{ 0,0,0 };
	for (size_t i = 0; i < 100; i++)
	{
		std::vector<player_t> players{};
		players.emplace_back(std::make_unique<MC_player>(1000, 1));
		players.emplace_back(std::make_unique<MC_player>(100, 1));
		players.emplace_back(std::make_unique<MC_player>(1000, 20));
		game game{ std::move(players) };
		auto res = game.play_game();
		rewards[0] += res[0];
		rewards[1] += res[1];
		rewards[2] += res[2];
		if (!(i % 10))
			std::cout << "Complete: " << i / 100.0 << "\n";
	}

	for (auto&& x : rewards)
		std::cout << x << "\n";


	return 0;
}