#ifndef GAME_H
#define GAME_H



#include "player.h"
#include "deck.h"

#include <vector>

const int number_of_rules{ 20 };

class game
{
public:
	game(std::vector<std::unique_ptr<base_player>>&& player) : players{ std::move(player) } {};
	std::vector<int> play_game();
private:
	std::vector<std::unique_ptr<base_player>> players;
	deck deck_{};
	unsigned int player_index{};

	void swap_hands();
	void deal_cards();
	void add_points();
	void puddings();
	void maki(std::vector<std::pair<std::size_t, int>>&& maki_rolls);
};

int evaluate(player_weight_t state);


#endif // !GAME_H