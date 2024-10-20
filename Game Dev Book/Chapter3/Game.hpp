#pragma once

#include <vector>
#include <unordered_map>
#include "SDL.h"

class Game {
public:
    Game();

    bool Initialize();
    void RunLoop();
    void ShutDown();

    void AddActor(class Actor* actor);
    void RemoveActor(class Actor* actor);

    SDL_Texture* GetTexture(const char* fileName);

    void AddSprite(class SpriteComponent* sprite);
    void RemoveSprite(class SpriteComponent* sprite);

    // game-specific (add/remove asteroid)
    const std::vector<class Asteroid*>& GetAsteroids() const {
        return mAsteroids;
    }
    void AddAsteroid(class Asteroid* asteroid);
    void RemoveAsteroid(class Asteroid* asteroid);

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

    // game-specific
    class Ship* mShip;  // Player's ship
    std::vector<class Asteroid*> mAsteroids;

    // map of textures loaded
    std::unordered_map<const char*, SDL_Texture*> mTextureMap;

    // all the sprite components drawn
    std::vector<class SpriteComponent*> mSprites;
};
