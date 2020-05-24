#include "game.h"
#include "player.h"
#include "genetic_algorithm.h"
#include "simulated_annealing.h"
#include "MCTS.h"


#include <iostream>
#include <fstream>


int main()
{
	const int number_of_games = 50;
	std::vector<double> rewards{ 0, 0, 0 };

	for (size_t i = 0; i < number_of_games; ++i)
	{
		std::vector<player_t> players{};
	//	players.emplace_back(std::make_unique<DUCT_player>(2000, 3, 0.4, eval_type::sigmoid2, true, playout::weighted));
		players.emplace_back(std::make_unique<DUCT_player>(500, 5, 0.4, eval_type::sigmoid2, true, playout::random));
		players.emplace_back(std::make_unique<DUCT_player>(500, 10, 0.3, eval_type::sigmoid2, true, playout::random));
	//	players.emplace_back(std::make_unique<Cheating_player>(100));
	//	players.emplace_back(std::make_unique<random_player>());
	//	players.emplace_back(std::make_unique<random_player>());
	//	players.emplace_back(std::make_unique<Rule_player>());
	//	players.emplace_back(std::make_unique<Rule_player>());
		players.emplace_back(std::make_unique<Rule_player>());
	//	players.emplace_back(std::make_unique<Rule_player>());

		game game{ std::move(players) };

		auto res = game.play_game();

		if (res[0] > res[1])
			++rewards[0];
		else if (res[1] > res[0])
			++rewards[1];
		else
			++rewards[2];

		if (!(i % 100))
			std::cout << "Complete: " << i / static_cast<double>(number_of_games) << "\n";
	}
	std::cout << "\n";

	std::ofstream file{ "results.txt", std::ios::app };

	for (auto&& x : rewards)
	{
		std::cout << x << "\n";
		file << x << "\n";
	}
	file << "\n\n";
	

	return 0;
}