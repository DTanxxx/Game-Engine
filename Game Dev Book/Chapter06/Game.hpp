#pragma once

#include <vector>
#include "SDL/SDL.h"

class Game {
public:
    Game();

    bool Initialize();
    void RunLoop();
    void ShutDown();

    void AddActor(class Actor* actor);
    void RemoveActor(class Actor* actor);

    class Renderer* GetRenderer() const {
        return mRenderer;
    }

private:
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();
    
    void LoadData();
    void UnloadData();

private:
    bool mIsRunning;
    Uint32 mTicksCount;

    // track if we are updating actors right now
    bool mUpdatingActors;

    class Renderer* mRenderer;
    class CameraActor* mCameraActor;

    // all the actors in the game
    std::vector<class Actor*> mActors;
    // all pending actors
    std::vector<class Actor*> mPendingActors;
};
