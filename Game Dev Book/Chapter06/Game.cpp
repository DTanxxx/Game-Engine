#include "Game.hpp"
#include "Actor.hpp"
#include "CameraActor.hpp"
#include "PlaneActor.hpp"
#include "Renderer.hpp"
#include <algorithm>
#include "Math.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"
#include <string>
#include "MeshComponent.hpp"
#include "SpriteComponent.hpp"

Game::Game() {
    mIsRunning = true;
    mUpdatingActors = false;
    mTicksCount = 0;
    mRenderer = nullptr;
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
        delete mRenderer;
        mRenderer = nullptr;
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

    // create 4 point lights
    PointLight pt = PointLight();
    pt.mPosition = Vector3(200.0f, -100.0f, 0.0f);
    pt.mRadius = 1000.0f;
    pt.mSpecColor = Vector3(0.8f, 0.0f, 0.0f);
    pt.mDiffuseColor = Vector3(1.0f, 0.0f, 0.0f);
    pt.mSpecPower = 10.0f;
    mRenderer->AddPointLight(pt);

    pt = PointLight();
    pt.mPosition = Vector3(200.0f, -50.0f, 0.0f);
    pt.mRadius = 100.0f;
    pt.mSpecColor = Vector3(0.0f, 0.8f, 0.0f);
    pt.mDiffuseColor = Vector3(0.0f, 1.0f, 0.0f);
    pt.mSpecPower = 5.0f;
    mRenderer->AddPointLight(pt);

    pt = PointLight();
    pt.mPosition = Vector3(200.0f, 150.0f, 100.0f);
    pt.mRadius = 100.0f;
    pt.mSpecColor = Vector3(0.0f, 0.0f, 0.8f);
    pt.mDiffuseColor = Vector3(0.0f, 0.0f, 1.0f);
    pt.mSpecPower = 10.0f;
    mRenderer->AddPointLight(pt);

    pt = PointLight();
    pt.mPosition = Vector3(200.0f, 150.0f, 0.0f);
    pt.mRadius = 50.0f;
    pt.mSpecColor = Vector3(0.5f, 0.8f, 0.5f);
    pt.mDiffuseColor = Vector3(0.7f, 1.0f, 0.1f);
    pt.mSpecPower = 5.0f;
    mRenderer->AddPointLight(pt);

    // camera actor
    mCameraActor = new CameraActor(this);

    // UI elements
    a = new Actor(this);
    a->SetPosition(Vector3(-350.0f, -350.0f, 0.0f));
    
    SpriteComponent* sc = new SpriteComponent(a);
    sc->SetTexture(mRenderer->GetTexture("Assets/HealthBar.png"));

    a = new Actor(this);
    a->SetPosition(Vector3(375.0f, -275.0f, 0.0f));
    a->SetScale(0.75f);
    
    sc = new SpriteComponent(a);
    sc->SetTexture(mRenderer->GetTexture("Assets/Radar.png"));
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
    mRenderer->Draw();
}
