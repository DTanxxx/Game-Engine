#include "Board.hpp"
#include "Random.hpp"

BoardState::BoardState() {
	for (int i = 0; i < 6; ++i) {
		for (int j = 0; j < 7; ++j) {
			mBoard[i][j] = Empty;
		}
	}
}

std::vector<BoardState*> BoardState::GetPossibleMoves(SquareState player) const {
	std::vector<BoardState*> retVal;

	// for each column, find if a move is possible
	for (int col = 0; col < 7; ++col) {
		for (int row = 5; row >= 0; --row) {
			if (mBoard[row][col] == BoardState::Empty) {
				retVal.emplace_back(new BoardState(*this));
				retVal.back()->mBoard[row][col] = player;
				break;
			}
		}
	}

	return retVal;
}

bool BoardState::IsTerminal() const {
	// is the board full?
	if (IsFull()) {
		return true;
	}

	// is there a four-in-a-row?
	int fourInRow = GetFourInARow();
	if (fourInRow != 0) {
		return true;
	}

	return false;
}

float BoardState::GetScore() const {
	// if the board is full, the score is 0
	if (IsFull()) {
		return 0.0f;
	}

	// is there a four-in-a-row?
	int fourInRow = GetFourInARow();
	if (fourInRow != 0) {
		return static_cast<float>(fourInRow);
	}

	return CalculateHeuristic();
}

bool BoardState::IsFull() const {
	bool isFull = true;
	for (int i = 0; i < 6; ++i) {
		for (int j = 0; j < 7; ++j) {
			if (mBoard[i][j] == Empty) {
				isFull = false;
			}
		}
	}

	return isFull;
}

int BoardState::GetFourInARow() const {
	// returns -10 if yellow wins, 10 if red wins, 0 otherwise

	// check if there's a row with four in a row
	for (int row = 0; row < 6; ++row) {
		for (int col = 0; col < 4; ++col) {
			if (mBoard[row][col] == mBoard[row][col + 1] &&
				mBoard[row][col] == mBoard[row][col + 2] &&
				mBoard[row][col] == mBoard[row][col + 3]) {
				if (mBoard[row][col] == BoardState::Yellow) {
					return -10;
				}
				else if (mBoard[row][col] == BoardState::Red) {
					return 10;
				}
			}
		}
	}

	// check if there's a column with four in a row
	for (int col = 0; col < 7; ++col) {
		for (int row = 0; row < 3; ++row) {
			if (mBoard[row][col] == mBoard[row + 1][col] &&
				mBoard[row][col] == mBoard[row + 2][col] &&
				mBoard[row][col] == mBoard[row + 3][col]) {
				if (mBoard[row][col] == BoardState::Yellow) {
					return -10;
				}
				else if (mBoard[row][col] == BoardState::Red) {
					return 10;
				}
			}
		}
	}

	// check if there's a right diagonal four in a row
	for (int row = 0; row < 3; ++row) {
		for (int col = 0; col < 4; ++col) {
			if (mBoard[row][col] == mBoard[row + 1][col + 1] &&
				mBoard[row][col] == mBoard[row + 2][col + 2] &&
				mBoard[row][col] == mBoard[row + 3][col + 3]) {
				if (mBoard[row][col] == BoardState::Yellow) {
					return -10;
				}
				else if (mBoard[row][col] == BoardState::Red) {
					return 10;
				}
			}
		}
	}

	// check if there's a left diagonal four in a row
	for (int row = 5; row >= 3; --row) {
		for (int col = 0; col < 4; ++col) {
			if (mBoard[row][col] == mBoard[row - 1][col + 1] &&
				mBoard[row][col] == mBoard[row - 2][col + 2] &&
				mBoard[row][col] == mBoard[row - 3][col + 3]) {
				if (mBoard[row][col] == BoardState::Yellow) {
					return -10;
				}
				else if (mBoard[row][col] == BoardState::Red) {
					return 10;
				}
			}
		}
	}

	return 0;
}

