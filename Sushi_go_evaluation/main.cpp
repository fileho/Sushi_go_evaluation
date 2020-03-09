#include "game.h"
#include "player.h"
#include "genetic_algorithm.h"
#include "simulated_annealing.h"
#include "MCTS.h"

#include <iostream>
#include <chrono>
#include <thread>


int main()
{
	std::vector<player_t> players{};

	for (size_t i = 0; i < 5; i++)
	{
		players.emplace_back(std::make_unique<random_player>());
	}
	game game{ std::move(players) };
	auto res = game.play_game();
	
	for (auto&& r : res)
		std::cout << r << "\n";

/* MCTS
	player_weight_t w{};

	deck deck{};
	base_player pl{ w };

	for (int i{}; i < 10; ++i)
		pl.hand.emplace_back(deck.draw());

	MCTS mcts{};

	mcts.init_players(pl.hand);
	mcts.determize();
	mcts.find_best_move();
	*/

	return 0;
}