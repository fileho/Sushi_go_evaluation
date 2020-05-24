#ifndef MCTS_H
#define MCTS_H

#include "player.h"

#include <vector>
#include <memory>
#include <random>



class base_player;

typedef std::unique_ptr<base_card> card_t;
typedef std::unique_ptr<base_player> player_t;

// Implementation of determined MCTS using deterministic DUCT heuristic and stochastic EXP3 heuristic 
// Determined: creates multiple consistent card deals to approximate game with partial information and covgerts it into game with perfect information
// UCT: upper confidance bound for trees heuristic which selects nodes for expansion
// DUCT: decoupled UCT: handes simultanious moves, each player has his own values of UCT for his actions, each player selects move individualy

// Static random engines so we don't have to create a new one every time
static std::random_device rd{};
static std::mt19937 random_generator(rd());

enum class eval_type { win, point_diff, higher_base, sigmoid, sigmoid2, simgoid1x };
// Playout policies
enum class playout { random, weighted };


typedef std::vector<unsigned int> card_list_t;
typedef unsigned action_t;


// Const indicies
constexpr std::size_t Nigiri3_i = 0;
constexpr std::size_t Nigiri2_i = 1;
constexpr std::size_t Nigiri1_i = 2;

constexpr std::size_t Maki3_i = 3;
constexpr std::size_t Maki2_i = 4;
constexpr std::size_t Maki1_i = 5;

constexpr std::size_t Wasabi_i = 6;
constexpr std::size_t Wasabi_Nig3 = 12;
constexpr std::size_t Wasabi_Nig2 = 13;
constexpr std::size_t Wasabi_Nig1 = 14;

constexpr std::size_t No_move = 15;


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
		hand{ hand }, played{ played }, points{ points }, puddings{ puddings }{ ; }
	int points;
	int puddings;
	card_list_t hand;
	card_list_t played;
	action_list_t actions{};
	action_t selected_action{};
	double prob{}; // Probability of selecting prarticular move by EXP3, used in backpropagation

	MCTS_player random_action(playout type) noexcept;
	MCTS_player perform_action() const noexcept;
	action_list_t generate_actions() const noexcept;
	

	action_t UCT(double visit_count, double constant) noexcept; // finds best action according to UCT
	action_t exp3(double lambda) noexcept; // finds move according to EXP3

private:
	void generate_pairs(action_list_t& action_list) const noexcept;
	void smart_generete(action_list_t& action_list, unsigned i) const noexcept;
	void perform(card_list_t& new_hand, card_list_t& new_played, action_t action) const noexcept;
	
};	

class MCTS_node;

typedef std::vector<std::unique_ptr<MCTS_node>> table_t;

class MCTS_node
{
public:
	MCTS_node() = default;
	MCTS_node(MCTS_node* parent, std::vector<MCTS_player> players);
	MCTS_node(std::vector<MCTS_player> players);
	MCTS_node* parent{nullptr};
	double visit_count{};
	std::vector<MCTS_player> players;
	//table of nodes according to each simultanious move
	table_t transpositional_table;

	bool is_terminal() const noexcept;
	std::vector<double> evaluate(eval_type type, int pudding_value) const noexcept;
	std::vector<MCTS_player> swap_hands(std::vector<MCTS_player>& new_players) const noexcept;
	std::vector<MCTS_player> tree_node() const noexcept;
	MCTS_node new_node(playout type) noexcept;
	std::size_t table_index() const noexcept;
private:
	std::vector<int> maki(const std::vector<int>& maki_rolls) const noexcept;
	std::vector<int> pudding(const std::vector<int>& puddings, int value) const noexcept;
};

typedef std::size_t MCTS_card_t;

class MCTS_deck
{
public:
	void create_deck(const card_list_t& played) noexcept;
	MCTS_card_t draw() noexcept;
	void shuffle() noexcept;
private:
	std::vector<MCTS_card_t> deck{};
	std::size_t index{};
};


class MCTS
{
public:
	MCTS(std::size_t simulation_count, std::size_t deterministic_count, double UCT_const, eval_type type, bool pudding_scaling, playout playout_type)
		: number_of_simulation{ simulation_count }, roots{ deterministic_count }, tuning_constant{ UCT_const }, type{ type }, playout_type{ playout_type }{ if (pudding_scaling) pudding_value = 4; };
	// Cheating ctor
	MCTS(std::size_t simulation_count, double UCT_const, eval_type type, bool pudding_scaling, playout playout_type)
		: number_of_simulation{ simulation_count }, roots{ 1 }, tuning_constant{ UCT_const }, type{ type }, playout_type{ playout_type } { round_index = 20; if (pudding_scaling) pudding_value = 4; }

	void generete_root(const std::vector<MCTS_player>& player, std::size_t index) noexcept;
	std::pair<action_t, action_t> find_best_move() noexcept;
	std::pair<action_t, action_t> Exp3_best_move() noexcept;
	void init_players(const std::vector<card_t>& player) noexcept;
	void new_set(const std::vector<card_t>& player) noexcept;
	void determize() noexcept;
	void update(const std::vector<player_t>& player, std::size_t index) noexcept;
	void add_points(const std::vector<player_t>& player, std::size_t index) noexcept;
	void cheat_new_set() noexcept;

private:
	void simulate_game(std::size_t index) noexcept;
	void simulate_exp3(std::size_t index) noexcept;
	void simulate_n_games(std::size_t index) noexcept;
	void simulate_n_exp3(std::size_t index) noexcept;
	card_list_t from_card_list(const card_list& cl) noexcept;
	card_list_t get_played() const noexcept;
	void save_played() noexcept;

	double tuning_constant{};
	int pudding_value{ 6 };
	eval_type type;
	playout playout_type;
	std::vector<MCTS_node> roots;
	std::size_t number_of_simulation;
	std::vector<MCTS_player> players{};
	MCTS_deck deck{};
	std::size_t round_index{ 1 };
	card_list_t played_list = card_list_t(12,0);
};

#endif // !MCTS_H