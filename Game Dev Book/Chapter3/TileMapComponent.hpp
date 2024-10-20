#pragma once

#include "SpriteComponent.hpp"
#include <vector>
#include "Math.hpp"

// for tilemap layers
class TileMapComponent : public SpriteComponent {
public:
	TileMapComponent(class Actor* owner, int drawOrder = 10) : SpriteComponent(owner, drawOrder) {}

	void Draw(SDL_Renderer* renderer) override;
	void SetTexture(SDL_Texture* texture) override;

	void SetScreenSize(const Vector2& size) {
		mScreenSize = size;
	}

	void LoadTileMapCSV(const char* fileName);

private:
	const int mNumCols = 8;
	int mNumRows;
	float mGridSize;
	Vector2 mScreenSize;

	std::vector<std::vector<int>> mRows;
};