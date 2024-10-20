#include "Renderer.hpp"
#include <GL/glew.h>
#include "Shader.hpp"
#include "Texture.hpp"
#include "Mesh.hpp"
#include "VertexArray.hpp"
#include "MeshComponent.hpp"
#include "SpriteComponent.hpp"

Renderer::Renderer(Game* game) {
	mGame = game;
	mSpriteShader = nullptr;
}

Renderer::~Renderer() {
}

bool Renderer::Initialize(float screenWidth, float screenHeight) {
	mScreenWidth = screenWidth;
	mScreenHeight = screenHeight;

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
    // request a depth buffer with 24 bits
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    // enable double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // force OpenGL to use hardware acceleration
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    mWindow = SDL_CreateWindow(
        "Game Programming in C++ (Chapter 7)",
        100,
        100,
        static_cast<int>(mScreenWidth),
        static_cast<int>(mScreenHeight),
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

    // make sure we can create/compile shaders
    if (!LoadShaders()) {
        SDL_Log("Failed to load shaders");
        return false;
    }

    // create quad for drawing sprites
    CreateSpriteVerts();

    return true;
}

void Renderer::Shutdown() {
    delete mSpriteVerts;
    mSpriteShader->Unload();
    delete mSpriteShader;

    for (auto i : mMeshShaderMap) {
        i.second->Unload();
        delete i.second;
    }
    mMeshShaderMap.clear();

    SDL_GL_DeleteContext(mContext);
    SDL_DestroyWindow(mWindow);
}

void Renderer::UnloadData() {
    // destroy textures
    for (auto i : mTextures) {
        i.second->Unload();
        delete i.second;
    }
    mTextures.clear();

    // destroy meshes
    for (auto i : mMeshes) {
        i.second->Unload();
        delete i.second;
    }
    mMeshes.clear();

    mPointLights.clear();
}

void Renderer::Draw() {
    // set the clear color to light gray
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // clear the color buffer to the specified color and clear the depth buffer
    // to the maximum depth value in normalized device coords (1.0)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // PHASE 1: RENDER 3D OBJECTS
    // draw mesh components
    // enable depth buffering & disable alpha blend
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    for (auto i : mMeshCompsMap) {
        std::string shaderName = i.first;
        std::vector<MeshComponent*> meshComps = i.second;

        // set shaderName shader active
        Shader* shader = mMeshShaderMap.find(shaderName)->second;
        shader->SetActive();

        // recalculate view-projection matrix every frame to account for a moving camera
        shader->SetMatrixUniform("uViewProj", mView * mProjection);

        if (shaderName.compare("Phong") == 0) {
            // update lighting uniforms
            SetLightUniforms(shader);
        }

        for (auto mc : meshComps) {
            mc->Draw(shader);
        }
    }

    // PHASE 2: RENDER 2D SPRITES
    // draw sprite components
    // disable depth buffering
    glDisable(GL_DEPTH_TEST);
    // enable alpha blending to support transparency
    glEnable(GL_BLEND);  // turn on color buffer blending
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendFuncSeparate(
        GL_SRC_ALPHA,  // sourceAlpha is srcAlpha
        GL_ONE_MINUS_SRC_ALPHA,  // destinationFactor is 1 - sourceAlpha
        GL_ONE,
        GL_ZERO
    );

    // set sprite shader and vertex array objects active
    mSpriteShader->SetActive();
    mSpriteVerts->SetActive();

    for (auto sprite : mSprites) {
        sprite->Draw(mSpriteShader);
    }

    // swap the front and back buffers, which also displays the scene
    SDL_GL_SwapWindow(mWindow);
}

void Renderer::AddSprite(SpriteComponent* sprite) {
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

void Renderer::RemoveSprite(SpriteComponent* sprite) {
    // we can't swap because it ruins ordering
    auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
    if (iter != mSprites.end()) {
        mSprites.erase(iter);
    }
}

void Renderer::AddMeshComp(MeshComponent* mesh) {
    if (mMeshCompsMap.find(mesh->GetMeshShaderName()) == mMeshCompsMap.end()) {
        // add a new vector to the map
        std::vector<MeshComponent*> newComps = { mesh };
        mMeshCompsMap.emplace(mesh->GetMeshShaderName(), newComps);
    }
    else {
        auto i = mMeshCompsMap.find(mesh->GetMeshShaderName());
        std::vector<MeshComponent*> comps = i->second;
        comps.emplace_back(mesh);
        i->second = comps;
    }    
}

void Renderer::RemoveMeshComp(MeshComponent* mesh) {
    for (auto i : mMeshCompsMap) {
        std::vector<MeshComponent*> meshComps = i.second;
        auto iter = std::find(meshComps.begin(), meshComps.end(), mesh);
        if (iter != meshComps.end()) {
            meshComps.erase(iter);
        }
    }
}

Texture* Renderer::GetTexture(const std::string& fileName) {
    if (mTextures.find(fileName) == mTextures.end()) {
        // load from file
        Texture* newTex = new Texture();
        if (newTex->Load(fileName)) {
            mTextures.emplace(fileName, newTex);
        }
        else {
            delete newTex;
            newTex = nullptr;
        }
        return newTex;
    }

    return mTextures.find(fileName)->second;
}

Mesh* Renderer::GetMesh(const std::string& fileName) {
    if (mMeshes.find(fileName) == mMeshes.end()) {
        // load from file
        Mesh* newMesh = new Mesh();
        if (newMesh->Load(fileName, this)) {
            mMeshes.emplace(fileName, newMesh);
        }
        else {
            delete newMesh;
            newMesh = nullptr;
        }
        return newMesh;
    }

    return mMeshes.find(fileName)->second;
}

bool Renderer::LoadShaders() {
    // create sprite shader
    mSpriteShader = new Shader();
    if (!mSpriteShader->Load("Shaders/Sprite.vert", "Shaders/Sprite.frag")) {
        return false;
    }

    // set this shader as active
    mSpriteShader->SetActive();

    // create and set the view-projection matrix to the simple view projection
    Matrix4 viewProj = Matrix4::CreateSimpleViewProj(mScreenWidth, mScreenHeight);
    mSpriteShader->SetMatrixUniform("uViewProj", viewProj);

    // create mesh shaders
    Shader* basicShader = new Shader();
    if (!basicShader->Load("Shaders/BasicMesh.vert", "Shaders/BasicMesh.frag")) {
        return false;
    }
    basicShader->SetActive();
    mMeshShaderMap.emplace("BasicMesh", basicShader);

    Shader* phongShader = new Shader();
    if (!phongShader->Load("Shaders/Phong.vert", "Shaders/Phong.frag")) {
        return false;
    }
    phongShader->SetActive();
    mMeshShaderMap.emplace("Phong", phongShader);

    // set the view-projection matrix
    // view matrix is a look-at matrix facing down the x-axis
    mView = Matrix4::CreateLookAt(
        Vector3::Zero,  // camera position
        Vector3::UnitX,  // target position
        Vector3::UnitZ  // up direction
    );
    // projection matrix is a perspective matrix
    mProjection = Matrix4::CreatePerspectiveFOV(
        Math::ToRadians(70.0f),  // horizontal FOV
        mScreenWidth,  // width of view
        mScreenHeight,  // height of view 
        25.0f,  // near plane distance
        10000.0f  // far plane distance
    );
    basicShader->SetMatrixUniform("uViewProj", mView * mProjection);
    phongShader->SetMatrixUniform("uViewProj", mView * mProjection);

    return true;
}

void Renderer::CreateSpriteVerts() {
    // x, y, z positions, normals, and texture coords u, v, for texture mapping
    float vertices[] = {
        -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // top left
         0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f  // bottom left
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    // 4 vertices in vertexBuffer, 6 indices in indexBuffer (corresponding to the 2 triangles in the quad)
    mSpriteVerts = new VertexArray(vertices, 4, indices, 6);
}

void Renderer::SetLightUniforms(Shader* shader) {
    // camera position is from inverted view
    Matrix4 invView = mView;
    invView.Invert();
    shader->SetVectorUniform("uCameraPos", invView.GetTranslation());

    // ambient light
    shader->SetVectorUniform("uAmbientLight", mAmbientLight);

    // directional light
    shader->SetVectorUniform("uDirLight.mDirection", mDirLight.mDirection);
    shader->SetVectorUniform("uDirLight.mDiffuseColor", mDirLight.mDiffuseColor);
    shader->SetVectorUniform("uDirLight.mSpecColor", mDirLight.mSpecColor);

    // point lights
    for (int i = 0; i < mPointLights.size(); ++i) {
        std::string posName = "uPointLights[" + std::to_string(i) + "].mPosition";
        std::string difColName = "uPointLights[" + std::to_string(i) + "].mDiffuseColor";
        std::string specColName = "uPointLights[" + std::to_string(i) + "].mSpecColor";
        std::string specPowName = "uPointLights[" + std::to_string(i) + "].mSpecPower";
        std::string radName = "uPointLights[" + std::to_string(i) + "].mRadius";

        shader->SetVectorUniform(posName.c_str(), mPointLights[i].mPosition);
        shader->SetVectorUniform(difColName.c_str(), mPointLights[i].mDiffuseColor);
        shader->SetVectorUniform(specColName.c_str(), mPointLights[i].mSpecColor);
        shader->SetFloatUniform(specPowName.c_str(), mPointLights[i].mSpecPower);
        shader->SetFloatUniform(radName.c_str(), mPointLights[i].mRadius);
    }
}
