#include "game.h"
#include "player.h"
#include "genetic_algorithm.h"
#include "MCTS.h"


#include <iostream>
#include <fstream>


int main()
{
/*	genetic_algorithm ga{};
	auto ind = ga.simulate();

	for (auto&& c : ind)
		std::cout << c << " ";
	std::cout << "\n";*/
	std::vector<double> ind{ 99.4683, 88.5946, 2.38302, 74.0515, 43.7159, 0, 94.5184, 0, 10.4859, 65.2681, 95.6682, 2.58068, 3.66796, 1.99055, 0, 77.3228 };
	const int number_of_games = 100;
	std::vector<double> rewards{ 0, 0, 0, 0, 0 };


	for (size_t i = 0; i < number_of_games; ++i)
	{
		std::vector<player_t> players{};
		players.emplace_back(std::make_unique<DUCT_player>(2000, 3, 0.4, eval_type::sigmoid1x, eval_pudding::scaled_plus, playout::random));
	//	players.emplace_back(std::make_unique<EXP3_player>(2000, 3, 0.2, eval_type::sigmoid1x, true, playout::random));
		players.emplace_back(std::make_unique<Genetic_player>(ind));
	//	players.emplace_back(std::make_unique<DUCT_player>(2000, 3, 0.4, eval_type::sigmoid1x, eval_pudding::scaled, playout::random));
	//	players.emplace_back(std::make_unique<Cheating_player>(100)11);
	//	players.emplace_back(std::make_unique<random_player>());
	//	players.emplace_back(std::make_unique<random_player>());
		players.emplace_back(std::make_unique<Rule_player>());
		players.emplace_back(std::make_unique<Rule_player>());
		players.emplace_back(std::make_unique<Rule_player>());
	//	players.emplace_back(std::make_unique<Rule_player>());

		game game{ std::move(players) };

		auto res = game.play_game();

		for (size_t j = 0; j < res.size(); ++j)
			rewards[j] += res[j];

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