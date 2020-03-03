#include "MCTS.h"

#include <cmath>
#include <iostream>
#include <numeric>

MCTS_player MCTS_player::random_action()
{
	actions = generate_actions();
	std::random_device rd;
	std::uniform_int_distribution<std::size_t> dist(0, actions.size() - 1);
	
	selected_action = dist(rd);	
	
	return preform_action();
}

MCTS_player MCTS_player::preform_action() const
{
	card_list_t new_hand = hand;
	--new_hand[actions[selected_action].action];

	card_list_t new_played = played;

	// Nigiri & Wasabi
	if (actions[selected_action].action >= 3 || !played[Wasabi_i])
		++new_played[actions[selected_action].action];
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

		double UCT = actions[i].reward / actions[i].visit_count + UCT_const * std::sqrt(std::log(visit_count) / actions[i].visit_count);
		if (UCT > max_value)
		{
			max_value = UCT;
			index = i;
		}
	}

	return index;
}

action_list_t MCTS_player::generate_actions() const
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
	{
		size *= p.actions.size();
	}

	
	for (std::size_t i{}; i < size; ++i)
		transpositional_table.push_back(nullptr);	
}

MCTS_node::MCTS_node(std::vector<MCTS_player> players) :
	parent{ nullptr }, transpositional_table{}, players{ players }{; }


bool MCTS_node::is_terminal() const
{
	return !(players[0].generate_actions()).size();
}

std::vector<double> MCTS_node::evaluate() const
{
	std::vector<double> rewards(players.size(), 0);

	std::vector<std::pair<std::size_t, int>> points{};
	std::vector<std::pair<std::size_t, int>> makis{};

	const std::vector<int> dumplings{ 0,1,3,5,10,15,15,15,15,15,15 };

	for (std::size_t i{}; i < players.size(); ++i)
	{
		const auto& p = players[i].played;
		int point = 3 * p[Nigiri3_i] + 2 * p[Nigiri2_i] + p[Nigiri1_i] + 9 * p[Wasabi_Nig3] + 6 * p[Wasabi_Nig2] + 3 * p[Wasabi_Nig1] + \
			+ 5 * (p[7]/2) + 10 * (p[8] / 3) + dumplings[p[9]];
		points.push_back(std::make_pair(i, point));

		int maki = 3 * p[Maki3_i] + 2 * p[Maki2_i] + p[Maki1_i];
		makis.push_back(std::make_pair(i,maki));
	}
	std::vector<int> maki_rewards = maki(std::move(makis));

	for (std::size_t i{}; i < maki_rewards.size(); ++i)
		points[i].second += maki_rewards[i];

	std::sort(points.begin(), points.end(), [](auto&& x, auto&& y) {return x.second > y.second; });


	for (std::size_t i{}; i < points.size(); ++i)
		rewards[points[i].first] = 1 - i / (points.size() + 1.0);

	return rewards;
}

std::vector<MCTS_player> MCTS_node::swap_hands(std::vector<MCTS_player>& new_players) const
{
	auto tmp = std::move(new_players[new_players.size() - 1].hand);

	for (std::size_t i{ new_players.size() - 1 }; i >= 1; --i)
		new_players[i].hand = std::move(new_players[i - 1].hand);

	new_players[0].hand = std::move(tmp);

	for (auto&& p : new_players)
		p.actions = p.generate_actions();

	return new_players;
}

std::vector<MCTS_player> MCTS_node::tree_node() const
{
	std::vector<MCTS_player> new_players{};
	for (auto&& p : players)
		new_players.emplace_back(p.preform_action());

	return swap_hands(new_players);
}


MCTS_node MCTS_node::new_node()
{
	std::vector<MCTS_player> new_players{};
	for (auto&& p : players)
		new_players.emplace_back(p.random_action());
	
	return MCTS_node{ swap_hands(new_players) };
}

std::size_t MCTS_node::table_index() const
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

