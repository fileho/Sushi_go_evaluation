#include "game.h"

#include <algorithm>
#include <iostream>

const unsigned int set_count = 3;

std::vector<int> game::play_game()
{
	for (std::size_t set_index{}; set_index < set_count; ++set_index)
	{
		deal_cards();

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
	std::vector<std::pair<std::size_t, int>> maki_{};
	for (std::size_t i{}; i < players.size(); ++i)
	{
		const auto& y = players[i]->played_list;
		players[i]->round_points.push_back(y.nigiri_1 + 2 * y.nigiri_2 + 3 * y.nigiri_3 + y.tempura / 2 * 5 + y.sashimi / 3 * 10 + \
			3 * y.wasabi_nigiri_1 + 6 * y.wasabi_nigiri_2 + 9 * y.wasabi_nigiri_3 + dumlp[static_cast<std::size_t>(y.dumplings)]);
		maki_.push_back(std::make_pair(i, y.maki_roll_1 + 2 * y.maki_roll_2 + 3 * y.maki_roll_3));
		players[i]->puddings += y.pudding;
	}
	maki(std::move(maki_));
}

void game::maki(std::vector<std::pair<std::size_t, int>>&& maki_rolls)
{
	std::sort(maki_rolls.begin(), maki_rolls.end(), [](auto&& v1, auto&& v2) { return v1.second > v2.second; });
	if (maki_rolls[1].second == maki_rolls[0].second)
	{
		int val = maki_rolls[0].second;
		for (std::size_t i{}; i < maki_rolls.size(); ++i)
		{
			if (maki_rolls[i].second == val)
				players[maki_rolls[i].first]->round_points[players[maki_rolls[i].first]->round_points.size() - 1] += 6;
			else
				return;
		}
	}
	int val = maki_rolls[1].second;
	players[maki_rolls[0].first]->round_points[players[0]->round_points.size() - 1] += 6;
	for (std::size_t i{ 1 }; i < maki_rolls.size(); ++i)
	{
		if (maki_rolls[i].second == val)
			players[maki_rolls[i].first]->round_points[players[0]->round_points.size() - 1] += 3;
		else
			return;
	}
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