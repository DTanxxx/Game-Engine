#pragma once

#include "SDL.h"
#include <unordered_map>
#include <string>
#include <vector>
#include "Math.hpp"
#include "Board.hpp"

class Game {
public:
	Game();

	bool Initialize();
	void RunLoop();
	void Shutdown();

	void AddActor(class Actor* actor);
	void RemoveActor(class Actor* actor);

	void AddSprite(class SpriteComponent* sprite);
	void RemoveSprite(class SpriteComponent* sprite);

	SDL_Texture* GetTexture(const std::string& fileName);

	// helper function to draw a texture without sprite components
	void DrawTexture(SDL_Texture* texture, const Vector2& pos, const Vector2& size);

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	void LoadData();
	void UnloadData();

	// map of textures loaded
	std::unordered_map<std::string, SDL_Texture*> mTextures;

	// all the actors in the game
	std::vector<class Actor*> mActors;
	// any pending actors
	std::vector<class Actor*> mPendingActors;

	// all the sprite components drawn
	std::vector<class SpriteComponent*> mSprites;

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	Uint32 mTicksCount;
	bool mIsRunning;

	// track if we are updating actors right now
	bool mUpdatingActors;

	// game-specific
	BoardState mBoardState;
};