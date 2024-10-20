#include "GameTree.hpp"
#include <iostream>
#include <limits>
#include <algorithm>

Minimax::Minimax() {

}

// generate a game tree starting from root node for a game of tic-tac-toe
void Minimax::GenerateStates(GTNode* root, bool xPlayer) {
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			if (root->mState.mBoard[i][j] == GameState::Empty) {
				GTNode* node = new GTNode;
				root->mChildren.emplace_back(node);
				node->mState = root->mState;
				node->mState.mBoard[i][j] = xPlayer ? GameState::X : GameState::O;
				GenerateStates(node, !xPlayer);
			}
		}
	}
}

// calculate score for any state using either a heuristic value (for non-terminal states) or the actual score value (for terminal states)
float Minimax::GetScore(const GameState& state) {
	// are any of the rows the same?
	for (int i = 0; i < 3; ++i) {
		bool same = true;
		GameState::SquareState v = state.mBoard[i][0];
		for (int j = 1; j < 3; ++j) {
			if (state.mBoard[i][j] != v) {
				same = false;
			}
		}

		if (same) {
			if (v == GameState::X) {
				return 1.0f;  // since X player goes first (maximizer)
			}
			else {
				return -1.0f;  // O player is minimizer
			}
		}
	}

	// are any of the columns the same?
	for (int j = 0; j < 3; ++j) {
		bool same = true;
		GameState::SquareState v = state.mBoard[0][j];
		for (int i = 1; i < 3; ++i) {
			if (state.mBoard[i][j] != v) {
				same = false;
			}
		}

		if (same) {
			if (v == GameState::X) {
				return 1.0f;
			}
			else {
				return -1.0f;
			}
		}
	}

	// what about diagonals?
	if (((state.mBoard[0][0] == state.mBoard[1][1]) &&
		(state.mBoard[1][1] == state.mBoard[2][2])) ||
		((state.mBoard[2][0] == state.mBoard[1][1]) &&
		(state.mBoard[1][1] == state.mBoard[0][2]))) 
	{
		if (state.mBoard[1][1] == GameState::X) {
			return 1.0f;
		}
		else {
			return -1.0f;
		}
	}

	// we tied
	return 0.0f;
}

float Minimax::MaxPlayer(const GTNode* node) {
	// if this is a leaf, return score
	if (node->mChildren.empty()) {
		return GetScore(node->mState);
	}

	// find the subtree with the maximum value
	float maxValue = -std::numeric_limits<float>::infinity();
	for (const GTNode* child : node->mChildren) {
		maxValue = std::max(maxValue, MinPlayer(child));
	}
	return maxValue;
}

float Minimax::MinPlayer(const GTNode* node) {
	// if this is a leaf, return score
	if (node->mChildren.empty()) {
		return GetScore(node->mState);
	}

	// find the subtree with the minimum value
	float minValue = std::numeric_limits<float>::infinity();
	for (const GTNode* child : node->mChildren) {
		minValue = std::min(minValue, MaxPlayer(child));
	}
	return minValue;
}

const GTNode* Minimax::MinimaxDecide(const GTNode* root) {
	// find the subtree with the maximum value, and save the choice
	const GTNode* choice = nullptr;
	float maxValue = -std::numeric_limits<float>::infinity();
	for (const GTNode* child : root->mChildren) {
		float v = MinPlayer(child);
		if (v > maxValue) {
			maxValue = v;
			choice = child;
		}
	}
	return choice;
}

// this function tries to achieve what MaxPlayer() does by generating game tree during 
// execution rather than beforehand
float Minimax::MaxPlayerLimit(const GameState* state, int depth) {
	// if this is terminal or we have gone max depth
	if (depth == 0 || IsTerminal(*state)) {
		return GetScore(*state);
	}

	// find the subtree with the max value
	float maxValue = -std::numeric_limits<float>::infinity();
	for (const GameState* child : GetPossibleMoves(*state)) {
		maxValue = std::max(maxValue, MinPlayerLimit(child, depth - 1));
	}
	return maxValue;
}

// this function tries to achieve what MinPlayer() does by generating game tree during
// execution rather than beforehand
float Minimax::MinPlayerLimit(const GameState* state, int depth) {
	// if this is terminal or we have gona max depth
	if (depth == 0 || IsTerminal(*state)) {
		return GetScore(*state);
	}

	// find the subtree with the min value
	float minValue = std::numeric_limits<float>::infinity();
	for (const GameState* child : GetPossibleMoves(*state)) {
		minValue = std::min(minValue, MaxPlayerLimit(child, depth - 1));
	}
	return minValue;
}

// this function tries to achieve what MinimaxDecide() does by generating game tree during 
// execution rather than beforehand
const GameState* Minimax::MinimaxDecideLimit(const GameState* root, int maxDepth) {
	// find the subtree with the maximum value, and save the choice
	const GameState* choice = nullptr;
	float maxValue = -std::numeric_limits<float>::infinity();
	for (const GameState* child : GetPossibleMoves(*root)) {
		float v = MinPlayerLimit(child, maxDepth - 1);
		if (v > maxValue) {
			maxValue = v;
			choice = child;
		}
	}
	return choice;
}

