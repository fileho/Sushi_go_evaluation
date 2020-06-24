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

int base_player::maki_count() const
{
	int count{};
	for (auto&& c : hand)
	{
		if (c->get_card() == card_type::maki_roll_3)
			count += 3;
		else if (c->get_card() == card_type::maki_roll_2)
			count += 2;
		else if (c->get_card() == card_type::maki_roll_1)
			++count;
	}
	return count;
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

	std::uniform_int_distribution<> dist(0, hand.size() - 1);

	selected.push_back(dist(random_generator));
}


void random_player::play()
{
	selected.clear();


	std::uniform_int_distribution<unsigned> dist(0, hand.size() - 1);

	selected.push_back(dist(random_generator));
}


// MCTS

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

DUCT_player::DUCT_player(std::size_t silumations, std::size_t determinizations, double UCT_const, eval_type type, eval_pudding diff_puddings, playout playout_type)
{
	mcts.reset(new MCTS(silumations, determinizations, UCT_const, type, diff_puddings, playout_type));
}


void DUCT_player::play()
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


// Works the same way as normal MCTS player but updates hands of all players before first move so it has a perfect information

Cheating_player::Cheating_player(std::size_t silumations, double UCT_const, eval_type type, eval_pudding diff_pudding, playout playout_type)
	: mcts{ std::make_unique<MCTS>(silumations, UCT_const, type, diff_pudding, playout_type) } { ; }

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
	else
		mcts->cheat_new_set();
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

void Genetic_player::play()
{
	selected.clear();

	// Only one action
	if (hand.size() == 1)
	{
		selected.push_back(0);
		return;
	}

	if (hand.size() == 2 && played_list.chopsticks)
	{
		selected.push_back(0);
		selected.push_back(1);
		return;
	}

	std::vector<double> values{};
	double sum{};

	for (std::size_t i{}; i < weights.size(); ++i)
	{
		if (check(static_cast<Rules>(i)))
			values.push_back(weights[i]);
		else
			values.push_back(0);
		sum += values[values.size() - 1];
	}

	// Total weights == 0
	if (sum == 0)
	{
		selected.push_back(0);
		return;
	}

	std::uniform_real_distribution<>dist(0, sum);
	auto val = dist(random_generator);

	std::size_t index = 0;
	sum = values[index];

	while (sum < val)
	{
		++index;
		sum += values[index];
	}

	use(static_cast<Rules>(index));
}

void Genetic_player::update(const std::vector<player_t>& players, std::size_t index)
{
	int maxMaki{};
	int maxPuddings{};
	int minPuddings{20};
	
	for (size_t i = 0; i < players.size(); ++i)
	{
		if (i == index)
			continue;
		maxPuddings = std::max(maxPuddings, players[i]->puddings);
		minPuddings = std::min(minPuddings, players[i]->puddings);
		maxMaki = std::max(maxMaki, players[i]->maki_count());
	}

	int pudding_count{ players[index]->puddings };
	int makis{ players[index]->maki_count() };
	
	puddingMaxDiff = pudding_count - maxPuddings;
	puddingMinDiff = pudding_count - minPuddings;
	makiDiff = makis - maxMaki;
}

