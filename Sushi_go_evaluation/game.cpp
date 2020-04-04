#include "game.h"

#include <algorithm>
#include <iostream>

const unsigned int set_count = 3;


// Single game simulation
std::vector<int> game::play_game()
{
	for (std::size_t set_index{}; set_index < set_count; ++set_index)
	{
		deal_cards();

		for (auto&& p : players)
			p->start_set();
		
		for (size_t i = 0; i < players.size(); ++i)
			players[i]->cheat(players, i);

		while (players[0]->hand.size())
		{
			for (auto&& p : players)
				p->play();
			for (auto&& p : players)
				p->play_cards();
			swap_hands();
			
			for (size_t i = 0; i < players.size(); ++i)
				players[i]->update(players, i);
		}

		add_points();

		for (auto&& p : players)
			p->played_list = card_list{};

		for (size_t i = 0; i < players.size(); ++i)
			players[i]->add_points(players, i);		
	}

	puddings();

	std::vector<int> results{};

	for (auto&& p : players)
		results.push_back(p->points());

	return results;
}

void game::deal_cards()
{
	const std::vector<int> card_amounts{ 0, 0, 10, 9, 8, 7 };

	int card_count{ card_amounts[players.size()] };

	for (auto&& x : players)
	{
		for (int i{}; i < card_count; ++i)
			x->hand.emplace_back(deck_.draw());
	}
}

void game::swap_hands()
{
	auto tmp = std::move(players[players.size() - 1]->hand);

	for (std::size_t i{ players.size() - 1}; i > 0; --i)
		players[i]->hand = std::move(players[i - 1]->hand);

	players[0]->hand = std::move(tmp);
}

void game::add_points()
{
	const std::vector<int> dumlp{ 0, 1, 3, 6, 10, 15, 15, 15, 15, 15, 15 };
	std::vector<int> maki_{};
	for (std::size_t i{}; i < players.size(); ++i)
	{
		const auto& y = players[i]->played_list;
		players[i]->round_points.push_back(y.nigiri_1 + 2 * y.nigiri_2 + 3 * y.nigiri_3 + y.tempura / 2 * 5 + y.sashimi / 3 * 10 + \
			3 * y.wasabi_nigiri_1 + 6 * y.wasabi_nigiri_2 + 9 * y.wasabi_nigiri_3 + dumlp[static_cast<std::size_t>(y.dumplings)]);
		maki_.push_back(y.maki_roll_1 + 2 * y.maki_roll_2 + 3 * y.maki_roll_3);
		players[i]->puddings += y.pudding;
	}
	maki(maki_);
}

void game::maki(const std::vector<int>& maki_rolls)
{
	int max{};
	std::vector<unsigned> index{};

	// Add 6 points to all players with highest amount of maki rolls
	for (size_t i = 0; i < maki_rolls.size(); ++i)
	{
		if (maki_rolls[i] > max)
		{
			index.clear();
			max = maki_rolls[i];
			index.push_back(i);
		}
		else if (maki_rolls[i] == max && max > 0)
			index.push_back(i);
	}

	for (auto&& i : index)
		players[i]->round_points[players[i]->round_points.size() -1] += 6;

	index.clear();

	int second_max{};
	// Add 3 points to all players with second highest amount of maki rolls, must have at least 1 maki
	for (size_t i = 0; i < maki_rolls.size(); ++i)
	{
		if (maki_rolls[i] > second_max && maki_rolls[i] < max)
		{
			index.clear();
			second_max = maki_rolls[i];
			index.push_back(i);
		}
		else if (maki_rolls[i] == second_max && second_max > 0)
			index.push_back(i);
	}
	for (auto&& i : index)
		players[i]->round_points[players[i]->round_points.size() - 1] += 3;
}

void game::puddings()
{
	int max_puddings{ 0 };
	for (auto&& x : players)
	{
		if (x->puddings > max_puddings)
			max_puddings = x->puddings;
	}

	int min_puddings{ 20 };
	// Don't subtract points for least points in two player game
	if (players.size() > 2)
	{
		for (auto&& x : players)
		{
			if (x->puddings < min_puddings)
				min_puddings = x->puddings;
		}
	}

	for (auto&& x : players)
	{
		if (x->puddings == max_puddings)
			x->round_points.push_back(6);
		else if (x->puddings == min_puddings)
			x->round_points.push_back(-6);
		else
			x->round_points.push_back(0);
	}
}
/*
int evaluate(player_weight_t state)
{
	int ret = 0;

	std::vector<player> players{};
	players.emplace_back(player{ state });

	player_weight_t fill{};
	for (int i{}; i < 4; ++i)
		players.emplace_back(player{ fill });

	for (int i{}; i < 100; ++i)
	{
		auto copy = players;
		game game{ std::move(players) };
		auto results = game.play_game();
		ret += results[0];
	}

	return ret;
}*/