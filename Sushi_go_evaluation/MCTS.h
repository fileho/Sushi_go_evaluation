#pragma once

#include "player.h"

#include <vector>
#include <memory>
#include <random>
#include <algorithm>

class base_player;

typedef std::unique_ptr<base_card> card_t;
typedef std::unique_ptr<base_player> player_t;

// Implementation of determined MCTS using DUCT heuristic
// Determined: creates multiple consistent card deals to approximate game with partial information and covgerts it into game with perfect information
// UCT: upper confidance bound for trees heuristic which selects nodes for expansion
// DUCT: decoupled UCT: handes simultanious moves, each player has his own values of UCT for his actions, each player selects move individualy

enum class eval_type { win, point_diff, higher_base };


typedef std::vector<unsigned int> card_list_t;
typedef unsigned action_t;



const std::size_t Nigiri3_i = 0;
const std::size_t Nigiri2_i = 1;
const std::size_t Nigiri1_i = 2;

const std::size_t Maki3_i = 3;
const std::size_t Maki2_i = 4;
const std::size_t Maki1_i = 5;

const std::size_t Wasabi_i = 6;
const std::size_t Wasabi_Nig3 = 12;
const std::size_t Wasabi_Nig2 = 13;
const std::size_t Wasabi_Nig1 = 14;

const std::size_t No_move = 100;


class MCTS_action_data
{
public:
	MCTS_action_data(action_t action_index, action_t action2 = No_move) : action{ action_index }, action2{ action2 } {; }
	action_t action;
	action_t action2;
	double reward = 0;
	double visit_count = 0;
};

typedef std::vector<MCTS_action_data> action_list_t;

class MCTS_player
{
public:
	MCTS_player(card_list_t hand, card_list_t played, int points = 0, int puddings = 0) :
		hand{ hand }, played{ played }, points{ points }, puddings{ puddings }{ actions = generate_actions(); }
	int points;
	int puddings;
	card_list_t hand;
	card_list_t played;
	action_list_t actions;
	action_t selected_action{};

	MCTS_player random_action();
	MCTS_player perform_action() const;
	action_list_t generate_actions() const;
	

	std::size_t UCT(double visit_count, double constant); // finds best action according to UCT

private:
	void generate_pairs(action_list_t& action_list) const;
	void smart_generete(action_list_t& action_list, unsigned i) const;
	void perform(card_list_t& new_hand, card_list_t& new_played, action_t action) const;
	
};	

class MCTS_node;

typedef std::vector<std::unique_ptr<MCTS_node>> table_t;

class MCTS_node
{
public:
	MCTS_node(MCTS_node* parent, std::vector<MCTS_player> players);
	MCTS_node(std::vector<MCTS_player> players);
	MCTS_node* parent;
	double visit_count{};
	std::vector<MCTS_player> players;
	//table of nodes according to each simultanious move
	table_t transpositional_table;

	bool is_terminal() const;
	std::vector<double> evaluate(eval_type type) const;
	std::vector<MCTS_player> swap_hands(std::vector<MCTS_player>& new_players) const;
	std::vector<MCTS_player> tree_node() const;
	MCTS_node new_node();
	std::size_t table_index() const;
private:
	std::vector<int> maki(std::vector<std::pair<std::size_t, int>>&& maki_rolls) const;
	std::vector<int> pudding(std::vector<int>& puddings) const;
};

typedef std::size_t MCTS_card_t;

class MCTS_deck
{
public:
	void create_deck(const card_list_t& played);
	MCTS_card_t draw();
	void shuffle();
private:
	std::vector<MCTS_card_t> deck{};
	std::size_t index{};
};


class MCTS
{
public:
	MCTS(std::size_t simulation_count, std::size_t deterministic_count, double UCT_const, eval_type type)
		: number_of_simulation{ simulation_count }, roots{ deterministic_count }, UCT_const{ UCT_const }, type{ type }{};
	// Cheating ctor
	MCTS(std::size_t simulation_count, double UCT_const, eval_type type)
		: number_of_simulation{ simulation_count }, roots{ 1 }, UCT_const{ UCT_const }, type{ type } { round_index = 20; }

	void generete_root(const std::vector<MCTS_player>& player, std::size_t index);
	std::pair<action_t, action_t> find_best_move();
	void init_players(const std::vector<card_t>& player);
	void new_set(const std::vector<card_t>& player);
	void determize();
	void update(const std::vector<player_t>& player, std::size_t index);
	void add_points(const std::vector<player_t>& player, std::size_t index);
private:
	void simulate_game(std::size_t index);
	void simulate_n_games(std::size_t index);
	card_list_t from_card_list(const card_list& cl);
	card_list_t get_played() const;
	void save_played();

	double UCT_const{};
	eval_type type;
	std::vector<std::unique_ptr<MCTS_node>> roots;
	MCTS_deck deck{};
	std::size_t round_index{ 1 };
	card_list_t played_list = card_list_t(12,0);
	std::size_t number_of_simulation;
	std::vector<MCTS_player> players{};
};