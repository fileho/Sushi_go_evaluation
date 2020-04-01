#include "MCTS.h"

#include <cmath>
#include <iostream>
#include <numeric>
#include <thread>
#include <utility>

MCTS_player MCTS_player::random_action()
{
	actions = generate_actions();
	std::random_device rd;
	std::uniform_int_distribution<std::size_t> dist(0, actions.size() - 1);
	
	selected_action = dist(rd);	
	
	return perform_action();
}

MCTS_player MCTS_player::perform_action() const
{
	card_list_t new_hand = hand;
	card_list_t new_played = played;

	auto& action = actions[selected_action];

	perform(new_hand, new_played, action.action);
	if (action.action2 != No_move)
	{
		perform(new_hand, new_played, action.action2);
		--new_played[11];
		++new_hand[11];
	}

	return MCTS_player{ new_hand, new_played, points, puddings };
}

std::size_t MCTS_player::UCT(double visit_count, double constant)
{
	std::size_t index{};
	double max_value{};
	for (std::size_t i{}; i < actions.size(); ++i)
	{
		if (!actions[i].visit_count)
			return i;

		double UCT = actions[i].reward / actions[i].visit_count + constant * std::sqrt(std::log(visit_count) / actions[i].visit_count);
		if (UCT > max_value)
		{
			max_value = UCT;
			index = i;
		}
	}

	return index;
}

void MCTS_player::generate_pairs(action_list_t& action_list) const
{
	if (!played[11])
		return;

	smart_generete(action_list, 0);
	smart_generete(action_list, 3);

	for (unsigned i{ 6 }; i < 11; ++i)
	{
		if (hand[i] >= 2)
			action_list.push_back(MCTS_action_data{ i, i });
	}
	std::vector<unsigned> vec{};
	
	if (hand[Nigiri3_i])
		vec.push_back(Nigiri3_i);
	else if (hand[Nigiri2_i])
		vec.push_back(Nigiri2_i);
	else if (hand[Nigiri1_i])
		vec.push_back(Nigiri1_i);

	if (hand[Maki3_i])
		vec.push_back(Maki3_i);
	else if (hand[Maki2_i])
		vec.push_back(Maki2_i);
	else if (hand[Maki1_i])
		vec.push_back(Maki1_i);

	for (std::size_t i{ 6 }; i < 11; ++i)
	{
		if (hand[i])
			vec.push_back(i);
	}
	// all pairs
	for (std::size_t i{}; i < vec.size(); ++i)
	{
		for (std::size_t j{ i + 1 }; j < vec.size(); ++j)
			action_list.push_back(MCTS_action_data{ vec[j], vec[i] });
	}

}
// finds two best nigiri/maki
void MCTS_player::smart_generete(action_list_t& action_list, unsigned i) const
{
	if (hand[i] >= 2)
	{
		action_list.push_back(MCTS_action_data{ i,i });
		return;
	}
	if (hand[i] && hand[i + 1])
	{
		action_list.push_back(MCTS_action_data{ i,i + 1 });
		return;
	}
	if (hand[i] && hand[i + 2])
	{
		action_list.push_back(MCTS_action_data{ i,i + 2 });
		return;
	}


	if (hand[i + 1] >= 2)
	{
		action_list.push_back(MCTS_action_data{ i + 1,i + 1 });
		return;
	}
	if (hand[i + 1] && hand[i + 2])
	{
		action_list.push_back(MCTS_action_data{ i + 1,i + 2 });
		return;
	}
	if(hand[i + 2] >= 2)
		action_list.push_back(MCTS_action_data{ i + 2,i + 2 });
	return;
}