bool Genetic_player::check(Rules rule) const
{
	switch (rule)
	{
	case Rules::Wasabi:
		return hasCard(card_type::wasabi) && !played_list.wasabi && hand.size() >= 7;
	case Rules::Nigiri3:
		return hasCard(card_type::nigiri_3);
	case Rules::Nigiri2:
		return hasCard(card_type::nigiri_2) && !hasCard(card_type::nigiri_3);
	case Rules::N3Was:
		return hasCard(card_type::nigiri_3) && played_list.wasabi;
	case Rules::N2Was:
		return hasCard(card_type::nigiri_2) && !hasCard(card_type::nigiri_3) && played_list.wasabi;
	case Rules::N1Was:
		return hasCard(card_type::nigiri_1) && !hasCard(card_type::nigiri_3) && !hasCard(card_type::nigiri_2) && played_list.wasabi;
	case Rules::TempOdd:
		return hasCard(card_type::tempura) && played_list.tempura % 2;
	case Rules::TempEven:
		return hasCard(card_type::tempura) && !(played_list.tempura % 2);
	case Rules::TempLot:
		return countCards(card_type::tempura) >= 3;
	case Rules::Dumpl:
	{
		auto count = countCards(card_type::dumplings);
		return count && count + played_list.dumplings >= 3;
	}
	case Rules::Sashimi:
	{
		auto count = countCards(card_type::sashimi);
		return count && count + played_list.sashimi >= 3;
	}
	case Rules::PuddMax:
		return hasCard(card_type::pudding) && std::abs(puddingMaxDiff) <= 1;
	case Rules::PuddMin:
		return hasCard(card_type::pudding) && std::abs(puddingMinDiff) <= 1;
	case Rules::Maki3:
		return hasCard(card_type::maki_roll_3) && std::abs(makiDiff) < 3;
	case Rules::Maki2:
		return hasCard(card_type::maki_roll_2) && !hasCard(card_type::maki_roll_3) && std::abs(makiDiff) < 3;
	case Rules::Maki1:
		return hasCard(card_type::maki_roll_1) && !hasCard(card_type::maki_roll_1) && !hasCard(card_type::maki_roll_2) &&
			std::abs(makiDiff) < 3;
	default:
		break;
	}
	return false;
}

void Genetic_player::use(Rules rule)
{
	card_type type{};

	// Selects card
	switch (rule)
	{
	case Rules::Wasabi:
		type = card_type::wasabi;
		break;
	case Rules::Nigiri3:
		type = card_type::nigiri_3;
		break;
	case Rules::Nigiri2:
		type = card_type::nigiri_2;
		break;
	case Rules::N3Was:
		type = card_type::nigiri_3;
		break;
	case Rules::N2Was:
		type = card_type::nigiri_2;
		break;
	case Rules::N1Was:
		type = card_type::nigiri_1;
		break;
	case Rules::TempOdd:
		type = card_type::tempura;
		break;
	case Rules::TempEven:
		type = card_type::tempura;
		break;
	case Rules::TempLot:
		type = card_type::tempura;
		break;
	case Rules::Dumpl:
		type = card_type::dumplings;
		break;
	case Rules::Sashimi:
		type = card_type::sashimi;
		break;
	case Rules::PuddMax:
		type = card_type::pudding;
		break;
	case Rules::PuddMin:
		type = card_type::pudding;
		break;
	case Rules::Maki3:
		type = card_type::maki_roll_3;
		break;
	case Rules::Maki2:
		type = card_type::maki_roll_2; 
		break;
	case Rules::Maki1:
		type = card_type::maki_roll_1;
		break;
	default:
		break;
	}

	selected.push_back(getIndex(type));
}

bool Genetic_player::hasCard(card_type type) const
{
	for (auto&& c : hand)
	{
		if (c->get_card() == type)
			return true;
	}
	return false;
}

int Genetic_player::countCards(card_type type) const
{
	int result{};
	for (auto&& c : hand)
	{
		if (c->get_card() == type)
			++result;
	}
	return result;
}

std::size_t Genetic_player::getIndex(card_type type)
{
	for (std::size_t i{}; i < hand.size(); ++i)
	{
		if (hand[i]->get_card() == type)
			return i;
	}
	return std::size_t();
}



EXP3_player::EXP3_player(std::size_t silumations, std::size_t determinizations, double UCT_const, eval_type type, eval_pudding diff_puddings, playout playout_type)
{
	mcts.reset(new MCTS(silumations, determinizations, UCT_const, type, diff_puddings, playout_type));
}

void EXP3_player::play()
{
	selected.clear();

	mcts->determize();

	auto res = mcts->Exp3_best_move();

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
