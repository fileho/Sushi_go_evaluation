#include "MCTS.h"

#include <cmath>
#include <random>
#include <numeric>
#include <thread>
#include <utility>
#include <algorithm>
#include <iostream>

MCTS_player MCTS_player::random_action(playout type) noexcept
{
	actions = generate_actions();

	switch (type)
	{
	case playout::random:
	{
		std::uniform_int_distribution<action_t> dist(0, static_cast<action_t>(actions.size() - 1));
		prob = 1.0 / actions.size();
		selected_action = dist(random_generator);
		break;
	}
	case playout::weighted:
	{
		// Assigns possive weight to each action
		const std::vector<unsigned> weights{ 4, 2, 1, 3, 2, 1, 4, 3, 1, 2, 4, 2, 0, 0, 0 ,1 };
		std::vector<int> values{};
		action_t sum{};
		for (auto&& a : actions)
		{
			values.push_back(weights[a.action] + weights[a.action2] - 1);
			sum += weights[a.action] + weights[a.action2] - 1;
		}
		std::uniform_int_distribution<action_t> dist(0, sum);
		// Selects action from weighted distribution
		auto val = dist(random_generator);
		action_t index{};
		sum = values[0];

		while (sum < val)
		{
			++index;
			sum += values[index];
		}
		prob = 1.0 / actions.size();
		selected_action = index;

		break;
	}
	default:
		break;
	}

	
	return perform_action();
}

MCTS_player MCTS_player::perform_action() const noexcept
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

