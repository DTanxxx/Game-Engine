#include "SpriteComponent.hpp"
#include "Game.hpp"
#include "Actor.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

SpriteComponent::SpriteComponent(Actor* owner, int drawOrder) : Component(owner) {
	mDrawOrder = drawOrder;
	mTexture = nullptr;
	mTexWidth = 0;
	mTexHeight = 0;

	mOwner->GetGame()->AddSprite(this);
}

SpriteComponent::~SpriteComponent() {
	mOwner->GetGame()->RemoveSprite(this);
	Component::~Component();
}

void SpriteComponent::SetTexture(Texture* texture) {
	mTexture = texture;
	
	if (mTexture == nullptr) {
		return;
	}

	// get width/height of texture
	mTexWidth = mTexture->GetWidth();
	mTexHeight = mTexture->GetHeight();
}

void SpriteComponent::Draw(Shader* shader) {
	if (mTexture) {
		// scale the quad by the width/height of texture
		Matrix4 scaleMat = Matrix4::CreateScale(
			static_cast<float>(mTexWidth),
			static_cast<float>(mTexHeight),
			1.0f);
		Matrix4 world = scaleMat * mOwner->GetWorldTransform();  // so that if the actor has scale 2.0f and texture has size 128x128, the resulting world transform has size 256x256

		// since all sprites use the same shader/vertices, the game first sets them active before any sprite draws

		// set world transform
		shader->SetMatrixUniform("uWorldTransform", world);

		// set current active texture
		mTexture->SetActive();

		// draw a quad
		glDrawElements(
			GL_TRIANGLES,  // type of polygon/primitive to draw
			6,  // number of indices in index buffer
			GL_UNSIGNED_INT,  // type of each index
			nullptr  // usually nullptr
		);
	}
}
