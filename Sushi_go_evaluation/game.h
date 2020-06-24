#ifndef GAME_H
#define GAME_H



#include "player.h"
#include "deck.h"

#include <vector>

const int number_of_rules{ 16 };

class game
{
public:
	game(std::vector<std::unique_ptr<base_player>>&& player) : players{ std::move(player) } {};
	std::vector<double> play_game();
private:
	std::vector<std::unique_ptr<base_player>> players;
	deck deck_{};
	unsigned int player_index{};

	void swap_hands();
	void deal_cards();
	void add_points();
	void puddings();
	void maki(const std::vector<int>& maki_rolls);
};

// Evaluation for GA
double evaluate(player_weight_t state, player_weight_t opponent);
double evaluate(player_weight_t state);

#endif // !GAME_H