std::vector<int> MCTS_node::maki(std::vector<std::pair<std::size_t, int>>&& maki_rolls) const
{
	std::vector<int>rewards( maki_rolls.size(), 0);

	std::sort(maki_rolls.begin(), maki_rolls.end(), [](auto&& v1, auto&& v2) { return v1.second > v2.second; });
	if (maki_rolls[1].second == maki_rolls[0].second)
	{
		int val = maki_rolls[0].second;
		for (std::size_t i{}; i < maki_rolls.size(); ++i)
		{
			if (maki_rolls[i].second == val)
				rewards[maki_rolls[i].first] += 6;
			else
				return rewards;
		}
	}
	else
	{
		int val = maki_rolls[1].second;
		rewards[maki_rolls[0].first] += 6;
		for (std::size_t i{ 1 }; i < maki_rolls.size(); ++i)
		{
			if (maki_rolls[i].second == val)
				rewards[maki_rolls[i].first] += 3;
			else
				return rewards;
		}
	}

	return rewards;
}


void MCTS::generete_root()
{
	root.reset(new MCTS_node{ nullptr, players });
}

void MCTS::find_best_move()
{
	for (std::size_t i{}; i < 10000; ++i)
		simulate_game();

	
	for (auto&& p : root.get()->players)
	{
		for (auto&& a : p.actions)
			std::cout << a.action << ": " << a.visit_count << "  reward: " << a.reward << "\n";
		std::cout << "\n\n";
	}
}

void MCTS::init_players(const std::vector<card_t>& player)
{
	card_list_t hand(12, 0);
	card_list_t played(15, 0);

	for (auto&& x : player)
		++hand[x->MCTS()];

	players.emplace_back(MCTS_player{ hand, played });

	deck.create_deck(hand);

	hand = card_list_t(12, 0);
	for(std::size_t i{}; i < 11 - player.size(); ++i)
		players.emplace_back(MCTS_player{ hand, played });
}

void MCTS::determize()
{
	if (round_index >= players.size())
		return;

	deck.shuffle();

	for (std::size_t i{ round_index }; i < players.size(); ++i)
	{
		for (std::size_t j{}; j < 13 - round_index - players.size(); ++j)
			++players[i].hand[deck.draw()];
		players[i].actions = players[i].generate_actions();
	}
}

// main function which silumates single game
void MCTS::simulate_game()
{
	MCTS_node* current = root.get();

	// UCT
	while (!current->is_terminal())
	{

		for (auto&& p : current->players)
			p.selected_action = p.UCT(current->visit_count);

		if (current->transpositional_table[current->table_index()])
			current = current->transpositional_table[current->table_index()].get();
		else
			break;
	}

	std::vector<double> rewards{};

	if (!current->is_terminal())
	{
		// ADD one node
		current->transpositional_table[current->table_index()].reset(new MCTS_node{ current, current->tree_node() });
		current = current->transpositional_table[current->table_index()].get();


		// Game simulation
		MCTS_node simulator = MCTS_node{ current->players };
		while (!simulator.is_terminal())
			simulator = simulator.new_node();

		rewards = simulator.evaluate();
	}
	else
		rewards = current->evaluate();
	
	// Back-propagation
	while (current)
	{
		if (current->players[0].actions.size())
		{
			for (std::size_t i{}; i < current->players.size(); ++i)
			{
				auto& p = current->players[i];
				p.actions[p.selected_action].reward += rewards[i];
				++p.actions[p.selected_action].visit_count;
			}
		}
		++current->visit_count;
		current = current->parent;
	}
}

void MCTS_deck::create_deck(const std::vector<MCTS_card_t>& played)
{
	deck.clear();

	std::vector<MCTS_card_t> counts{ 10,5,10,8,12,6,6,14,14,14,10,4 };
	for (std::size_t i{}; i < counts.size(); ++i)
		counts[i] -= played[i];

	for (std::size_t i{}; i < counts.size(); ++i)
	{
		for (std::size_t j{}; j < counts[i]; ++j)
			deck.push_back(i);
	}
}

MCTS_card_t MCTS_deck::draw()
{
	auto ret = deck[deck.size() - 1];
	deck.pop_back();
	return ret;
}

void MCTS_deck::shuffle()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::shuffle(deck.begin(), deck.end(), gen);
}