// Computes UCT value for all actions, makes action selection
action_t MCTS_player::UCT(double visit_count, double constant) noexcept
{
	action_t index{};
	double max_value{};
	for (action_t i{}; i < actions.size(); ++i)
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

action_t MCTS_player::exp3(double lambda) noexcept
{
	double maxReward{};

	// Unexplored moves
	for (action_t i = 0; i < actions.size(); ++i)
	{
		if (!actions[i].visit_count)
		{
			prob = lambda / actions.size();
			return i;
		}
		if (actions[i].reward > maxReward)
			maxReward = actions[i].reward;
	}

	// calculates the distuburion
	std::vector<double> distribution{};

	const double exploration { lambda / actions.size() };

	double sum{};

	for (auto&& a : actions)
		sum += std::exp(exploration * (a.reward - maxReward));
	
	double distSum{};

	for (auto&& a : actions)
	{
		double val = ((1 - lambda) * std::exp(exploration * (a.reward - maxReward))) / sum + exploration;
		distribution.push_back(val);
		distSum += val;
	}

	// Selects an action according to distribution
	std::uniform_real_distribution<> dist(0, distSum);

	double value = dist(random_generator);

	sum = 0;

	for (action_t i = 0; i < distribution.size(); ++i)
	{
		sum += distribution[i];
		if (sum >= value)
		{
			prob = distribution[i];
			return i;
		}
	}

	return 0UL;
}

void MCTS_player::generate_pairs(action_list_t& action_list) const noexcept
{
	// Checks Chopsticks
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

	for (action_t i{ 6 }; i < 11; ++i)
	{
		if (hand[i])
			vec.push_back(i);
	}
	// all pairs
	for (action_t i{}; i < vec.size(); ++i)
	{
		for (action_t j{ i + 1 }; j < vec.size(); ++j)
			action_list.push_back(MCTS_action_data{ vec[j], vec[i] });
	}

}
// Finds two best nigiri/maki when agent has a Chopstick card
void MCTS_player::smart_generete(action_list_t& action_list, unsigned i) const noexcept
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

void MCTS_player::perform(card_list_t& new_hand, card_list_t& new_played, action_t action) const noexcept
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

// Generate all "non-terrible" actions - allows only highest Nigiri, highest Maki and non duplicates
action_list_t MCTS_player::generate_actions() const noexcept
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


bool MCTS_node::is_terminal() const noexcept
{
	return !(players[0].actions.size());
}

std::vector<double> MCTS_node::evaluate(eval_type type, double pudding_value) const noexcept
{
	std::vector<double> rewards(players.size(), 0);

	std::vector<std::pair<std::size_t, double>> points{};
	std::vector<int> makis{};
	std::vector<int> puddings{};

	const std::vector<int> dumplings{ 0,1,3,5,10,15,15,15,15,15,15 };

	for (std::size_t i{}; i < players.size(); ++i)
	{
		const auto& p = players[i].played;
		int point = 3 * p[Nigiri3_i] + 2 * p[Nigiri2_i] + p[Nigiri1_i] + 9 * p[Wasabi_Nig3] + 6 * p[Wasabi_Nig2] + 3 * p[Wasabi_Nig1] + \
			+ 5 * (p[7]/2) + 10 * (p[8] / 3) + dumplings[p[9]] + players[i].points;
		points.push_back(std::make_pair(i, point));

		makis.push_back(3 * p[Maki3_i] + 2 * p[Maki2_i] + p[Maki1_i]);
		puddings.push_back(p[10] + players[i].puddings);
	}
	auto maki_rewards = maki(makis);
	auto puddings_rewards = pudding(puddings, pudding_value);

	for (std::size_t i{}; i < maki_rewards.size(); ++i)
		points[i].second += maki_rewards[i] + puddings_rewards[i];

	std::sort(points.begin(), points.end(), [](auto&& x, auto&& y) {return x.second > y.second; });

	// Different payout heuristics
	switch (type)
	{
	case eval_type::win:
	{
		if (points[0].second > points[1].second)
		{
			rewards[points[0].first] = 1;
			return rewards;
		}

		auto val = points[0].second;
		std::size_t index{};
		while (index < points.size() && points[index].second == val)
		{
			rewards[points[index].first] = 0.5;
			++index;
		}

		return rewards;
	}
	case eval_type::point_diff:
		rewards[points[0].first] = std::min(0.5 + (std::sqrt(static_cast<double>(points[0].second) - points[1].second)) / 6, 1.0);
		for (std::size_t i{ 1 }; i < points.size(); ++i)
			rewards[points[i].first] = std::max(0.5 - (std::sqrt(static_cast<double>(points[0].second) - points[i].second)) / 6, 0.0);

		return rewards;
	case eval_type::higher_base:
		rewards[points[0].first] = std::min(0.6 + (std::sqrt(static_cast<double>(points[0].second) - points[1].second)) / 6, 1.0);
		for (std::size_t i{ 1 }; i < points.size(); ++i)
			rewards[points[i].first] = std::max(0.6 - (std::sqrt(static_cast<double>(points[0].second) - points[i].second)) / 6, 0.0);

		return rewards;
	case eval_type::sigmoid:
		rewards[points[0].first] = 1.0 / (1 + std::exp(-(points[0].second - points[1].second)));
		for (std::size_t i{ 1 }; i < points.size(); ++i)
			rewards[points[i].first] = 1.0 / (1 + std::exp(-(points[i].second - points[0].second)));

		return rewards;
	case eval_type::sigmoid2:
		rewards[points[0].first] = 1.0 / (1 + std::exp2(-(points[0].second - points[1].second)));
		for (std::size_t i{ 1 }; i < points.size(); ++i)
			rewards[points[i].first] = 1.0 / (1 + std::exp2(-(points[i].second - points[0].second)));

		return rewards;
	case eval_type::sigmoid1x:
		rewards[points[0].first] = 1.0 / (1 + std::pow(1.7,-(points[0].second - points[1].second)));
		for (std::size_t i{ 1 }; i < points.size(); ++i)
			rewards[points[i].first] = 1.0 / (1 + std::pow(1.7,-(points[i].second - points[0].second)));
		
		return rewards;
	default:
		return rewards;
	}
}

std::vector<MCTS_player> MCTS_node::swap_hands(std::vector<MCTS_player>& new_players) const noexcept
{
	auto tmp = std::move(new_players[new_players.size() - 1].hand);

	for (std::size_t i{ new_players.size() - 1 }; i >= 1; --i)
		new_players[i].hand = std::move(new_players[i - 1].hand);

	new_players[0].hand = std::move(tmp);

	for (auto&& p : new_players)
		p.actions = p.generate_actions();

	return new_players;
}

// Creates a node which will be added to a game tree
std::vector<MCTS_player> MCTS_node::tree_node() const noexcept
{
	std::vector<MCTS_player> new_players{};
	for (auto&& p : players)
		new_players.emplace_back(p.perform_action());

	return swap_hands(new_players);
}


// Creates random child node for rollout policy
MCTS_node MCTS_node::new_node(playout type) noexcept
{
	std::vector<MCTS_player> new_players{};
	for (auto&& p : players)
		new_players.emplace_back(p.random_action(type));
	
	return MCTS_node{ swap_hands(new_players) };
}

std::size_t MCTS_node::table_index() const noexcept
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

std::vector<int> MCTS_node::maki(const std::vector<int>& maki_rolls) const noexcept
{
	std::vector<int>rewards( maki_rolls.size(), 0);

	int max{};
	std::vector<unsigned> index{};

	// Add 6 points to all players with highest amount of maki rolls
	for (action_t i = 0; i < maki_rolls.size(); ++i)
	{
		if (maki_rolls[i] > max)
		{
			index.clear();
			max = maki_rolls[i];
			index.push_back(i);
		}
		else if (maki_rolls[i] == max && max > 0)
			index.push_back(i);
	}

	for (auto&& i : index)
		rewards[i] = 6;
	
	index.clear();

	int second_max{};
	// Add 3 points to all players with second highest amount of maki rolls, must have at least 1 maki
	for (action_t i = 0; i < maki_rolls.size(); ++i)
	{
		if (maki_rolls[i] > second_max && maki_rolls[i] < max)
		{
			index.clear();
			second_max = maki_rolls[i];
			index.push_back(i);
		}
		else if (maki_rolls[i] == second_max && second_max > 0)
			index.push_back(i);
	}
	for (auto&& i : index)
		rewards[i] = 3;

	return rewards;
}

std::vector<double> MCTS_node::pudding(const std::vector<int>& puddings, double value) const noexcept
{
	std::vector<double> ret(puddings.size(), 0);

	int max_puddings{ 0 };
	for (auto&& x : puddings)
	{
		if (x > max_puddings)
			max_puddings = x;
	}

	// Don't subtract points for Puddings in a game of two players
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
			ret[i] = value;
		else if (puddings[i] == min_puddings)
			ret[i] = -value;
	}

	return ret;
}


