#pragma once

#include "SDL.h"
#include "Vector2.hpp"
#include "Ball.hpp"
#include <vector>

class Game {
    public:
        Game();
        // initialize the game
        bool Initialize();
        // runs the game loop until the game is over
        void RunLoop();
        // shut down the game
        void ShutDown();

    private:
        // helper functions for the game loop
        void ProcessInput();
        void UpdateGame();
        void GenerateOutput();

        // window created by SDL
        SDL_Window* mWindow;
        // game should continue to run
        bool mIsRunning;
        SDL_Renderer* mRenderer;

        // wall's thickness
        const int thickness = 15;

        // center point positions for paddles
        Vector2 mLeftPaddlePos;
        Vector2 mRightPaddlePos;

        // amount of elapsed game time in milliseconds
        Uint32 mTicksCount;

        // used to control where the paddles should move
        int mLeftPaddleDir;
        int mRightPaddleDir;

        // paddle's height
        const int paddleH = 100;
        const int numBalls = 3;

        std::vector<Ball> mBalls;
};
