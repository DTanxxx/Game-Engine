#pragma once

#include <vector>
#include <unordered_map>
#include "SDL.h"
#include <string>
#include "Math.hpp"

class Game {
public:
    Game();

    bool Initialize();
    void RunLoop();
    void ShutDown();

    void AddActor(class Actor* actor);
    void RemoveActor(class Actor* actor);

    SDL_Texture* GetTexture(const std::string& fileName);

    void AddSprite(class SpriteComponent* sprite);
    void RemoveSprite(class SpriteComponent* sprite);

    class Grid* GetGrid() {
        return mGrid;
    }

    std::vector<class Enemy*>& GetEnemies() {
        return mEnemies;
    }

    class Enemy* GetNearestEnemy(const Vector2& pos);

private:
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();
    
    void LoadData();
    void UnloadData();

private:
    SDL_Window* mWindow;
    bool mIsRunning;
    SDL_Renderer* mRenderer;
    Uint32 mTicksCount;

    // all the actors in the game
    std::vector<class Actor*> mActors;
    // all pending actors
    std::vector<class Actor*> mPendingActors;
    
    // track if we are updating actors right now
    bool mUpdatingActors;

    // map of textures loaded
    std::unordered_map<std::string, SDL_Texture*> mTextureMap;

    // all the sprite components drawn
    std::vector<class SpriteComponent*> mSprites;

    // game-specific
    std::vector<class Enemy*> mEnemies;
    class Grid* mGrid;
    float mNextEnemy;
};
