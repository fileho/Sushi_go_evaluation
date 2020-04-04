#include "player.h"

#include <numeric>
#include <random>
#include <iostream>


void base_player::play_cards()
{
	if (selected.size() == 2)
	{
		--played_list.chopsticks;
		// Handles corrent order of played cards 
		// Places highest value nigiri on wasabi and plays wasabi before nigiri
		if (hand[selected[0]]->priority() >= hand[selected[1]]->priority())
		{
			hand[selected[0]]->play(played_list);
			hand[selected[1]]->play(played_list);
		}
		else
		{
			hand[selected[1]]->play(played_list);
			hand[selected[0]]->play(played_list);
		}
	}
	else
		hand[selected[0]]->play(played_list);

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
	std::uniform_int_distribution<unsigned> dist(0, hand.size() - 1);

	selected.push_back(dist(gen));
}


// MCTS

MC_player::MC_player(std::size_t silumations, std::size_t determinizations, double UCT_const)
	: mcts{ std::make_unique<MCTS>(silumations, determinizations, UCT_const, eval_type::point_diff) } { ; }

MC_player::MC_player(std::size_t silumations, std::size_t determinizations, double UCT_const, eval_type type)
	: mcts{ std::make_unique<MCTS>(silumations, determinizations, UCT_const, type) } { ; }


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


// Works the same way as normal MCTS player but updates hands of all players before first move so it has a perfect information

Cheating_player::Cheating_player(std::size_t silumations, double UCT_const)
	: mcts{ std::make_unique<MCTS>(silumations, UCT_const, eval_type::point_diff) } { ; }

Cheating_player::Cheating_player(std::size_t silumations, double UCT_const, eval_type type)
	: mcts{ std::make_unique<MCTS>(silumations, UCT_const, type) } { ; }

void Cheating_player::play()
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

void Cheating_player::update(const std::vector<player_t>& player, std::size_t index)
{
	mcts->update(player, index);
}

void Cheating_player::start_set()
{
	++set_index;
	if (set_index == 1)
		mcts->init_players(hand);
}

void Cheating_player::add_points(const std::vector<player_t>& player, std::size_t index)
{
	mcts->add_points(player, index);
}

void Cheating_player::cheat(const std::vector<player_t>& player, std::size_t index)
{
	mcts->update(player, index);
}

void Rule_player::play()
{
	selected.clear();

	std::vector<unsigned> cards(12, 0);

	for (auto&& c : hand)
		++cards[c->MCTS()];

	if (!played_list.chopsticks || hand.size() == 1 || true)
	{
		std::pair<int, unsigned> play{};

		for (size_t i = 0; i < hand.size(); ++i)
		{
			auto val = get_points(hand[i]->get_card());
			if (val > play.first)
			{
				play.first = val;
				play.second = i;
			}
		}

		selected.push_back(play.second);
	}

	// TODO chopsticks
}

int Rule_player::get_points(card_type type)
{
	switch (type)
	{
	case card_type::tempura:
		if (played_list.tempura % 2)
			return 20;
		return 10;
	case card_type::chopsticks:
		return 8;
	case card_type::maki_roll_1:
		return 3;
	case card_type::maki_roll_2:
		return 6;
	case card_type::maki_roll_3:
		return 8;
	case card_type::wasabi:
		return 15;
	case card_type::sashimi:
		if (played_list.sashimi % 3 == 2)
			return 35;
		return 8;
	case card_type::pudding:
		return 14;
	case card_type::nigiri_1:
		return 5;
	case card_type::nigiri_2:
		if (played_list.wasabi)
			return 20;
		return 10;
	case card_type::nigiri_3:
		if (played_list.wasabi)
			return 30;
		return 15;
	case card_type::dumplings:
		return 10 + 3 * played_list.dumplings;
	}
	return 1;
}

Genetic_player::Genetic_player(player_weight_t weight)
	: weights{ weight } { ; }