float BoardState::CalculateHeuristic() const {
	// Red color is AI, Yellow color is player
	// find any sequence of 3 consecutive colors
	// if Red, then return 0.5f; otherwise return -0.5f;

	// check if there's a row with three in a row
	for (int row = 0; row < 6; ++row) {
		for (int col = 0; col < 5; ++col) {
			if (mBoard[row][col] == mBoard[row][col + 1] &&
				mBoard[row][col] == mBoard[row][col + 2]) {
				// check if both ends of this sequence are the opposite color
				if (mBoard[row][col] == BoardState::Yellow) {
					return -0.5f;
				}
				else if (mBoard[row][col] == BoardState::Red) {
					if ((col == 0 && mBoard[row][col + 3] == BoardState::Yellow) ||
						(col == 4 && mBoard[row][col - 1] == BoardState::Yellow) ||
						(mBoard[row][col - 1] == BoardState::Yellow && mBoard[row][col + 3] == BoardState::Yellow)) {
						// both ends are opposite color, bad move for AI
						return -0.25f;
					}
					else {
						return 0.5f;
					}
				}
			}
		}
	}

	// check if there's a column with three in a row
	for (int col = 0; col < 7; ++col) {
		for (int row = 0; row < 4; ++row) {
			if (mBoard[row][col] == mBoard[row + 1][col] &&
				mBoard[row][col] == mBoard[row + 2][col]) {
				// check if both ends of this sequence are the opposite color
				if (mBoard[row][col] == BoardState::Yellow) {
					return -0.5f;
				}
				else if (mBoard[row][col] == BoardState::Red) {
					if ((row == 0 && mBoard[row + 3][col] == BoardState::Yellow) ||
						(row == 3 && mBoard[row - 1][col] == BoardState::Yellow) ||
						(mBoard[row - 1][col] == BoardState::Yellow && mBoard[row + 3][col] == BoardState::Yellow)) {
						// both ends are opposite color, bad move for AI
						return -0.25f;
					}
					else {
						return 0.5f;
					}
				}
			}
		}
	}

	// check if there's a right diagonal three in a row
	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 5; ++col) {
			if (mBoard[row][col] == mBoard[row + 1][col + 1] &&
				mBoard[row][col] == mBoard[row + 2][col + 2]) {
				// check if both ends of this sequence are the opposite color
				if (mBoard[row][col] == BoardState::Yellow) {
					return -0.5f;
				}
				else if (mBoard[row][col] == BoardState::Red) {
					if ((row == 0 && col == 4) ||
						(row == 3 && col == 0) ||
						((row == 0 || col == 0) && mBoard[row + 3][col + 3] == BoardState::Yellow) ||
						((row == 3 || col == 4) && mBoard[row - 1][col - 1] == BoardState::Yellow) ||
						(mBoard[row - 1][col - 1] == BoardState::Yellow && mBoard[row + 3][col + 3] == BoardState::Yellow)) {
						// both ends are opposite color, bad move for AI
						return -0.25f;
					}
					else {
						return 0.5f;
					}
				}
			}
		}
	}

	// check if there's a left diagonal three in a row
	for (int row = 5; row >= 2; --row) {
		for (int col = 0; col < 5; ++col) {
			if (mBoard[row][col] == mBoard[row - 1][col + 1] &&
				mBoard[row][col] == mBoard[row - 2][col + 2]) {
				// check if both ends of this sequence are the opposite color
				if (mBoard[row][col] == BoardState::Yellow) {
					return -0.5f;
				}
				else if (mBoard[row][col] == BoardState::Red) {
					if ((row == 2 && col == 0) ||
						(row == 5 && col == 4) ||
						((row == 5 || col == 0) && mBoard[row - 3][col + 3] == BoardState::Yellow) ||
						((row == 2 || col == 4) && mBoard[row + 1][col - 1] == BoardState::Yellow) ||
						(mBoard[row + 1][col - 1] == BoardState::Yellow && mBoard[row - 3][col + 3] == BoardState::Yellow)) {
						// both ends are opposite color, bad move for AI
						return -0.25f;
					}
					else {
						return 0.5f;
					}
				}
			}
		}
	}

	return 0.0f;
}

bool TryPlayerMove(BoardState* state, int column) {
	// find the first row in that column that's available (if any)
	for (int row = 5; row >= 0; --row) {
		if (state->mBoard[row][column] == BoardState::Empty) {
			state->mBoard[row][column] = BoardState::Yellow;  // player is Yellow
			return true;
		}
	}

	return false;
}

void CPUMove(BoardState* state) {
	// use alpha-beta pruning with a depth limit to compute the next AI move	
	const BoardState* chosenMove = AlphaBetaDecideLimit(state, 8);
	*state = *chosenMove;
	delete chosenMove;
	
	/*
	// for now, this just randomly picks one of the possible moves
	std::vector<BoardState*> moves = state->GetPossibleMoves(BoardState::Red);  // computer AI is Red

	int index = Random::GetIntRange(0, moves.size() - 1);

	// copies over data from "moves[index]" pointer to "state" pointer
	*state = *moves[index];

	// clear up memory from possible moves
	for (auto move : moves) {
		delete move;  // note that "delete" deallocates the object pointed to by the pointers in "moves" list; since we have copied over the object data to "state" pointer, deleting pointers in "moves" is safe
	}
	*/
}

float AlphaBetaMaxLimit(const BoardState* state, int depth, float alpha, float beta) {
	if (depth == 0 || state->IsTerminal()) {
		return state->GetScore();
	}

	float maxValue = -std::numeric_limits<float>::infinity();
	std::vector<BoardState*> moves = state->GetPossibleMoves(BoardState::Red);

	for (const BoardState* child : moves) {
		maxValue = std::max(maxValue, AlphaBetaMinLimit(child, depth - 1, alpha, beta));
		if (maxValue >= beta) {
			for (auto child : moves) {
				delete child;
			}

			return maxValue;
		}

		alpha = std::max(maxValue, alpha);
	}

	for (auto child : moves) {
		delete child;
	}

	return maxValue;
}

float AlphaBetaMinLimit(const BoardState* state, int depth, float alpha, float beta) {
	if (depth == 0 || state->IsTerminal()) {
		return state->GetScore();
	}

	float minValue = std::numeric_limits<float>::infinity();
	std::vector<BoardState*> moves = state->GetPossibleMoves(BoardState::Yellow);

	for (const BoardState* child : moves) {
		minValue = std::min(minValue, AlphaBetaMaxLimit(child, depth - 1, alpha, beta));
		if (minValue <= alpha) {
			for (auto child : moves) {
				delete child;
			}

			return minValue;
		}

		beta = std::min(minValue, beta);
	}

	for (auto child : moves) {
		delete child;
	}

	return minValue;
}

const BoardState* AlphaBetaDecideLimit(const BoardState* root, int maxDepth) {
	const BoardState* choice = nullptr;

	float maxValue = -std::numeric_limits<float>::infinity();
	float beta = std::numeric_limits<float>::infinity();

	std::vector<BoardState*> moves = root->GetPossibleMoves(BoardState::Red);
	for (BoardState* child : moves) {
		float v = AlphaBetaMinLimit(child, maxDepth - 1, maxValue, beta);
		if (v > maxValue) {
			maxValue = v;
			choice = child;
		}
	}

	for (auto child : moves) {
		if (child != choice) {
			delete child;
		}
	}

	return choice;
}