void MCTS::generete_root(const std::vector<MCTS_player>& player, std::size_t index) noexcept
{
	roots[index] = MCTS_node{ nullptr, player };
	//roots[index].reset(new MCTS_node{ nullptr, player });
}

std::pair<action_t, action_t> MCTS::find_best_move() noexcept
{
	if (round_index >= players.size())
	{
		// Don't search is case of only valid action
		if (roots[0].players[0].actions.size() == 1)
			return std::make_pair(roots[0].players[0].actions[0].action, roots[0].players[0].actions[0].action2);


		simulate_n_games(0);


		// Returns best move
		double best = 0;
		std::pair<action_t, action_t> action;
		for (auto&& a : roots[0].players[0].actions)
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

	// Finds best move for simulations
	std::vector<double> rewards(roots[0].players[0].actions.size(), 0);

	for (auto&& r : roots)
	{
		for (size_t i = 0; i < r.players[0].actions.size(); ++i)
			rewards[i] += r.players[0].actions[i].reward;
	}

	double best = 0;
	std::pair<action_t, action_t> action;
	auto& ac = roots[0].players[0].actions;
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

std::pair<action_t, action_t> MCTS::Exp3_best_move() noexcept
{
	if (round_index >= players.size() || roots.size() == 1)
	{
		// Don't search is case of only valid action
		if (roots[0].players[0].actions.size() == 1)
			return std::make_pair(roots[0].players[0].actions[0].action, roots[0].players[0].actions[0].action2);

		simulate_n_exp3(0);

		// Selects move from distribution
		double sum = static_cast<double>(number_of_simulation);

		// Removes action caused by exploration
		for (auto&& a : roots[0].players[0].actions)
			a.visit_count = std::max(0.0, a.visit_count - sum * tuning_constant / roots[0].players[0].actions.size());

		std::vector<double> finalDist{};
		double finalSum{};

		for (auto&& a : roots[0].players[0].actions)
		{
			double value = a.visit_count / sum;
			finalDist.push_back(value);
			finalSum += value;
		}

		std::uniform_real_distribution<> dist(0, finalSum);

		double value = dist(random_generator);

		sum = 0;

		for (size_t i = 0; i < finalDist.size(); ++i)
		{
			sum += finalDist[i];
			if (sum >= value)
			{
				auto& a = roots[0].players[0].actions[i];
				return std::make_pair(a.action, a.action2);
			}
		}
		return std::make_pair(0, 0);
	}

	// Simulates each determinizations in a single thread

	std::vector<std::thread> threadPool{};

	for (std::size_t i{}; i < roots.size(); ++i)
	{
		std::thread t(&MCTS::simulate_n_exp3, this, i);
		threadPool.emplace_back(std::move(t));
	}
	for (size_t i = 0; i < threadPool.size(); i++)
		threadPool[i].join();

	// Finds best move for simulations
	std::vector<double> visits(roots[0].players[0].actions.size(), 0);

	for (auto&& r : roots)
	{
		for (size_t i = 0; i < r.players[0].actions.size(); ++i)
			visits[i] += r.players[0].actions[i].visit_count;
	}

	double sum = static_cast<double>(number_of_simulation) * static_cast<double>(roots.size());

	for (auto&& v : visits)
		v = std::max(0.0, v - sum * tuning_constant / visits.size());

	std::vector<double> finalDist{};
	double finalSum{};

	for (auto&& v : visits)
	{
		double value = v / sum;
		finalDist.push_back(value);
		finalSum += value;
	}

	std::uniform_real_distribution<> dist(0, finalSum);

	double value = dist(random_generator);

	sum = 0;

	for (size_t i = 0; i < finalDist.size(); ++i)
	{
		sum += finalDist[i];
		if (sum >= value)
		{
			auto& a = roots[0].players[0].actions[i];
			return std::make_pair(a.action, a.action2);
		}
	}

	return std::make_pair(0, 0);
}

void MCTS::init_players(const std::vector<card_t>& player) noexcept
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

void MCTS::new_set(const std::vector<card_t>& player) noexcept
{
	card_list_t hand(12, 0);
	card_list_t played(15, 0);

	round_index = 1;

	// Handles pudding rewards
	if (pudding_value < 6)
		++pudding_value;

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

void MCTS::determize() noexcept
{
	// Perfect information
	if (round_index >= players.size())
	{
		for (auto&& p : players)
			p.actions = p.generate_actions();
		generete_root(players, 0);
		return;
	}

	// Partial information
	// Creates consistent deck sample
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

void MCTS::update(const std::vector<player_t>& player, std::size_t index) noexcept
{
	++round_index;

	// Updates played cards
	for (size_t i = 0; i < players.size(); ++i)
	{
		players[i].played = from_card_list(player[(i + index) % players.size()]->played_list);
		players[i].hand = card_list_t(12, 0);
	}


	// Updates hands of known players
	for (size_t i = 0; i < std::min(players.size(), round_index); ++i)
	{
		for (auto&& c : player[(i + index) % players.size()]->hand)
			++players[i].hand[c->MCTS()];
	}

	if (round_index == 12 - players.size())
		save_played();
}

void MCTS::add_points(const std::vector<player_t>& player, std::size_t index) noexcept
{
	for (size_t i = 0; i < players.size(); ++i)
	{
		players[i].points = player[(i + index) % player.size()]->points();
		players[i].puddings = player[(i + index) % player.size()]->puddings;
	}
}

void MCTS::cheat_new_set() noexcept
{
	if (pudding_value < 6)
		++pudding_value;
}

// Main function which silumates single game
void MCTS::simulate_game(std::size_t index) noexcept
{
//	MCTS_node* current = roots[index].get();
	MCTS_node* current = &roots[index];
	// UCT
	while (!current->is_terminal())
	{
		for (auto&& p : current->players)
			p.selected_action = p.UCT(current->visit_count, tuning_constant);

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
			simulator = simulator.new_node(playout_type);

		rewards = simulator.evaluate(type, pudding_value);
	}
	else
		rewards = current->evaluate(type, pudding_value);
	
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

void MCTS::simulate_exp3(std::size_t index) noexcept
{
	MCTS_node* current = &roots[index];
	// EXP3 
	while (!current->is_terminal())
	{
		for (auto&& p : current->players)
			p.selected_action = p.exp3(tuning_constant);

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
			simulator = simulator.new_node(playout_type);

		rewards = simulator.evaluate(type, pudding_value);
	}
	else
		rewards = current->evaluate(type, pudding_value);

	// Back-propagation
	while (current)
	{
		if (current->players[0].actions.size())
		{
			for (std::size_t i{}; i < current->players.size(); ++i)
			{
				auto& p = current->players[i];
				// Reward is scaled according to distribution
				p.actions[p.selected_action].reward += rewards[i] / p.prob;
				++p.actions[p.selected_action].visit_count;
			}
		}
		++current->visit_count;
		current = current->parent;
	}
}

void MCTS::simulate_n_games(std::size_t index) noexcept
{
	for (size_t i = 0; i < number_of_simulation; ++i)
		simulate_game(index);
}

void MCTS::simulate_n_exp3(std::size_t index) noexcept
{
	for (size_t i = 0; i < number_of_simulation; ++i)
		simulate_exp3(index);
}

card_list_t MCTS::from_card_list(const card_list& cl) noexcept
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

card_list_t MCTS::get_played() const noexcept
{
	auto ret = played_list;

	// Played cards of all players
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

	// Seen hands
	for (size_t i = 0; i < std::min(players.size(), round_index); ++i)
	{
		for (size_t j{}; j < players[i].hand.size(); ++j)
			ret[j] += players[i].hand[j];
	}

	return ret;
}

void MCTS::save_played() noexcept
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



void MCTS_deck::create_deck(const card_list_t& played) noexcept
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

// Draw only moves the index so I can reshuffle the deck and reset index instead of creating a new deck
MCTS_card_t MCTS_deck::draw() noexcept
{
	++index;
	return deck[index];
}

void MCTS_deck::shuffle() noexcept
{
	std::mt19937 gen(random_generator());
	std::shuffle(deck.begin(), deck.end(), gen);
	index = 0;
}