float Minimax::AlphaBetaMax(const GTNode* node, float alpha, float beta)
{
	// if this is a leaf, return score
	if (node->mChildren.empty()) {
		return GetScore(node->mState);
	}

	// find the subtree with the maximum value
	float maxValue = -std::numeric_limits<float>::infinity();
	for (const GTNode* child : node->mChildren)
	{
		maxValue = std::max(maxValue, AlphaBetaMin(child, alpha, beta));
		if (maxValue >= beta)
		{
			return maxValue; // beta prune
		}
		alpha = std::max(maxValue, alpha);
	}
	return maxValue;
}

float Minimax::AlphaBetaMin(const GTNode* node, float alpha, float beta) {
	// if this is a leaf, return score
	if (node->mChildren.empty()) {
		return GetScore(node->mState);
	}

	// find the subtree with the minimum value
	float minValue = std::numeric_limits<float>::infinity();
	for (const GTNode* child : node->mChildren) {
		minValue = std::min(minValue, AlphaBetaMax(child, alpha, beta));
		if (minValue <= alpha) {
			return minValue;  // alpha prune
		}
		beta = std::min(minValue, beta);
	}
	return minValue;
}

const GTNode* Minimax::AlphaBetaDecide(const GTNode* root) {
	// find the subtree with the maximum value, and save the choice
	const GTNode* choice = nullptr;
	float maxValue = -std::numeric_limits<float>::infinity();
	float beta = std::numeric_limits<float>::infinity();
	for (const GTNode* child : root->mChildren) {
		float v = AlphaBetaMin(child, maxValue, beta);
		if (v > maxValue) {
			maxValue = v;
			choice = child;
		}
	}
	return choice;
}

float Minimax::AlphaBetaMaxLimit(const GameState* node, int depth, float alpha, float beta) {
	if (depth == 0 || IsTerminal(*node)) {
		return GetScore(*node);
	}

	float maxValue = -std::numeric_limits<float>::infinity();
	for (const GameState* child : GetPossibleMoves(*node)) {
		maxValue = std::max(maxValue, AlphaBetaMinLimit(child, depth - 1, alpha, beta));

		// if maxValue is greater than or equal to beta, it means the score can be no
		// better than the previous upper bound - it becomes unnecessary to test the remaining
		// siblings so the function returns
		if (maxValue >= beta) {
			return maxValue;  // beta prune
		}

		// otherwise, increase the alpha lower bound if maxValue is greater than alpha
		alpha = std::max(maxValue, alpha);
	}
	return maxValue;
}

float Minimax::AlphaBetaMinLimit(const GameState* node, int depth, float alpha, float beta) {
	if (depth == 0 || IsTerminal(*node)) {
		return GetScore(*node);
	}

	float minValue = std::numeric_limits<float>::infinity();
	for (const GameState* child : GetPossibleMoves(*node)) {
		minValue = std::min(minValue, AlphaBetaMaxLimit(child, depth - 1, alpha, beta));
		
		// if minValue is less than or equal to alpha, it means the score can be no
		// better than the previous lower bound - it becomes unnecessary to test the remaining
		// siblings so the function returns
		if (minValue <= alpha) {
			return minValue;  // alpha prune
		}

		// otherwise, decrease the beta upper bound if minValue is less than beta
		beta = std::min(minValue, beta);
	}
	return minValue;
}

const GameState* Minimax::AlphaBetaDecideLimit(const GameState* root, int maxDepth) {
	const GameState* choice = nullptr;
	
	// alpha starts at negative infinity, beta at positive infinity
	float maxValue = -std::numeric_limits<float>::infinity();
	float beta = std::numeric_limits<float>::infinity();
	for (const GameState* child : GetPossibleMoves(*root)) {
		float v = AlphaBetaMinLimit(child, maxDepth - 1, maxValue, beta);
		if (v > maxValue) {
			maxValue = v;
			choice = child;
		}
	}
	return choice;
}

bool Minimax::IsTerminal(const GameState& state) {
	// placeholder for determining whether a tic-tac-toe GameState is a terminal state
	return false;
}

std::vector<const GameState*> Minimax::GetPossibleMoves(const GameState& state) {
	// placeholder for finding a vector of game states that are one move after the 
	// current state
	return {};
}

void Minimax::TestTicTacToe() {
	GTNode* root = new GTNode;
	root->mState.mBoard[0][0] = GameState::O;
	root->mState.mBoard[0][1] = GameState::Empty;
	root->mState.mBoard[0][2] = GameState::X;
	root->mState.mBoard[1][0] = GameState::X;
	root->mState.mBoard[1][1] = GameState::O;
	root->mState.mBoard[1][2] = GameState::O;
	root->mState.mBoard[2][0] = GameState::X;
	root->mState.mBoard[2][1] = GameState::Empty;
	root->mState.mBoard[2][2] = GameState::Empty;

	GenerateStates(root, true);
	const GTNode* choice = AlphaBetaDecide(root);
	std::cout << choice->mChildren.size();
}