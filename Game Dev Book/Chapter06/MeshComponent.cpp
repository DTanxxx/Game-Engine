#include "MeshComponent.hpp"
#include "Actor.hpp"
#include "Renderer.hpp"
#include "Game.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include "Mesh.hpp"

MeshComponent::MeshComponent(Actor* owner, Mesh* mesh) : Component(owner) {
	// set the mesh/texture index used by mesh component
	mMesh = mesh;
	mTextureIndex = 0;
	mOwner->GetGame()->GetRenderer()->AddMeshComp(this);
}

MeshComponent::~MeshComponent() {
	mOwner->GetGame()->GetRenderer()->RemoveMeshComp(this);
}

void MeshComponent::Draw(Shader* shader) {
	if (mMesh) {
		// set the world transform matrix uniform
		shader->SetMatrixUniform("uWorldTransform", mOwner->GetWorldTransform());

		if (GetMeshShaderName().compare("Phong") == 0) {
			// set specular power
			shader->SetFloatUniform("uSpecPower", mMesh->GetSpecPower());
		}

		// set the active texture
		Texture* t = mMesh->GetTexture(mTextureIndex);
		if (t) {
			t->SetActive();
		}
		// set the mesh's vertex array as active
		VertexArray* va = mMesh->GetVertexArray();
		va->SetActive();

		// draw the triangles
		glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
	}
}

std::string MeshComponent::GetMeshShaderName() const {
	return mMesh->GetShaderName();
}
