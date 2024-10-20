#pragma once

#include "Actor.hpp"
#include <vector>

class Grid : public Actor {
public:
	Grid(class Game* game);

	// handle a mouse click at the x/y screen locations
	void ProcessClick(int x, int y);

	// use A* to find a path
	bool FindPath(class Tile* start, class Tile* goal);

	// try to build a tower
	void BuildTower();

	// get start/end tile
	class Tile* GetStartTile();
	class Tile* GetEndTile();

	void UpdateActor(float deltaTime) override;

private:
	// select a specific tile
	void SelectTile(size_t row, size_t col);

	// update textures for tiles on path
	void UpdatePathTiles(class Tile* start);
	
	// currently selected tile
	class Tile* mSelectedTile;

	// 2D vector of tiles in grid
	std::vector<std::vector<class Tile*>> mTiles;

	// time until next enemy
	float mNextEnemy;

	// rows/columns in grid
	const size_t NumRows = 7;
	const size_t NumCols = 16;

	// start y position of top left corner
	const float StartY = 192.0f;

	// width/height of each tile
	const float TileSize = 64.0f;

	// time between enemy spawns
	const float EnemyTime = 1.5f;
};