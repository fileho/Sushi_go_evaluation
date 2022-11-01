# Sushi_go_evaluation

This is an application to develop and benchmark AIs for the Sushi Go! board game.

## MCTS
The main focus is on the MCTS approaches. Sushi Go! has simultaneous moves. Therefore, we need to use SM-MCTS. It implements deterministic DUCT and stochastic EXP3 algortihms, focusing on effective implementation. The effective implementation allows us to calculate more simulations and pick a better move. 

There is also a cheating agent. The cheating agent can look at each player's cards. Otherwise, MCTS is using several determinations to play fair.

It also implements many heuristics. There are heuristics for picking a move during the simulation. But most important are the heuristics for scoring. Here a sigmoid-like function works the best.

## Genetic algorithm
There is also an implementation of a simple reflex agent. This agent uses several rules and assigns weight to them using a genetic algorithm.

## Results
DUCT heavily outperforms EXP3. Therefore, we do not need to play a mixed strategy in a Sushi Go game. Both of them destroy the simple genetic algorithm. 
