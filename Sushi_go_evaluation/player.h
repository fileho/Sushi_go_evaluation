#pragma once

#include "card.h"
#include "MCTS.h"

#include <vector>
#include <memory>

class base_player;
class MCTS;

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
	virtual void start_set() { return; };
	virtual void add_points(const std::vector<player_t>&, std::size_t) { return; };
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


class MC_player : public base_player
{
public:
	MC_player();
	void play() override;
	void update(const std::vector<player_t>& player, std::size_t index) override;
	void start_set() override;
	void add_points(const std::vector<player_t>& player, std::size_t index) override;

	std::unique_ptr<MCTS> mcts;
private:
	unsigned set_index{};
};