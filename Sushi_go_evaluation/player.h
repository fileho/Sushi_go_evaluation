#ifndef PLAYER_H
#define PLAYER_H


#include "card.h"
#include "MCTS.h"

#include <vector>
#include <memory>

class base_player;
class MCTS;
enum class eval_type;

typedef std::vector<int> player_weight_t;
typedef std::unique_ptr<base_card> card_t;
typedef std::unique_ptr<base_player> player_t;


class base_player
{
public:
	virtual ~base_player() noexcept = default;
	virtual void play() = 0;
	void play_cards();
	int points() const;
	// Funcions for updating information
	virtual void update(const std::vector<player_t>&, std::size_t) { return; };
	virtual void start_set() { return; };
	virtual void add_points(const std::vector<player_t>&, std::size_t) { return; };
	// Fuction for cheating MCTS
	virtual void cheat(const std::vector<player_t>&, std::size_t) { return; };

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
	MC_player(std::size_t silumations = 100, std::size_t determinizations = 1, double UCT_const = 1);
	MC_player(std::size_t silumations, std::size_t determinizations, double UCT_const, eval_type type);
	void play() override;
	void update(const std::vector<player_t>& player, std::size_t index) override;
	void start_set() override;
	void add_points(const std::vector<player_t>& player, std::size_t index) override;

	std::unique_ptr<MCTS> mcts;
private:
	unsigned set_index{};
};


class Cheating_player : public base_player
{
public:
	Cheating_player(std::size_t silumations = 100, double UCT_const = 1);
	Cheating_player(std::size_t silumations, double UCT_const, eval_type type);
	void play() override;
	void update(const std::vector<player_t>& player, std::size_t index) override;
	void start_set() override;
	void add_points(const std::vector<player_t>& player, std::size_t index) override;
	void cheat(const std::vector<player_t>& player, std::size_t index) override;

	std::unique_ptr<MCTS> mcts;
private:
	unsigned set_index{};
};


class Rule_player : public base_player
{
public:
	void play() override;
private:
	int get_points(card_type type);
};


class Genetic_player : public base_player
{
public:
	Genetic_player(player_weight_t weight);

private:
	player_weight_t weights;
};

#endif // !PLAYER_H