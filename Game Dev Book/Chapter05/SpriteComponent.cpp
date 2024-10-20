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
	//SDL_QueryTexture(texture, nullptr, nullptr, &mTexWidth, &mTexHeight);
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

//void SpriteComponent::Draw(SDL_Renderer* renderer) {
//	if (mTexture) {
//		SDL_Rect r;
//		// scale the width/height by owner's scale
//		r.w = static_cast<int>(mTexWidth * mOwner->GetScale());
//		r.h = static_cast<int>(mTexHeight * mOwner->GetScale());
//		// center the rectangle around the position of the owner
//		r.x = static_cast<int>(mOwner->GetPosition().x - r.w / 2);
//		r.y = static_cast<int>(mOwner->GetPosition().y - r.h / 2);
//
//		// draw (have to convert angle from radians to degrees, and counterclockwise to clockwise)
//		SDL_RenderCopyEx(renderer,  // render target to draw to
//			mTexture,  // texture to draw
//			nullptr,  // part of texture to draw (null if whole)
//			&r,  // rectangle to draw onto the target
//			-Math::ToDegrees(mOwner->GetRotation()),  // rotation angle (in degrees, clockwise)
//			nullptr,  // point to rotate about (nullptr for center)
//			SDL_FLIP_NONE);  // how to flip texture (usually SDL_FLIP_NONE)
//	}
//}