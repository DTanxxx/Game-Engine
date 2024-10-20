#pragma once

#include <vector>
#include "SDL/SDL.h"
#include <string>
#include "Math.hpp"
#include <unordered_map>

class Game {
public:
    Game();

    bool Initialize();
    void RunLoop();
    void ShutDown();

    void AddActor(class Actor* actor);
    void RemoveActor(class Actor* actor);

    void AddSprite(class SpriteComponent* sprite);
    void RemoveSprite(class SpriteComponent* sprite);

    class Texture* GetTexture(const std::string& fileName);

    void AddAsteroid(class Asteroid* asteroid);
    void RemoveAsteroid(class Asteroid* asteroid);

    std::vector<class Asteroid*>& GetAsteroids() {
        return mAsteroids;
    }

private:
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();

    bool LoadShaders();
    void CreateSpriteVerts();
    
    void LoadData();
    void UnloadData();

private:
    bool mIsRunning;
    Uint32 mTicksCount;

    // track if we are updating actors right now
    bool mUpdatingActors;

    class InputSystem* mInputSystem;

    // all the actors in the game
    std::vector<class Actor*> mActors;
    // all pending actors
    std::vector<class Actor*> mPendingActors;

    std::unordered_map<std::string, class Texture*> mTextures;
    std::vector<class SpriteComponent*> mSprites;

    class Shader* mSpriteShader;
    class VertexArray* mSpriteVerts;

    SDL_Window* mWindow;
    SDL_GLContext mContext;
   
    std::vector<class Ship*> mShips;
    std::vector<class Asteroid*> mAsteroids;
};
