#include "player.h"

#include <numeric>
#include <random>
#include <iostream>


void base_player::play_cards()
{
	for (auto&& x : selected)
		hand[x]->play(played_list);

	update_card();
}

int base_player::points() const
{
	return std::accumulate(round_points.cbegin(), round_points.cend(), 0);
}


void base_player::update_card()
{
	if (selected.size() == 2)
	{
		if (selected[0] < selected[1])
			--selected[1];

		for (auto&& x : selected)
			hand.erase(hand.cbegin() + static_cast<std::vector<card_t>::difference_type>(x));
		
		hand.emplace_back(std::make_unique<::chopsticks>());

		return;
	}
	
	hand.erase(hand.begin() + static_cast<std::vector<card_t>::difference_type>(selected[0]));
}


void base_player::play()
{
	selected.clear();

	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_int_distribution<> dist(0, hand.size() - 1);

	selected.push_back(dist(gen));
}


void random_player::play()
{
	selected.clear();

	std::random_device rd;
	std::mt19937 gen{ rd() };
	std::uniform_int<unsigned> dist(0, hand.size() - 1);

	selected.push_back(dist(gen));
}

MC_player::MC_player() : mcts{ std::make_unique<MCTS>() } { ; }


void MC_player::play()
{
	selected.clear();

	mcts->determize();

	auto res = mcts->find_best_move();

	unsigned used{ 100 };

	for (size_t i = 0; i < hand.size(); ++i)
	{
		if (hand[i]->MCTS() == res.second)
		{
			used = i;
			
			selected.push_back(i);
			break;
		}
	}
	for (size_t i = 0; i < hand.size(); ++i)
	{
		if (hand[i]->MCTS() == res.first && i != used)
		{
			selected.push_back(i);
			break;
		}
	}
}

void MC_player::update(const std::vector<player_t>& player, std::size_t index)
{
	mcts->update(player, index);
}

void MC_player::start_set()
{
	++set_index;
	if(set_index == 1)
		mcts->init_players(hand);
	else
		mcts->new_set(hand);
}

void MC_player::add_points(const std::vector<player_t>& player, std::size_t index)
{
	mcts->add_points(player, index);
}
