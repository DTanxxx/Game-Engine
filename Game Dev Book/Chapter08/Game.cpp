#include "Game.hpp"
#include "Actor.hpp"
#include <algorithm>
#include <GL/glew.h>
#include "Shader.hpp"
#include "Random.hpp"
#include "Math.hpp"
#include "Texture.hpp"
#include "InputSystem.hpp"
#include <string>
#include "MoveComponent.hpp"
#include "SpriteComponent.hpp"
#include "VertexArray.hpp"
#include "Ship.hpp"
#include "Asteroid.hpp"

Game::Game() {
    mIsRunning = true;
    mUpdatingActors = false;
    mTicksCount = 0;

    mWindow = nullptr;
    mSpriteShader = nullptr;
}

bool Game::Initialize() {
    int sdlResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER);
    if (sdlResult != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // set OpenGL attributes
    // use the core OpenGL profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    // specify version 3.3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    // request a color buffer with 8-bits per RGBA channel
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    // enable double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // force OpenGL to use hardware acceleration
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    mWindow = SDL_CreateWindow("Game Programming in C++ (Chapter 8)",
        100, 100, 1024, 768, SDL_WINDOW_OPENGL);

    if (!mWindow) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    // initialize input system
    mInputSystem = new InputSystem();
    if (!mInputSystem->Initialize()) {
        SDL_Log("Failed to initialize input system");
        return false;
    }

    // create an OpenGL context
    mContext = SDL_GL_CreateContext(mWindow);

    // initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        SDL_Log("Failed to initialize GLEW");
        return false;
    }

    // on some platforms, GLEW will emit a benign error code, so clear it
    glGetError();

    // make sure we can craete/compile shaders
    if (!LoadShaders()) {
        SDL_Log("Failed to load shaders");
        return false;
    }

    // create quad for drawing sprites
    CreateSpriteVerts();

    LoadData();

    mTicksCount = SDL_GetTicks();

    return true;
}

void Game::RunLoop() {
    while (mIsRunning) {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::ShutDown() {
    UnloadData();
    
    if (mInputSystem) {
        mInputSystem->Shutdown();
        delete mInputSystem;
    }

    delete mSpriteVerts;
    mSpriteShader->Unload();
    delete mSpriteShader;
    
    SDL_GL_DeleteContext(mContext);
    SDL_DestroyWindow(mWindow);

    SDL_Quit();
}

void Game::AddActor(Actor* actor) {
    // if updating actors, need to add to pending so that we don't mess up the iteration over mActors vector
    if (mUpdatingActors) {
        mPendingActors.emplace_back(actor);
    }
    else {
        mActors.emplace_back(actor);
    }
}

void Game::RemoveActor(Actor* actor) {
    // is it in pending actors?
    auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
    if (iter != mPendingActors.end()) {
        // swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mPendingActors.end() - 1);
        mPendingActors.pop_back();
        return;
    }

    // is it in actors?
    iter = std::find(mActors.begin(), mActors.end(), actor);
    if (iter != mActors.end()) {
        // swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mActors.end() - 1);
        mActors.pop_back();
    }
}

void Game::LoadData() {
    // create player ship(s)
    std::vector<ControllerState> controllers = mInputSystem->GetState().Controllers;
    for (int i = 0; i < controllers.size(); ++i) {
        if (controllers[i].GetIsConnected()) {
            // create a ship for this controller
            Ship* ship = new Ship(this, controllers[i].GetControllerIndex());
            ship->SetRotation(Math::PiOver2);
            mShips.push_back(ship);
        }
    }

    // create asteroids
    const int numAsteroids = 20;
    for (int i = 0; i < numAsteroids; ++i) {
        new Asteroid(this);
    }
}

void Game::UnloadData() {
    // delete actors
    // because ~Actor calls RemoveActor, have to use a different style loop
    while (!mActors.empty()) {
        delete mActors.back();
    }

    // destroy textures
    for (auto i : mTextures) {
        i.second->Unload();
        delete i.second;
    }
    mTextures.clear();
}

void Game::ProcessInput() {
    mInputSystem->PrepareForUpdate();

    // SDL_PollEvent loop
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            mIsRunning = false;
            break;
        default:
            int controllerIndex = -1;
            mInputSystem->ProcessEvent(event, controllerIndex);

            if (controllerIndex >= 0) {
                // add or remove ship accordingly
                SDL_Log("Controller added/removed %i", controllerIndex);

                if (event.type == SDL_CONTROLLERDEVICEADDED) {
                    if (mShips.size() < 4) {
                        Ship* ship = new Ship(this, controllerIndex);
                        ship->SetRotation(Math::PiOver2);
                        mShips.push_back(ship);
                    }
                }
                else if (event.type == SDL_CONTROLLERDEVICEREMOVED) {
                    std::vector<Ship*> newShips;
                    for (int i = 0; i < mShips.size(); ++i) {
                        if (mShips[i] != nullptr && mShips[i]->GetControllerIndex() != controllerIndex) {
                            newShips.emplace_back(mShips[i]);
                        }
                    }
                    mShips = newShips;
                }
            }
            
            break;
        }
    }

    mInputSystem->Update();
    const InputState& state = mInputSystem->GetState();

    if (state.Keyboard.GetKeyState(SDL_SCANCODE_ESCAPE) == EReleased) {
        mIsRunning = false;
    }

    // loop over all actors and call ProcessInput() on each one
    mUpdatingActors = true;
    for (auto actor : mActors) {
        actor->ProcessInput(state);
    }
    mUpdatingActors = false;
}

