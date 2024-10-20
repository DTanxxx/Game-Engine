#include "Game.hpp"
#include "Actor.hpp"
#include "SpriteComponent.hpp"
#include "BGSpriteComponent.hpp"
#include "TileMapComponent.hpp"
#include "SDL_image.h"
#include "Ship.hpp"
#include "Dummy.hpp"
#include <algorithm>

Game::Game() {
    mWindow = nullptr;
    mIsRunning = true;
    mRenderer = nullptr;
    mUpdatingActors = false;
    mTicksCount = 0;
}

bool Game::Initialize() {
    int sdlResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    if (sdlResult != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow(
        "Game Programming in C++ (Chapter 2)",
        100,
        100,
        1024,
        768,
        0
    );

    if (!mWindow) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(
        mWindow,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!mRenderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return false;
    }

    int sdlImageResult = IMG_Init(IMG_INIT_PNG);
    if (sdlImageResult == 0) {
        SDL_Log("Unable to initialize SDL Image: %s", SDL_GetError());
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
    IMG_Quit();
    SDL_DestroyWindow(mWindow);
    SDL_DestroyRenderer(mRenderer);
    SDL_Quit();
}

void Game::AddActor(Actor* actor) {
    // if updating actors, need to add to pending
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
    // create player's ship
    mShip = new Ship(this);
    mShip->SetPosition(Vector2(100.0f, 384.0f));
    mShip->SetScale(1.5f);

    // Exercise 2.2
 /*   mDummy = new Dummy(this);
    mDummy->SetPosition(Vector2(100.0f, 384.0f));
    mDummy->SetScale(1.5f);*/

    // create actor for the background (this doesn't need a subclass)
    Actor* temp = new Actor(this);
    temp->SetPosition(Vector2(512.0f, 384.0f));

    // create the "far back" background
    BGSpriteComponent* bg = new BGSpriteComponent(temp);
    bg->SetScreenSize(Vector2(1024.0f, 768.0f));
    std::vector<SDL_Texture*> bgtexs = {
        GetTexture("Assets/Farback01.png"),
        GetTexture("Assets/Farback02.png")
    };
    bg->SetBGTextures(bgtexs);
    bg->SetScrollSpeed(-100.0f);

    // create the closer background
    bg = new BGSpriteComponent(temp, 50);
    bg->SetScreenSize(Vector2(1024.0f, 768.0f));
    bgtexs = {
        GetTexture("Assets/Stars.png"),
        GetTexture("Assets/Stars.png")
    };
    bg->SetBGTextures(bgtexs);
    bg->SetScrollSpeed(-200.0f);

    // Exercise 2.3
    //Actor* temp = new Actor(this);
    //temp->SetPosition(Vector2(512.0f, 384.0f));

    //// create Layer 3
    //TileMapComponent* layer3 = new TileMapComponent(temp);
    //layer3->SetScreenSize(Vector2(1024.0f, 768.0f));
    //layer3->SetTexture(GetTexture("Assets/Tiles.png"));
    //layer3->LoadTileMapCSV("Assets/MapLayer3.csv");

    //// create Layer 2
    //TileMapComponent* layer2 = new TileMapComponent(temp, 20);
    //layer2->SetScreenSize(Vector2(1024.0f, 768.0f));
    //layer2->SetTexture(GetTexture("Assets/Tiles.png"));
    //layer2->LoadTileMapCSV("Assets/MapLayer2.csv");

    //// create Layer 1
    //TileMapComponent* layer1 = new TileMapComponent(temp, 30);
    //layer1->SetScreenSize(Vector2(1024.0f, 768.0f));
    //layer1->SetTexture(GetTexture("Assets/Tiles.png"));
    //layer1->LoadTileMapCSV("Assets/MapLayer1.csv");
}

void Game::UnloadData() {
    // delete actors
    // because ~Actor calls RemoveActor, have to use a different style loop
    while (!mActors.empty()) {
        delete mActors.back();
    }

    // destroy textures
    for (auto i : mTextureMap) {
        SDL_DestroyTexture(i.second);
    }
    mTextureMap.clear();
}

void Game::AddSprite(SpriteComponent* sprite) {
    // find the insertion point in the sorted vector - the first element with a higher draw order than me
    int myDrawOrder = sprite->GetDrawOrder();
    auto iter = mSprites.begin();
    for (; iter != mSprites.end(); ++iter) {
        if (myDrawOrder < (*iter)->GetDrawOrder()) {
            break;
        }
    }

    // inserts element before position of iterator
    mSprites.insert(iter, sprite);
}

void Game::RemoveSprite(SpriteComponent* sprite) {
    // we can't swap because it ruins ordering
    auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
    if (iter != mSprites.end()) {
        mSprites.erase(iter);
    }
}

SDL_Texture* Game::GetTexture(const char* fileName) {
    // is the texture already in the map?
    if (mTextureMap.find(fileName) == mTextureMap.end()) {
        // load from file
        SDL_Surface* surf = IMG_Load(fileName);
        if (!surf) {
            SDL_Log("Failed to load texture file %s", fileName);
            return nullptr;
        }

        // create texture from surface
        SDL_Texture* text = SDL_CreateTextureFromSurface(mRenderer, surf);
        SDL_FreeSurface(surf);
        if (!text) {
            SDL_Log("Failed to convert surface to texture for %s", fileName);
            return nullptr;
        }

        mTextureMap.emplace(fileName, text);
        return text;
    }

    return mTextureMap.find(fileName)->second;
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

    // process ship input
    mShip->ProcessKeyboard(state);
    //mDummy->ProcessKeyboard(state);  // Exercise 2.2
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
    SDL_SetRenderDrawColor(
        mRenderer,
        0,
        0,
        0,
        255
    );

    SDL_RenderClear(mRenderer);

    // draw all sprite components
    for (int i = 0; i < mSprites.size(); ++i) {
        mSprites[i]->Draw(mRenderer);
    }

    SDL_RenderPresent(mRenderer);
}