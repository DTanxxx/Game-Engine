#pragma once

#include "Component.hpp"
#include <string>

class MeshComponent : public Component {
public:
	MeshComponent(class Actor* owner, class Mesh* mesh);
	~MeshComponent();

	// draw this mesh component
	virtual void Draw(class Shader* shader);

	void SetTextureIndex(size_t index) {
		mTextureIndex = index;
	}

protected:
	class Mesh* mMesh;

	// texture index determines which texture to use when drawing this component
	// (there can be multiple textures associated with a mesh)
	size_t mTextureIndex;
};