void MCTS_player::perform(card_list_t& new_hand, card_list_t& new_played, action_t action) const
{
	--new_hand[action];

	if (action >= 3 || !new_played[Wasabi_i])
		++new_played[action];
	else
	{
		--new_played[Wasabi_i];
		if (action == 0)
			++new_played[Wasabi_Nig3];
		else if (action == 1)
			++new_played[Wasabi_Nig2];
		else
			++new_played[Wasabi_Nig1];
	}
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

	generate_pairs(action_list);
		
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


bool MCTS_node::is_terminal() const
{
	return !(players[0].actions.size());
}

std::vector<double> MCTS_node::evaluate(eval_type type) const
{
	std::vector<double> rewards(players.size(), 0);

	std::vector<std::pair<std::size_t, int>> points{};
	std::vector<std::pair<std::size_t, int>> makis{};
	std::vector<int> puddings{};

	const std::vector<int> dumplings{ 0,1,3,5,10,15,15,15,15,15,15 };

	for (std::size_t i{}; i < players.size(); ++i)
	{
		const auto& p = players[i].played;
		int point = 3 * p[Nigiri3_i] + 2 * p[Nigiri2_i] + p[Nigiri1_i] + 9 * p[Wasabi_Nig3] + 6 * p[Wasabi_Nig2] + 3 * p[Wasabi_Nig1] + \
			+ 5 * (p[7]/2) + 10 * (p[8] / 3) + dumplings[p[9]] + players[i].points;
		points.push_back(std::make_pair(i, point));

		int maki = 3 * p[Maki3_i] + 2 * p[Maki2_i] + p[Maki1_i];
		makis.push_back(std::make_pair(i,maki));
		puddings.push_back(p[10] + players[i].puddings);
	}
	std::vector<int> maki_rewards = maki(std::move(makis));
	std::vector<int> puddings_rewards = pudding(puddings);

	for (std::size_t i{}; i < maki_rewards.size(); ++i)
		points[i].second += maki_rewards[i] + puddings_rewards[i];

	std::sort(points.begin(), points.end(), [](auto&& x, auto&& y) {return x.second > y.second; });

	if (type == eval_type::win)
	{
		for (std::size_t i{}; i < points.size(); ++i)
			rewards[points[i].first] = 1 - i / (points.size() - 1.0);

		return rewards;
	}

	if (type == eval_type::higher_base)
	{
		rewards[points[0].first] = std::min(0.6 + (std::sqrt(static_cast<double>(points[0].second) - points[1].second)) / 6, 1.0);
		for (std::size_t i{ 1 }; i < points.size(); ++i)
			rewards[points[i].first] = std::max(0.6 - (std::sqrt(static_cast<double>(points[0].second) - points[i].second)) / 6, 0.0);

		return rewards;
	}

	rewards[points[0].first] = std::min(0.5 + (std::sqrt(static_cast<double>(points[0].second) - points[1].second)) / 6, 1.0);
	for (std::size_t i{ 1 }; i < points.size(); ++i)
		rewards[points[i].first] = std::max(0.5 - (std::sqrt(static_cast<double>(points[0].second) - points[i].second)) / 6, 0.0);

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
		new_players.emplace_back(p.perform_action());

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

std::vector<int> MCTS_node::pudding(std::vector<int>& puddings) const
{
	std::vector<int> ret(puddings.size(), 0);

	int max_puddings{ 0 };
	for (auto&& x : puddings)
	{
		if (x > max_puddings)
			max_puddings = x;
	}

	int min_puddings{ 20 };
	if (players.size() > 2)
	{
		for (auto&& x : puddings)
		{
			if (x < min_puddings)
				min_puddings = x;
		}
	}

	for (std::size_t i{}; i < puddings.size(); ++i)
	{
		if (puddings[i] == max_puddings)
			ret[i] = 6;
		else if (puddings[i] == min_puddings)
			ret[i] = -6;
	}

	return ret;
}


void MCTS::generete_root(const std::vector<MCTS_player>& player, std::size_t index)
{
	roots[index].reset(new MCTS_node{ nullptr, player });
}

std::pair<action_t, action_t> MCTS::find_best_move()
{
	if (round_index >= players.size())
	{
		for (std::size_t i{}; i < number_of_simulation; ++i)
			simulate_game(0);


		double best = 0;
		std::pair<action_t, action_t> action;
		for (auto&& a : roots[0].get()->players[0].actions)
		{
			if (a.reward >= best)
			{
				best = a.reward;
				action = std::make_pair(a.action, a.action2);
			}
		}

		return action;
	}

	// Simulates each determinizations in a single thread

	std::vector<std::thread> threadPool{};

	for (std::size_t i{}; i < roots.size(); ++i)
	{
		std::thread t(&MCTS::simulate_n_games, this, i);
		threadPool.emplace_back(std::move(t));
	}
	for (size_t i = 0; i < threadPool.size(); i++)
		threadPool[i].join();

	std::vector<double> rewards(roots[0]->players[0].actions.size(), 0);

	for (auto&& r : roots)
	{
		for (size_t i = 0; i < r->players[0].actions.size(); ++i)
			rewards[i] += r->players[0].actions[i].reward;
	}

	double best = 0;
	std::pair<action_t, action_t> action;
	auto& ac = roots[0]->players[0].actions;
	for (size_t i = 0; i < rewards.size(); ++i)
	{
		if (rewards[i] >= best)
		{
			best = rewards[i];
			action = std::make_pair(ac[i].action, ac[i].action2);
		}
	}

	return action;
}

void MCTS::init_players(const std::vector<card_t>& player)
{
	card_list_t hand(12, 0);
	card_list_t played(15, 0);

	for (auto&& x : player)
		++hand[x->MCTS()];

	players.emplace_back(MCTS_player{ hand, played });


	hand = card_list_t(12, 0);
	for(std::size_t i{}; i < 11 - player.size(); ++i)
		players.emplace_back(MCTS_player{ hand, played });
}

void MCTS::new_set(const std::vector<card_t>& player)
{
	card_list_t hand(12, 0);
	card_list_t played(15, 0);

	round_index = 1;

	for (auto&& x : player)
		++hand[x->MCTS()];

	players[0].hand = hand;
	players[0].played = played;

	hand = card_list_t(12, 0);
	for (size_t i{ 1 }; i < players.size(); i++)
	{
		players[i].hand = hand;
		players[i].played = played;
	}
}

void MCTS::determize()
{
	if (round_index >= players.size())
	{
		for (auto&& p : players)
			p.actions = p.generate_actions();
		generete_root(players, 0);
		return;
	}

	deck.create_deck(get_played());

	for (std::size_t index{}; index < roots.size(); ++index)
	{
		deck.shuffle();

		std::vector<MCTS_player> pl = players;

		for (std::size_t i{ round_index }; i < players.size(); ++i)
		{
			for (std::size_t j{}; j < 13 - round_index - players.size(); ++j)
				++pl[i].hand[deck.draw()];
		}

		for (auto&& p : pl)
			p.actions = p.generate_actions();

		generete_root(pl, index);
	}
}

void MCTS::update(const std::vector<player_t>& player, std::size_t index)
{
	++round_index;

	for (size_t i = 0; i < players.size(); i++)
	{
		players[i].played = from_card_list(player[(i + index) % players.size()]->played_list);
		players[i].hand = card_list_t(12, 0);
	}

	for (size_t i = 0; i < std::min(players.size(), round_index); ++i)
	{
		for (auto&& c : player[(i + index) % players.size()]->hand)
			++players[i].hand[c->MCTS()];
	}

	if (round_index == 12 - players.size())
		save_played();
}

void MCTS::add_points(const std::vector<player_t>& player, std::size_t index)
{
	for (size_t i = 0; i < players.size(); ++i)
	{
		players[i].points = player[(i + index) % player.size()]->points();
		players[i].puddings = player[(i + index) % player.size()]->puddings;
	}
}

// Main function which silumates single game
void MCTS::simulate_game(std::size_t index)
{
	MCTS_node* current = roots[index].get();

	// UCT
	while (!current->is_terminal())
	{

		for (auto&& p : current->players)
			p.selected_action = p.UCT(current->visit_count, UCT_const);

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

		rewards = simulator.evaluate(type);
	}
	else
		rewards = current->evaluate(type);
	
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

void MCTS::simulate_n_games(std::size_t index)
{
	for (size_t i = 0; i < number_of_simulation; ++i)
		simulate_game(index);
}

card_list_t MCTS::from_card_list(const card_list& cl)
{
	card_list_t played(15, 0);
	played[0] = cl.nigiri_3;
	played[1] = cl.nigiri_2;
	played[2] = cl.nigiri_1;
	played[3] = cl.maki_roll_3;
	played[4] = cl.maki_roll_2;
	played[5] = cl.maki_roll_1;
	played[6] = cl.wasabi;
	played[7] = cl.tempura;
	played[8] = cl.sashimi;
	played[9] = cl.dumplings;
	played[10] = cl.pudding;
	played[11] = cl.chopsticks;
	played[12] = cl.wasabi_nigiri_3;
	played[13] = cl.wasabi_nigiri_2;
	played[14] = cl.wasabi_nigiri_1;

	return played;
}

card_list_t MCTS::get_played() const
{
	auto ret = played_list;

	for (auto&& p : players)
	{
		for (std::size_t i{}; i < p.played.size(); ++i)
		{
			if (i >= 12)
			{
				ret[6] += p.played[i];
				ret[i - 12] += p.played[i];
			}
			else
				ret[i] += p.played[i];
		}
	}

	for (size_t i = 0; i < std::min(players.size(), round_index); ++i)
	{
		for (size_t j{}; j < players[i].hand.size(); ++j)
			ret[j] += players[i].hand[j];
	}

	return ret;
}

void MCTS::save_played()
{
	for (auto&& p : players)
	{
		for (std::size_t i{}; i < p.played.size(); ++i)
		{
			if (i >= 12)
			{
				played_list[6] += p.played[i];
				played_list[i - 12] += p.played[i];
			}
			else
				played_list[i] += p.played[i];
		}
	}
}




void MCTS_deck::create_deck(const card_list_t& played)
{
	deck.clear();

	card_list_t counts{ 5,10,5,8,12,6,6,14,14,14,10,4 };
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
	++index;
	return deck[index];
}

void MCTS_deck::shuffle()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::shuffle(deck.begin(), deck.end(), gen);
	index = 0;
}