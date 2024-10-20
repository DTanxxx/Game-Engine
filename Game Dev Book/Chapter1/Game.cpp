#include "Game.hpp"

Game::Game() {
    mWindow = nullptr;
    mIsRunning = true;
    mRenderer = nullptr;

    mLeftPaddlePos.x = 5 + thickness;
    mLeftPaddlePos.y = 768 / 2;
    mRightPaddlePos.x = 1024 - 5 - thickness;
    mRightPaddlePos.y = 768 / 2;

    mTicksCount = 0;
}

bool Game::Initialize() {
    // ask SDL to initialize the video subsystem
    int sdlResult = SDL_Init(SDL_INIT_VIDEO);
    if (sdlResult != 0) {
        // initialization failed
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // create a window
    mWindow = SDL_CreateWindow(
        "Game Programming in C++ (Chapter 1)",  // window title
        100,  // top left x-coordinate of window
        100,  // top left y-coordinate of window
        1024,  // width of window
        768,  // height of window
        0  // flags (0 for no flags set)
    );

    if (!mWindow) {
        // window creation failed
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    // create a 2D graphics renderer
    mRenderer = SDL_CreateRenderer(
        mWindow,  // window to create renderer for
        -1,  // specifies which graphics driver to use; by default it is -1, which means to let SDL decide
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC  // initialization flags; here we want to use an accelerated renderer and to enable vertical synchronization
    );

    if (!mRenderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return false;
    }

    std::vector<Vector2> positions;
    std::vector<Vector2> velocities;
    for (int i = 0; i < numBalls; ++i) {
        Vector2 pos{
            1024 / 2,
            768 / 2
        };
        positions.push_back(pos);
    }
    
    for (int i = 0; i < numBalls; ++i) {
        float randomNum = (float)(rand()) / ((float)(RAND_MAX) / 100);
        Vector2 velocity{
            -200.0f + randomNum,
            235.0f + randomNum
        };
        velocities.push_back(velocity);
    }
    for (int i = 0; i < numBalls; ++i) {
        Ball ball{
            positions[i],
            velocities[i]
        };
        mBalls.push_back(ball);
    }

    return true;
}

void Game::RunLoop() {
    // keep running iterations of game loop until mIsRunning becomes false
    while (mIsRunning) {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::ShutDown() {
    // destroy window
    SDL_DestroyWindow(mWindow);
    // destroy renderer
    SDL_DestroyRenderer(mRenderer);
    // close SDL
    SDL_Quit();
}

void Game::ProcessInput() {
    // each input is represented by an event that resides in SDL's event queue
    // we can poll the event queue each frame and process each of the events

    SDL_Event event;
    // while there are still events in the queue
    while (SDL_PollEvent(&event)) {
        // SDL_PollEvent() returns true if there are still events in the event queue, and populates a SDL_Event data structure that is passed in by pointer
        switch (event.type) {
            // handle different event types here
            case SDL_QUIT:
                // user has given input to close the window, end game loop
                mIsRunning = false;
                break;
        }
    }

    // get state of keyboard
    const Uint8* state = SDL_GetKeyboardState(NULL);
    // if Escape key is pressed, also end game loop
    if (state[SDL_SCANCODE_ESCAPE]) {
        // the value for the corresponding Escape key in the keyboard state array is true, so user has pressed the Escape key to quit program
        mIsRunning = false;
    }

    // mLeftPaddleDir is 0 if the paddle doesn't move, -1 if the paddle moves up (-ve y), and 1 if the paddle moves down (+ve y)
    // if player presses both move-up (W) and move-down key (S) then the paddle doesn't move
    mLeftPaddleDir = 0;
    if (state[SDL_SCANCODE_W]) {
        mLeftPaddleDir -= 1;  // move up
    }
    if (state[SDL_SCANCODE_S]) {
        mLeftPaddleDir += 1;  // move down
    }

    // do the same for the right paddle's input
    mRightPaddleDir = 0;
    if (state[SDL_SCANCODE_I]) {
        mRightPaddleDir -= 1;
    }
    if (state[SDL_SCANCODE_K]) {
        mRightPaddleDir += 1;
    }
}

void Game::UpdateGame() {
    // wait until 16ms (target frame rate is 60 FPS) has elapsed since last frame - frame limiting, which forces the game loop to wait until a target delta time has elapsed
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16)) {
    }

    // delta time is the difference in ticks from last frame (converted to seconds)
    float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;  // divide by 1000 because SDL_GetTicks() returns an elapsed time in milliseconds
    
    // clamp maximum delta time value (delta time can get really large when you step through the game with a debugger and pause at a breakpoint)
    if (deltaTime > 0.05f) {
        deltaTime = 0.05f;
    }

    // update tick counts (for next frame)
    mTicksCount = SDL_GetTicks();

    // update objects in game world as function of delta time
    if (mLeftPaddleDir != 0) {
        // update paddle's y position at a speed of 300 pixels/second
        mLeftPaddlePos.y += mLeftPaddleDir * 300.0f * deltaTime;
        // make sure paddle doesn't move off screen!
        if (mLeftPaddlePos.y < (paddleH / 2.0f + thickness)) {
            mLeftPaddlePos.y = paddleH / 2.0f + thickness;
        }
        else if (mLeftPaddlePos.y > (768.0f - paddleH / 2.0f - thickness)) {
            mLeftPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
        }
    }

    if (mRightPaddleDir != 0) {
        mRightPaddlePos.y += mRightPaddleDir * 300.0f * deltaTime;
        if (mRightPaddlePos.y < (paddleH / 2.0f + thickness)) {
            mRightPaddlePos.y = paddleH / 2.0f + thickness;
        }
        else if (mRightPaddlePos.y > (768.0f - paddleH / 2.0f - thickness)) {
            mRightPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
        }
    }

    // update the ball's position in terms of the velocity
    for (int i = 0; i < numBalls; ++i) {
        Vector2 pos = mBalls[i].position;
        Vector2 vel = mBalls[i].velocity;

        Vector2 newPos;
        newPos.x = pos.x + vel.x * deltaTime;
        newPos.y = pos.y + vel.y * deltaTime;
        mBalls[i].position = newPos;

        Vector2 newVel{
            vel.x,
            vel.y
        };

        // collision detection between ball and wall
        if (newPos.y <= thickness && vel.y < 0.0f) {
            // if ball's y position is less than the thickness of wall and ball is moving towards the top wall, that means the ball has collided with top wall
            // in this case, we want to negate the ball's y velocity so it travels down
            newVel.y = vel.y * -1;
        }
        if (newPos.y >= 768.0f - thickness && vel.y > 0.0f) {
            // similar check for collision with bottom wall
            newVel.y = vel.y * -1;
        }

        // collision detection between ball and paddles
        float diff = fabs((double)(newPos.y) - (double)(mLeftPaddlePos.y));  // calculate the absolute difference between ball's y position and paddle's y position
        if (diff <= paddleH / 2.0f && newPos.x <= 25.0f && newPos.x >= 20.0f && newVel.x < 0.0f) {
            // if the absolute difference calculated above is greater than half the height of the paddle, then the ball is either too high or too low; otherwise
            // the ball is in the paddle's vertical range
            // also check if the ball's x position lines up with the paddle, and if the ball is moving towards the paddle
            newVel.x *= -1.0f;
        }

        diff = fabs((double)(newPos.y) - (double)(mRightPaddlePos.y));
        if (diff <= paddleH / 2.0f && newPos.x >= 1024 - 25.0f && newPos.x <= 1024 - 20.0f && newVel.x > 0.0f) {
            newVel.x *= -1.0f;
        }

        mBalls[i].velocity = newVel;
    }
}

void Game::GenerateOutput() {
    // render graphics
    // 1. clear the back buffer to a color (the game's current buffer)
    // 2. draw the entire game scene
    // 3. swap the front buffer and back buffer

    // step 1: to clear the back buffer, we need to first specify a color to draw (we will use blue here)...
    SDL_SetRenderDrawColor(
        mRenderer,
        0,  // R
        0,  // G
        255,  // B
        255  // Alpha
    );

    // ...then we need to clear the back buffer to the current draw color 
    SDL_RenderClear(mRenderer);

    // step 2: we will draw the scene for Pong game
    // first, we will change the draw color to white to be used for the ball, paddles, and walls
    SDL_SetRenderDrawColor(
        mRenderer,
        255,
        255,
        255,
        255
    );

    // then we need to specify dimensions for our wall rectangle geometries
    // note: the top-left corner of the screen is (0,0)
    SDL_Rect wallTop{
        0,  // top left x coordinate
        0,  // top left y coordinate
        1024,  // window width
        thickness  // wall height
    };

    // then we need to actually draw this wall rectangle using the new draw color
    SDL_RenderFillRect(mRenderer, &wallTop);

    // do the same for the bottom wall
    SDL_Rect wallBottom{
        0,
        768 - thickness,
        1024,
        thickness
    };

    SDL_RenderFillRect(mRenderer, &wallBottom);

    // now we will draw the balls...
    for (int i = 0; i < numBalls; ++i) {
        SDL_Rect ball{
        static_cast<int>(mBalls[i].position.x - thickness / 2),
        static_cast<int>(mBalls[i].position.y - thickness / 2),
        thickness,
        thickness
        };

        SDL_RenderFillRect(mRenderer, &ball);
    }

    // ...and the two paddles
    SDL_Rect leftPaddle{
        static_cast<int>(mLeftPaddlePos.x - thickness / 2),
        static_cast<int>(mLeftPaddlePos.y - paddleH / 2),
        thickness,
        paddleH
    };

    SDL_RenderFillRect(mRenderer, &leftPaddle);

    SDL_Rect rightPaddle{
        static_cast<int>(mRightPaddlePos.x - thickness / 2),
        static_cast<int>(mRightPaddlePos.y - paddleH / 2),
        thickness,
        paddleH
    };

    SDL_RenderFillRect(mRenderer, &rightPaddle);

    // step 3: swap the front and back buffers
    SDL_RenderPresent(mRenderer);
}