#pragma once

#include <string>
#include <unordered_map>
#include <SDL/SDL.h>
#include "Math.hpp"

struct DirectionalLight {
	// direction of light
	Vector3 mDirection;
	// diffuse color
	Vector3 mDiffuseColor;
	// specular color
	Vector3 mSpecColor;
};

struct PointLight {
	Vector3 mPosition;
	Vector3 mDiffuseColor;
	Vector3 mSpecColor;
	float mSpecPower;
	float mRadius;
};

class Renderer {
public:
	Renderer(class Game* game);
	~Renderer();

	// initialize and shutdown renderer
	bool Initialize(float screenWidth, float screenHeight);
	void Shutdown();

	// unload all textures/meshes
	void UnloadData();
	
	// draw the frame
	void Draw();

	void AddSprite(class SpriteComponent* sprite);
	void RemoveSprite(class SpriteComponent* sprite);

	void AddMeshComp(class MeshComponent* mesh);
	void RemoveMeshComp(class MeshComponent* mesh);

	class Texture* GetTexture(const std::string& fileName);
	class Mesh* GetMesh(const std::string& fileName);

	void SetViewMatrix(const Matrix4& view) {
		mView = view;
	}

	void SetAmbientLight(const Vector3& ambient) {
		mAmbientLight = ambient;
	}

	void AddPointLight(const PointLight& point) {
		mPointLights.emplace_back(point);
	}

	DirectionalLight& GetDirectionalLight() {
		return mDirLight;
	}

	float GetScreenWidth() const {
		return mScreenWidth;
	}

	float GetScreenHeight() const {
		return mScreenHeight;
	}

private:
	bool LoadShaders();
	void CreateSpriteVerts();
	void SetLightUniforms(class Shader* shader);

private:
	// map of textures loaded
	std::unordered_map<std::string, class Texture*> mTextures;
	// map of meshes loaded
	std::unordered_map<std::string, class Mesh*> mMeshes;

	// all of sprite components drawn
	std::vector<class SpriteComponent*> mSprites;
	// all of mesh components drawn
	std::unordered_map<std::string, std::vector<class MeshComponent*>> mMeshCompsMap;

	// game
	class Game* mGame;

	// sprite shader
	class Shader* mSpriteShader;
	// mesh shaders
	std::unordered_map<std::string, class Shader*> mMeshShaderMap;

	// sprite vertex array
	class VertexArray* mSpriteVerts;

	// view/projection for 3D shaders
	Matrix4 mView;
	Matrix4 mProjection;

	// width/height of screen
	float mScreenWidth;
	float mScreenHeight;

	// lighting data
	Vector3 mAmbientLight;
	DirectionalLight mDirLight;
	std::vector<PointLight> mPointLights;
	
	// window
	SDL_Window* mWindow;
	
	// OpenGL context
	SDL_GLContext mContext;
};