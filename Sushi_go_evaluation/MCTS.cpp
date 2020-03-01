#include "MCTS.h"

#include <cmath>

MCTS_player MCTS_player::random_action()
{
	std::uniform_int_distribution<std::size_t> dist(0, actions.size());

	selected_action = dist(MCTS_rd);
	return preform_action();
}

MCTS_player MCTS_player::preform_action()
{
	card_list_t new_hand = hand;
	--new_hand[selected_action];

	card_list_t new_played = played;

	// Nigiri & Wasabi
	if (selected_action >= 3 || !played[Wasabi_i])
		++new_played[selected_action];
	else
	{
		--new_played[Wasabi_i];
		if (selected_action == 0)
			++new_played[Wasabi_Nig3];
		else if (selected_action == 1)
			++new_played[Wasabi_Nig2];
		else
			++new_played[Wasabi_Nig1];
	}

	return MCTS_player{ new_hand, new_played, points, puddings };
}

std::size_t MCTS_player::UCT(double visit_count)
{
	std::size_t index{};
	double max_value{};
	for (std::size_t i{}; i < actions.size(); ++i)
	{
		if (!actions[i].visit_count)
			return i;

		double UCT = actions[i].reward + UCT_const * std::sqrt(std::log(visit_count) / actions[i].visit_count);
		if (UCT > max_value)
		{
			max_value = UCT;
			index = i;
		}
	}

	return index;
}

action_list_t MCTS_player::generate_actions()
{
	action_list_t action_list{};
	//Best Nigiri
	if (hand[Nigiri3_i])
		action_list.push_back(MCTS_action_data{ Nigiri3_i });
	else if (hand[Nigiri2_i])
		action_list.push_back(MCTS_action_data{ Nigiri2_i });
	else if (hand[Nigiri1_i])
		action_list.push_back(MCTS_action_data{ Nigiri1_i });

	//Best Maki
	if (hand[Maki3_i])
		action_list.push_back(MCTS_action_data{ Maki3_i });
	else if (hand[Maki2_i])
		action_list.push_back(MCTS_action_data{ Maki2_i });
	else if (hand[Maki1_i])
		action_list.push_back(MCTS_action_data{ Maki1_i });

	for (unsigned i{ 6 }; i < hand.size(); ++i)
	{
		if (hand[i])
			action_list.push_back(MCTS_action_data{ i });
	}
		
	return action_list;
}



MCTS_node::MCTS_node(MCTS_node* parent, std::vector<MCTS_player> players)
	: parent{ parent }, players{ players }
{
	std::size_t size = 1;
	for (auto&& p : players)
		size *= p.actions.size();
	transpositional_table = std::move(table_t(size));
}

MCTS_node::MCTS_node(std::vector<MCTS_player> players) :
	parent{ nullptr }, transpositional_table{}, players{ players }{; }


std::vector<MCTS_player> MCTS_node::swap_hands(std::vector<MCTS_player> new_players)
{
	auto tmp = std::move(new_players[new_players.size() - 1].hand);

	for (std::size_t i{ new_players.size() - 1 }; i >= 1; --i)
		new_players[i].hand = std::move(new_players[i - 1].hand);

	new_players[0].hand = std::move(tmp);

	return new_players;
}

std::vector<MCTS_player> MCTS_node::tree_node()
{
	std::vector<MCTS_player> new_players = players;
	return swap_hands(new_players);
}

MCTS_node MCTS_node::new_node()
{
	std::vector<MCTS_player> new_players{};
	for (auto&& p : players)
		new_players.emplace_back(p.random_action());
	
	return MCTS_node{ swap_hands(new_players) };
}

std::size_t MCTS_node::table_index()
{
	std::size_t index{};
	std::size_t shift{ 1 };
	for (auto&& p : players)
	{
		index += shift * p.selected_action;
		shift *= p.actions.size();
	}

	return index;
}

std::unique_ptr<MCTS_node> MCTS::generete_root()
{
	return std::make_unique<MCTS_node>(nullptr, players);
}

void MCTS::simulate()
{
	MCTS_node* current = root.get();

	// UCT
	do
	{
		for (auto&& p : current->players)
			p.UCT(current->visit_count);

	} while (current->transpositional_table[current->table_index()]);

	// ADD one node

	current->transpositional_table[current->table_index()].reset(new MCTS_node{ current, current->tree_node() });

	// Game simulation
	/*MCTS_node simulator = *current->transpositional_table[current->table_index()];
	while (simulator.players[0].actions.size())
		simulator = simulator.new_node();
	*/
	// Back-propagation
	
}
