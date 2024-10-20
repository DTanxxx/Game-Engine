#include "TileMapComponent.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include "Actor.hpp"

void TileMapComponent::SetTexture(SDL_Texture* texture) {
	SpriteComponent::SetTexture(texture);

	mGridSize = static_cast<float>(mTexWidth) / static_cast<float>(mNumCols);
	mNumRows = static_cast<int>(static_cast<float>(mTexHeight) / mGridSize);
}

void TileMapComponent::LoadTileMapCSV(const char* fileName) {
	std::string name{ fileName };
	std::ifstream input{ name };

	if (!input.is_open()) {
		SDL_Log("Couldn't read file: %s", fileName);
		return;
	}

	for (std::string line; std::getline(input, line);) {
		std::istringstream ss(std::move(line));
		std::vector<int> row;
		if (!mRows.empty()) {
			row.reserve(mRows.front().size());
		}

		for (std::string value; std::getline(ss, value, ',');) {
			row.push_back(stoi(std::move(value)));
		}
		mRows.push_back(std::move(row));
	}
}

void TileMapComponent::Draw(SDL_Renderer* renderer) {
	for (int i = 0; i < mRows.size(); ++i) {
		for (int j = 0; j < mRows[i].size(); ++j) {
			if (mRows[i][j] == -1) {
				continue;
			}

			SDL_Rect destR;
			destR.w = static_cast<int>(mGridSize);
			destR.h = static_cast<int>(mGridSize);
			destR.x = static_cast<int>(mOwner->GetPosition().x - mScreenSize.x / 2 + j * mGridSize);
			destR.y = static_cast<int>(mOwner->GetPosition().y - mScreenSize.y / 2 + i * mGridSize);

			SDL_Rect srcR;
			srcR.w = static_cast<int>(mGridSize);
			srcR.h = static_cast<int>(mGridSize);

			int srcIndex = mRows[i][j];
			int rowIndex = static_cast<int>(srcIndex / mNumCols);
			int colIndex = srcIndex % mNumCols;

			srcR.x = static_cast<int>(colIndex * mGridSize);
			srcR.y = static_cast<int>(rowIndex * mGridSize);

			SDL_RenderCopyEx(renderer,
				mTexture,
				&srcR,
				&destR,
				-Math::ToDegrees(mOwner->GetRotation()),
				nullptr, 
				SDL_FLIP_NONE);
		}
	}
}