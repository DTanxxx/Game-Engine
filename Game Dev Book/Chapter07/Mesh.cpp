#include "Mesh.hpp"
#include "Math.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"
#include "Renderer.hpp"
#include <fstream>
#include <sstream>
#include <rapidjson/document.h>

Mesh::Mesh() {
	mVertexArray = nullptr;
	mRadius = 0.0f;
}

Mesh::~Mesh() {
}

bool Mesh::Load(const std::string& fileName, class Renderer* rend) {
	std::ifstream file(fileName);
	if (!file.is_open()) {
		SDL_Log("File not found: Mesh %s", fileName.c_str());
		return false;
	}

	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);

	if (!doc.IsObject()) {
		SDL_Log("Mesh %s is not valid json", fileName.c_str());
		return false;
	}

	int version = doc["version"].GetInt();

	// check the version
	if (version != 1) {
		SDL_Log("Mesh %s not version 1", fileName.c_str());
		return false;
	}

	mShaderName = doc["shader"].GetString();

	// skip vertex format for now
	size_t vertSize = 8;

	// load textures
	const rapidjson::Value& textures = doc["textures"];
	if (!textures.IsArray() || textures.Size() < 1) {
		SDL_Log("Mesh %s has no textures, there should be at least one", fileName.c_str());
		return false;
	}

	mSpecPower = static_cast<float>(doc["specularPower"].GetDouble());

	for (rapidjson::SizeType i = 0; i < textures.Size(); ++i) {
		// is this texture already loaded?
		std::string texName = textures[i].GetString();
		Texture* t = rend->GetTexture(texName);
		if (t == nullptr) {
			// use the default texture
			t = rend->GetTexture("Assets/Default.png");
		}
		mTextures.emplace_back(t);
	}

	// load in the vertices
	const rapidjson::Value& vertsJson = doc["vertices"];
	if (!vertsJson.IsArray() || vertsJson.Size() < 1) {
		SDL_Log("Mesh %s has no vertices", fileName.c_str());
		return false;
	}

	std::vector<float> vertices;
	vertices.reserve(vertsJson.Size() * vertSize);
	mRadius = 0.0f;
	for (rapidjson::SizeType i = 0; i < vertsJson.Size(); ++i) {
		// for now, assume we have 8 elements per vertex
		const rapidjson::Value& vert = vertsJson[i];
		if (!vert.IsArray() || vert.Size() != 8) {
			SDL_Log("Unexpected vertex format for %s", fileName.c_str());
			return false;
		}

		Vector3 pos(vert[0].GetDouble(), vert[1].GetDouble(), vert[2].GetDouble());
		mRadius = Math::Max(mRadius, pos.LengthSq());

		// add the floats
		for (rapidjson::SizeType i = 0; i < vert.Size(); ++i) {
			vertices.emplace_back(static_cast<float>(vert[i].GetDouble()));
		}
	}

	// convert mRadius to actual length
	mRadius = Math::Sqrt(mRadius);

	// load in the indices
	const rapidjson::Value& indJson = doc["indices"];
	if (!indJson.IsArray() || indJson.Size() < 1) {
		SDL_Log("Mesh %s has no indices", fileName.c_str());
		return false;
	}

	std::vector<unsigned int> indices;
	indices.reserve(indJson.Size() * 3);
	for (rapidjson::SizeType i = 0; i < indJson.Size(); ++i) {
		const rapidjson::Value& ind = indJson[i];
		if (!ind.IsArray() || ind.Size() != 3) {
			SDL_Log("Invalid indices for %s", fileName.c_str());
			return false;
		}

		indices.emplace_back(ind[0].GetUint());
		indices.emplace_back(ind[1].GetUint());
		indices.emplace_back(ind[2].GetUint());
	}

	// finally, create a vertex array
	mVertexArray = new VertexArray(vertices.data(), static_cast<unsigned>(vertices.size()) / vertSize,
		indices.data(), static_cast<unsigned>(indices.size()));
	return true;
} 

void Mesh::Unload() {
	delete mVertexArray;
	mVertexArray = nullptr;
}

Texture* Mesh::GetTexture(size_t index) {
	if (index < mTextures.size()) {
		return mTextures[index];
	}
	else {
		return nullptr;
	}
}
