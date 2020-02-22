#pragma once


#include "player.h"
#include "deck.h"

#include <vector>


class simulation
{
public:
	simulation(std::vector<player> player) : players{ player } {};
	void play_game();
private:
	std::vector<player> players;
	deck deck_{};
};

