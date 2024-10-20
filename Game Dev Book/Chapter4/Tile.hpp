#pragma once

#include "Actor.hpp"
#include <vector>

class Tile : public Actor {
public:
	friend class Grid;  // to give Grid class access to Tile class' private members
	enum TileState {
		EDefault,
		EPath,
		EStart,
		EBase
	};

	Tile(class Game* game);

	void SetTileState(TileState state);
	TileState GetTileState() const {
		return mTileState;
	}

	void ToggleSelect();
	const Tile* GetParent() const {
		return mParent;
	}

private:
	// for pathfinding
	std::vector<Tile*> mAdjacent;
	Tile* mParent;
	float f;
	float g;
	float h;
	bool mInOpenSet;
	bool mInClosedSet;
	bool mBlocked;

	void UpdateTexture();  // change texture based on this tile's state
	class SpriteComponent* mSprite;
	TileState mTileState;
	bool mSelected;
};