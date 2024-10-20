#pragma once

#include <vector>

class BoardState {
public:
	enum SquareState {
		Empty,
		Red,
		Yellow
	};

	BoardState();

	std::vector<BoardState*> GetPossibleMoves(SquareState player) const;
	bool IsTerminal() const;
	float GetScore() const;

	SquareState mBoard[6][7];

protected:
	bool IsFull() const;
	int GetFourInARow() const;
	float CalculateHeuristic() const;
};

// try to place the player's piece
bool TryPlayerMove(class BoardState* state, int column);

// make the next CPU move
void CPUMove(class BoardState* state);

float AlphaBetaMaxLimit(const class BoardState* state, int depth, float alpha, float beta);
float AlphaBetaMinLimit(const class BoardState* state, int depth, float alpha, float beta);
const class BoardState* AlphaBetaDecideLimit(const class BoardState* root, int maxDepth);
