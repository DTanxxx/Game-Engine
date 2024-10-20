#include "Game.hpp"
#include "Actor.hpp"
#include "SpriteComponent.hpp"
#include "VertexArray.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Asteroid.hpp"
#include "Ship.hpp"
#include <algorithm>

Game::Game() {
    mWindow = nullptr;
    mIsRunning = true;
    mUpdatingActors = false;
    mTicksCount = 0;

    Vector3* red = new Vector3(1.0f, 0.0f, 0.0f);
    Vector3* blue = new Vector3(0.0f, 0.0f, 1.0f);
    Vector3* black = new Vector3(0.0f, 0.0f, 0.0f);
    currBGIndex = 0;
    allColours.emplace_back(black);
    allColours.emplace_back(blue);
    allColours.emplace_back(red);
    colourTransitionDur = 3.0f;
    curTransitionTimer = colourTransitionDur;
}

bool Game::Initialize() {
    int sdlResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    if (sdlResult != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // request OpenGL attributes and configure them
    // use the core OpenGL profile for desktop environment
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    // specify version 3.3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    // request a color buffer with 8-bits (0-255) per RGBA channel, for a total of 32 bits per pixel
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    // enable double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // force OpenGL to use hardware acceleration
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    mWindow = SDL_CreateWindow(
        "Game Programming in C++ (Chapter 5)",
        100,
        100,
        1024,
        768,
        SDL_WINDOW_OPENGL  // request a window for OpenGL usage
    );

    if (!mWindow) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    // create an OpenGL context - a context is the "world" of OpenGL that contains every item that OpenGL knows about
    // such as the color buffer, any images or models loaded, etc.
    mContext = SDL_GL_CreateContext(mWindow);

    // initialize GLEW
    glewExperimental = GL_TRUE;  // prevents an initialization error that may occur
    if (glewInit() != GLEW_OK) {
        SDL_Log("Failed to initialize GLEW.");
        return false;
    }
    // on some platforms, GLEW will emit a benign error code, so clear it
    glGetError();

    /*int sdlImageResult = IMG_Init(IMG_INIT_PNG);
    if (sdlImageResult == 0) {
        SDL_Log("Unable to initialize SDL Image: %s", SDL_GetError());
        return false;
    }*/

    // make sure we can create/compile shaders
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
    delete mSpriteVerts;
    mSpriteShader->Unload();
    delete mSpriteShader;
    //IMG_Quit();
    SDL_GL_DeleteContext(mContext);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void Game::CreateSpriteVerts() {
    // x, y, z positions as well as texture coords u, v, for texture mapping
    // Exercise 5.2 - add RGB vertex color attribute
    float vertices[] = {
        -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // top left
         0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f  // bottom left
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    InitSpriteVerts(vertices, indices);
}

void Game::InitSpriteVerts(const float* vertexBuffer, const unsigned int* indexBuffer) {
    // 4 vertices in vertexBuffer, 6 indices in indexBuffer (corresponding to the 2 triangles in the quad)
    mSpriteVerts = new VertexArray(vertexBuffer, 4, indexBuffer, 6);
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
    // Create player's ship
    mShip = new Ship(this);
    mShip->SetRotation(Math::PiOver2);

    // Create asteroids
    const int numAsteroids = 20;
    for (int i = 0; i < numAsteroids; i++)
    {
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
    for (auto i : mTextureMap) {
        //SDL_DestroyTexture(i.second);
        i.second->Unload();
        delete i.second;
    }
    mTextureMap.clear();
}

bool Game::LoadShaders() {
    mSpriteShader = new Shader();
    if (!mSpriteShader->Load("Shaders/Sprite.vert", "Shaders/Sprite.frag")) {
        return false;
    }

    // set this shader as active
    mSpriteShader->SetActive();

    // create and set the view-projection matrix to the simple view projection, assuming a 1024x768 screen resolution
    Matrix4 viewProj = Matrix4::CreateSimpleViewProj(1024.0f, 768.0f);
    mSpriteShader->SetMatrixUniform("uViewProj", viewProj);

    return true;
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

Texture* Game::GetTexture(const std::string& fileName) {
    if (mTextureMap.find(fileName) == mTextureMap.end()) {
        // load from file
        Texture* newTex = new Texture();
        if (newTex->Load(fileName)) {
            mTextureMap.emplace(fileName, newTex);
        }
        else {
            delete newTex;
            newTex = nullptr;
        }
        return newTex;
    }

    return mTextureMap.find(fileName)->second;
}

//SDL_Texture* Game::GetTexture(const std::string& fileName) {
//    // is the texture already in the map?
//    if (mTextureMap.find(fileName) == mTextureMap.end()) {
//        // load from file
//        SDL_Surface* surf = IMG_Load(fileName.c_str());
//        if (!surf) {
//            SDL_Log("Failed to load texture file %s", fileName);
//            return nullptr;
//        }
//
//        // create texture from surface
//        SDL_Texture* text = SDL_CreateTextureFromSurface(mRenderer, surf);
//        SDL_FreeSurface(surf);
//        if (!text) {
//            SDL_Log("Failed to convert surface to texture for %s", fileName);
//            return nullptr;
//        }
//
//        mTextureMap.emplace(fileName, text);
//        return text;
//    }
//
//    return mTextureMap.find(fileName)->second;
//}

void Game::AddAsteroid(Asteroid* ast)
{
    mAsteroids.emplace_back(ast);
}

void Game::RemoveAsteroid(Asteroid* ast)
{
    auto iter = std::find(mAsteroids.begin(), mAsteroids.end(), ast);
    if (iter != mAsteroids.end())
    {
        mAsteroids.erase(iter);
    }
}

std::vector<Asteroid*> Game::GetAsteroids() {
    return mAsteroids;
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

    // Exercise 5.1
    curTransitionTimer -= deltaTime;
    if (curTransitionTimer <= 0.0f) {
        curTransitionTimer = colourTransitionDur;
        // change colour
        currBGIndex = (currBGIndex + 1) % allColours.size();
    }
}

void Game::GenerateOutput() {
    // set the clear color to gray
    //glClearColor(0.86f, 0.86f, 0.86f, 1.0f);
    
    // Exercise 5.1
    Vector3* newColor = new Vector3();
    int nextIndex = (currBGIndex + 1) % allColours.size();

    newColor->x = allColours[currBGIndex]->x + (1 - curTransitionTimer / colourTransitionDur) * (allColours[nextIndex]->x - allColours[currBGIndex]->x);
    newColor->y = allColours[currBGIndex]->y + (1 - curTransitionTimer / colourTransitionDur) * (allColours[nextIndex]->y - allColours[currBGIndex]->y);
    newColor->z = allColours[currBGIndex]->z + (1 - curTransitionTimer / colourTransitionDur) * (allColours[nextIndex]->z - allColours[currBGIndex]->z);
    glClearColor(newColor->x, newColor->y, newColor->z, 1.0f);

    // clear the color buffer to the specified color
    glClear(GL_COLOR_BUFFER_BIT);

    // set sprite shader and vertex array objects active
    mSpriteShader->SetActive();
    mSpriteVerts->SetActive();

    // alpha blending to support transparency
    glEnable(GL_BLEND);  // turn on color buffer blending
    glBlendFunc(
        GL_SRC_ALPHA,  // sourceAlpha is srcAlpha
        GL_ONE_MINUS_SRC_ALPHA  // destinationFactor is 1 - sourceAlpha
    );

    // draw all sprites
    for (auto sprite : mSprites) {
        sprite->Draw(mSpriteShader);
    }

    // swap the front and back buffers, which also displays the scene
    SDL_GL_SwapWindow(mWindow);
}
