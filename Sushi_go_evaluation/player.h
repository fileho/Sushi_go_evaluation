#ifndef PLAYER_H
#define PLAYER_H


#include "card.h"
#include "MCTS.h"

#include <vector>
#include <memory>

class base_player;
class MCTS;
enum class eval_type;
enum class playout;
enum class eval_pudding;

typedef std::vector<double> player_weight_t;
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
	// Returns maki points
	int maki_count() const;

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
	void update(const std::vector<player_t>& player, std::size_t index) override;
	void start_set() override;
	void add_points(const std::vector<player_t>& player, std::size_t index) override;

	std::unique_ptr<MCTS> mcts;
protected:
	unsigned set_index{};
};

class DUCT_player : public MC_player
{
public:
	DUCT_player(std::size_t silumations, std::size_t determinizations, double UCT_const, eval_type type, eval_pudding different_puddings, playout playout_type);
	void play() override;
};

class EXP3_player : public MC_player
{
public:
	EXP3_player(std::size_t silumations, std::size_t determinizations, double UCT_const, eval_type type, eval_pudding different_puddings, playout playout_type);
	void play() override;
};

class Cheating_player : public base_player
{
public:
	Cheating_player(std::size_t silumations, double UCT_const, eval_type type, eval_pudding diff_pudding, playout playout_type);
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

// Rules for genetic player
enum class Rules{ Wasabi, Nigiri3, Nigiri2, N3Was, N2Was, N1Was, TempOdd, TempEven, TempLot, Dumpl, Sashimi, PuddMax, PuddMin, Maki3, Maki2, Maki1  };

class Genetic_player : public base_player
{
public:
	Genetic_player(player_weight_t weight);
	void play() override;
	void update(const std::vector<player_t>&, std::size_t) override;
private:

	player_weight_t weights;
	int puddingMaxDiff{};
	int puddingMinDiff{};
	int makiDiff{};
	bool check(Rules rule) const;
	void use(Rules rule);
	bool hasCard(card_type type) const;
	int countCards(card_type type) const;
	std::size_t getIndex(card_type type);
};

#endif // !PLAYER_H