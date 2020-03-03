#pragma once

#include "player.h"

#include <vector>
#include <memory>
#include <random>
#include <algorithm>


// Implementation of determined MCTS using DUCT heuristic
// Determined: creates multiple consistent card deals to approximate game with partial information and covgerts it into game with perfect information
// UCT: upper confidance bound for trees heuristic which selects nodes for expansion
// DUCT: decoupled UCT: handes simultanious moves, each player has his own values of UCT for his actions, each player selects move individualy

// tuning UCT parametr for DUCT
const float UCT_const = 1;

// std::random_device MCTS_rd;


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


class MCTS_action_data
{
public:
	MCTS_action_data(unsigned action_index) : action{ action_index } { ; }
	action_t action;
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
	MCTS_player preform_action() const;
	action_list_t generate_actions() const;
	

	std::size_t UCT(double visit_count); // finds best action according to UCT
	
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
	std::vector<double> evaluate() const;
	std::vector<MCTS_player> swap_hands(std::vector<MCTS_player>& new_players) const;
	std::vector<MCTS_player> tree_node() const;
	MCTS_node new_node();
	std::size_t table_index() const;
private:
	std::vector<int> maki(std::vector<std::pair<std::size_t, int>>&& maki_rolls) const;
};

typedef std::size_t MCTS_card_t;

class MCTS_deck
{
public:
	void create_deck(const std::vector<MCTS_card_t>& played);
	MCTS_card_t draw();
	void shuffle();
private:
	std::vector<MCTS_card_t> deck{};
};


class MCTS
{
public:

	void generete_root();
	std::unique_ptr<MCTS_node> root = nullptr;
	void find_best_move();
	void init_players(const std::vector<card_t>& player);
	void determize();
private:
	void simulate_game();
	MCTS_deck deck{};
	std::size_t round_index{ 1 };
	card_list_t played_list{};
	std::vector<MCTS_player> players{};
};