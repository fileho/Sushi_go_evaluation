#include "player.h"

#include <numeric>
#include <random>


void player::play_cards()
{
	for (auto&& x : selected)
		hand[x]->play(played_list);

	update_card();
}

int player::points() const
{
	return std::accumulate(round_points.cbegin(), round_points.cend(), 0);
}


void player::update_card()
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



player::player(const player& p)
{
	player_weights = p.player_weights;
}

void player::play()
{
	selected.clear();

	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_int_distribution<> dist(0, hand.size() - 1);

	selected.push_back(dist(gen));
}