void Game::UpdateGame() {
    // frame limiting
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16)) {
    }

    // compute delta time
    float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
    if (deltaTime > 0.05f) {
        deltaTime = 0.05f;
    }

    mTicksCount = SDL_GetTicks();

    // update all actors
    mUpdatingActors = true;
    for (auto actor : mActors) {
        actor->Update(deltaTime);
    }
    mUpdatingActors = false;

    // move any pending actors to mActors
    for (auto pending : mPendingActors) {
        // make sure any actors created while updating other actors have their world transform 
        // calculated in the same frame where they are created
        pending->ComputeWorldTransform();
        mActors.emplace_back(pending);
    }
    mPendingActors.clear();

    // add any dead actors to a temp vector
    std::vector<Actor*> deadActors;
    for (auto actor : mActors) {
        if (actor->GetState() == Actor::EDead) {
            deadActors.emplace_back(actor);
        }
    }

    // delete dead actors (which removes them from mActors)
    for (auto actor : deadActors) {
        delete actor;
    }
}

void Game::GenerateOutput() {
    // set the clear color to grey
    glClearColor(0.86f, 0.86f, 0.86f, 1.0f);
    // clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // draw all sprite components
    // enable alpha blending on the color buffer
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // set shader/vao as active
    mSpriteShader->SetActive();
    mSpriteVerts->SetActive();
    for (auto sprite : mSprites) {
        sprite->Draw(mSpriteShader);
    }

    // swap the buffers
    SDL_GL_SwapWindow(mWindow);
}

bool Game::LoadShaders() {
    mSpriteShader = new Shader();
    if (!mSpriteShader->Load("Shaders/Sprite.vert", "Shaders/Sprite.frag")) {
        return false;
    }

    mSpriteShader->SetActive();
    // set the view-projection matrix
    Matrix4 viewProj = Matrix4::CreateSimpleViewProj(1024.0f, 768.0f);
    mSpriteShader->SetMatrixUniform("uViewProj", viewProj);
    return true;
}

void Game::CreateSpriteVerts() {
    float vertices[] = {
        -0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.0, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    mSpriteVerts = new VertexArray(vertices, 4, indices, 6);
}

Texture* Game::GetTexture(const std::string& fileName) {
    Texture* tex = nullptr;
    auto iter = mTextures.find(fileName);
    if (iter != mTextures.end()) {
        tex = iter->second;
    }
    else {
        tex = new Texture();
        if (tex->Load(fileName)) {
            mTextures.emplace(fileName, tex);
        }
        else {
            delete tex;
            tex = nullptr;
        }
    }
    return tex;
}

void Game::AddAsteroid(Asteroid* asteroid) {
    mAsteroids.emplace_back(asteroid);
}

void Game::RemoveAsteroid(Asteroid* asteroid) {
    auto iter = std::find(mAsteroids.begin(), mAsteroids.end(), asteroid);
    if (iter != mAsteroids.end()) {
        mAsteroids.erase(iter);
    }
}

void Game::AddSprite(SpriteComponent* sprite) {
    // find the insertion point in the sorted vector (the first element with a higher draw order than me)
    int myDrawOrder = sprite->GetDrawOrder();
    auto iter = mSprites.begin();
    for (; iter != mSprites.end(); iter++) {
        if (myDrawOrder < (*iter)->GetDrawOrder()) {
            break;
        }
    }

    // insert element before position of iter
    mSprites.insert(iter, sprite);
}

void Game::RemoveSprite(SpriteComponent* sprite) {
    auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
    mSprites.erase(iter);
}