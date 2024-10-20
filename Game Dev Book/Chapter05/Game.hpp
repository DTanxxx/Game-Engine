#pragma once

#include <vector>
#include <unordered_map>
#include "SDL/SDL.h"
#include "GL/glew.h"
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

    //SDL_Texture* GetTexture(const std::string& fileName);
    class Texture* GetTexture(const std::string& fileName);

    void AddSprite(class SpriteComponent* sprite);
    void RemoveSprite(class SpriteComponent* sprite);

    void AddAsteroid(class Asteroid* ast);
    void RemoveAsteroid(class Asteroid* ast);
    std::vector<class Asteroid*> GetAsteroids();

private:
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();
    
    void LoadData();
    void UnloadData();

    bool LoadShaders();

    void CreateSpriteVerts();
    void InitSpriteVerts(const float* vertexBuffer, const unsigned int* indexBuffer);

private:
    SDL_Window* mWindow;
    bool mIsRunning;
    SDL_GLContext mContext;
    Uint32 mTicksCount;
    class VertexArray* mSpriteVerts;

    // all the actors in the game
    std::vector<class Actor*> mActors;
    // all pending actors
    std::vector<class Actor*> mPendingActors;
    
    // track if we are updating actors right now
    bool mUpdatingActors;

    // map of textures loaded
    //std::unordered_map<std::string, SDL_Texture*> mTextureMap;
    std::unordered_map<std::string, class Texture*> mTextureMap;

    // all the sprite components drawn
    std::vector<class SpriteComponent*> mSprites;

    class Shader* mSpriteShader;

    std::vector<class Asteroid*> mAsteroids;
    class Ship* mShip;

    std::vector<Vector3*> allColours;
    int currBGIndex;
    float colourTransitionDur;
    float curTransitionTimer;
};
