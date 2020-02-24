#pragma once

#include "card.h"

#include <vector>
#include <memory>


typedef std::vector<int> player_weight_t;
typedef std::unique_ptr<base_card> card_t;

class player
{
public:
	player(const player_weight_t& weights) : player_weights{ weights } { ; }
	player(const player& p);
	void play();
	void play_cards();
	int points() const;


	std::vector<unsigned int> selected{};
	std::vector<card_t> hand{};
	std::vector<card_t> played_cards{};
	int puddings{};
	card_list played_list{};
	std::vector<int> round_points{};
	bool chopsticks{};
private:
	player_weight_t player_weights;
	void update_card();
};

