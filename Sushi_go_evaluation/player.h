#pragma once

#include "card.h"

#include <vector>
#include <memory>

class base_player;

typedef std::vector<int> player_weight_t;
typedef std::unique_ptr<base_card> card_t;
typedef std::unique_ptr<base_player> player_t;


class base_player
{
public:
	virtual void play() = 0;
	void play_cards();
	int points() const;
	virtual void update(const std::vector<player_t>&, std::size_t) { return; };
	virtual ~base_player() noexcept = default;


	std::vector<unsigned int> selected{};
	std::vector<card_t> hand{};
	std::vector<card_t> played_cards{};
	int puddings{};
	card_list played_list{};
	std::vector<int> round_points{};
	bool chopsticks{};

protected:
	void update_card();
};

class random_player : public base_player
{
public:
	void play() override;
};
