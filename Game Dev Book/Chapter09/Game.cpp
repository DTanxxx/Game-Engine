#include "Game.hpp"
#include "Actor.hpp"
#include "PlaneActor.hpp"
#include "Renderer.hpp"
#include "AudioSystem.hpp"
#include <algorithm>
#include "Math.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"
#include <string>
#include "MeshComponent.hpp"
#include "MoveComponent.hpp"
#include "SpriteComponent.hpp"
#include "AudioComponent.hpp"
#include "FPSActor.hpp"

Game::Game() {
    mIsRunning = true;
    mUpdatingActors = false;
    mTicksCount = 0;
    mRenderer = nullptr;
    mAudioSystem = nullptr;
}

bool Game::Initialize() {
    int sdlResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    if (sdlResult != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // create the renderer
    mRenderer = new Renderer(this);
    if (!mRenderer->Initialize(1024.0f, 768.0f)) {
        SDL_Log("Failed to initialize renderer");
        mRenderer->Shutdown();
        delete mRenderer;
        mRenderer = nullptr;
        return false;
    }

    // create the audio system
    mAudioSystem = new AudioSystem(this);
    if (!mAudioSystem->Initialize()) {
        SDL_Log("Failed to initialize audio system");
        mAudioSystem->Shutdown();
        delete mAudioSystem;
        mAudioSystem = nullptr;
        return false;
    }

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
    
    if (mRenderer) {
        mRenderer->Shutdown();
    }

    if (mAudioSystem) {
        mAudioSystem->Shutdown();
    }

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
    // create actors
    Actor* a = new Actor(this);
    a->SetPosition(Vector3(200.0f, 75.0f, 0.0f));
    a->SetScale(100.0f);
    
    Quaternion q(Vector3::UnitY, -Math::PiOver2);
    q = Quaternion::Concatenate(q, Quaternion(Vector3::UnitZ, Math::Pi + Math::Pi / 4.0f));
    a->SetRotation(q);

    MeshComponent* mc = new MeshComponent(a, mRenderer->GetMesh("Assets/Cube.gpmesh"));
    
    a = new Actor(this);
    a->SetPosition(Vector3(200.0f, -75.0f, 0.0f));
    a->SetScale(3.0f);
    
    mc = new MeshComponent(a, mRenderer->GetMesh("Assets/Sphere.gpmesh"));

    // set up floor
    const float start = -1250.0f;
    const float size = 250.0f;
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            a = new PlaneActor(this);
            a->SetPosition(Vector3(start + i * size, start + j * size, -100.0f));
        }
    }

    // left/right walls
    q = Quaternion(Vector3::UnitX, Math::PiOver2);
    for (int i = 0; i < 10; ++i) {
        a = new PlaneActor(this);
        a->SetPosition(Vector3(start + i * size, start - size, 0.0f));
        a->SetRotation(q);

        a = new PlaneActor(this);
        a->SetPosition(Vector3(start + i * size, -start + size, 0.0f));
        a->SetRotation(q);
    }

    q = Quaternion::Concatenate(q, Quaternion(Vector3::UnitZ, Math::PiOver2));
    // forward/back walls
    for (int i = 0; i < 10; ++i) {
        a = new PlaneActor(this);
        a->SetPosition(Vector3(start - size, start + i * size, 0.0f));
        a->SetRotation(q);

        a = new PlaneActor(this);
        a->SetPosition(Vector3(-start + size, start + i * size, 0.0f));
        a->SetRotation(q);
    }

    // set up lights
    mRenderer->SetAmbientLight(Vector3(0.2f, 0.2f, 0.2f));
    DirectionalLight& dir = mRenderer->GetDirectionalLight();
    dir.mDirection = Vector3(0.0f, -0.707f, -0.707f);
    dir.mDiffuseColor = Vector3(0.78f, 0.88f, 1.0f);
    dir.mSpecColor = Vector3(0.8f, 0.8f, 0.8f);

    // UI elements
    a = new Actor(this);
    a->SetPosition(Vector3(-350.0f, -350.0f, 0.0f));
    
    SpriteComponent* sc = new SpriteComponent(a);
    sc->SetTexture(mRenderer->GetTexture("Assets/HealthBar.png"));

    a = new Actor(this);
    a->SetPosition(Vector3(-390.0f, 275.0f, 0.0f));
    a->SetScale(0.75f);
    
    sc = new SpriteComponent(a);
    sc->SetTexture(mRenderer->GetTexture("Assets/Radar.png"));

    a = new Actor(this);
    a->SetScale(2.0f);

    mCrosshair = new SpriteComponent(a);
    mCrosshair->SetTexture(mRenderer->GetTexture("Assets/Crosshair.png"));
    
    // start music
    mMusicEvent = mAudioSystem->PlayEvent("event:/Music");

    // FPS Camera ===================================================
    // enable relative mouse mode for camera look
    SDL_SetRelativeMouseMode(SDL_TRUE);
    // make an initial call to get relative data to clear out
    SDL_GetRelativeMouseState(nullptr, nullptr);

    // different camera actors
    mFPSActor = new FPSActor(this);
    // ==============================================================
}

void Game::UnloadData() {
    // delete actors
    // because ~Actor calls RemoveActor, have to use a different style loop
    while (!mActors.empty()) {
        delete mActors.back();
    }

    if (mRenderer) {
        mRenderer->UnloadData();
    }
}

void Game::ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            mIsRunning = false;
            break;
        // this fires when a key is initially pressed
        case SDL_KEYDOWN:
            if (!event.key.repeat) {
                HandleKeyPress(event.key.keysym.sym);
            }
            break;
        default:
            break;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_ESCAPE]) {
        mIsRunning = false;
    }

    // loop over all actors and call ProcessInput() on each one
    mUpdatingActors = true;
    for (auto actor : mActors) {
        actor->ProcessInput(state);
    }
    mUpdatingActors = false;
}

void Game::HandleKeyPress(int key) {
    float volume = 0.0f;

    switch (key) {
    case '-':
        // reduce master volume
        volume = mAudioSystem->GetBusVolume("bus:/");
        volume = Math::Max(0.0f, volume - 0.1f);
        mAudioSystem->SetBusVolume("bus:/", volume);
        break;
    case '=':
        // increase master volume
        volume = mAudioSystem->GetBusVolume("bus:/");
        volume = Math::Min(1.0f, volume + 0.1f);
        mAudioSystem->SetBusVolume("bus:/", volume);
        break;
    default:
        break;
    }
}

void Game::UpdateGame() {
    // frame limiting
    // wait until 16ms has elapsed since last frame
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

    // update audio system
    mAudioSystem->Update(deltaTime);
}

void Game::GenerateOutput() {
    mRenderer->Draw();
}
