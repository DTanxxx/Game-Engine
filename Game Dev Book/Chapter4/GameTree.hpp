#pragma once

#include <vector>

// tic-tac-toe
struct GameState {
	enum SquareState {
		Empty,
		X,
		O
	};
	
	// array of board
	SquareState mBoard[3][3];
};

// a game tree node
struct GTNode {
	// children nodes
	std::vector<GTNode*> mChildren;
	// state of game at this node
	GameState mState;
};

// for tic-tac-toe
class Minimax {
public:
	Minimax();

	float MaxPlayer(const GTNode* node);  // calculate best possible score for max player using game tree
	float MinPlayer(const GTNode* node);  // calculate best possible score for min player using game tree
	const GTNode* MinimaxDecide(const GTNode* root);  // determine the best move for max player using game tree

	float MaxPlayerLimit(const GameState* state, int depth);  // calculate best possible score for max player without knowing about any game tree
	float MinPlayerLimit(const GameState* state, int depth);  // calculate best possible score for min player without knowing about any game tree
	const GameState* MinimaxDecideLimit(const GameState* root, int maxDepth);  // determine the best move for max player without knowing about any game tree

	// alpha-beta pruning
	// alpha and beta values are the lower and upper bounds of the score for a node
	float AlphaBetaMax(const GTNode* node, float alpha, float beta);
	float AlphaBetaMin(const GTNode* node, float alpha, float beta);
	const GTNode* AlphaBetaDecide(const GTNode* root);

	float AlphaBetaMaxLimit(const GameState* node, int depth, float alpha, float beta);
	float AlphaBetaMinLimit(const GameState* node, int depth, float alpha, float beta);
	const GameState* AlphaBetaDecideLimit(const GameState* root, int maxDepth);

	// helper functions
	void GenerateStates(GTNode* root, bool xPlayer);
	float GetScore(const GameState& state);  
	bool IsTerminal(const GameState& state);
	std::vector<const GameState*> GetPossibleMoves(const GameState& state);

	void TestTicTacToe();
};