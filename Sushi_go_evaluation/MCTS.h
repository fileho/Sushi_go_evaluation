#pragma once

#include <vector>
#include <memory>
#include <random>

// tuning UCT parametr for DUCT
const float UCT_const = 1;

std::random_device MCTS_rd;


typedef std::vector<int> card_list_t;
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
	MCTS_player preform_action();
	

	std::size_t UCT(double visit_count); // finds best action according to UCT
private:
	action_list_t generate_actions();
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
	//table of nodes according to each simontanious move
	table_t transpositional_table;

	std::vector<MCTS_player> swap_hands(std::vector<MCTS_player> new_players);
	std::vector<MCTS_player> tree_node();
	MCTS_node new_node();
	std::size_t table_index();
};

class MCTS
{
public:
	std::unique_ptr<MCTS_node> generete_root();
	std::unique_ptr<MCTS_node> root = nullptr;
	void simulate();
private:
	card_list_t played{};
	std::vector<MCTS_player> players{};
};