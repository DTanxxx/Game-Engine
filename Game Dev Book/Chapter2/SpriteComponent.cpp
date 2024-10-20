#include "SpriteComponent.hpp"
#include "Game.hpp"
#include "Actor.hpp"

SpriteComponent::SpriteComponent(Actor* owner, int drawOrder) : Component(owner) {
	mDrawOrder = drawOrder;
	mTexture = nullptr;
	mTexWidth = 0;
	mTexHeight = 0;

	mOwner->GetGame()->AddSprite(this);
}

SpriteComponent::~SpriteComponent() {
	mOwner->GetGame()->RemoveSprite(this);
}

void SpriteComponent::SetTexture(SDL_Texture* texture) {
	mTexture = texture;
	
	// get width/height of texture
	SDL_QueryTexture(texture, nullptr, nullptr, &mTexWidth, &mTexHeight);
}

void SpriteComponent::Draw(SDL_Renderer* renderer) {
	if (mTexture) {
		SDL_Rect r;
		// scale the width/height by owner's scale
		r.w = static_cast<int>(mTexWidth * mOwner->GetScale());
		r.h = static_cast<int>(mTexHeight * mOwner->GetScale());
		// center the rectangle around the position of the owner
		r.x = static_cast<int>(mOwner->GetPosition().x - r.w / 2);
		r.y = static_cast<int>(mOwner->GetPosition().y - r.h / 2);

		// draw (have to convert angle from radians to degrees, and counterclockwise to clockwise)
		SDL_RenderCopyEx(renderer,  // render target to draw to
			mTexture,  // texture to draw
			nullptr,  // part of texture to draw (null if whole)
			&r,  // rectangle to draw onto the target
			-Math::ToDegrees(mOwner->GetRotation()),  // rotation angle (in degrees, clockwise)
			nullptr,  // point to rotate about (nullptr for center)
			SDL_FLIP_NONE);  // how to flip texture (usually SDL_FLIP_NONE)
	}
}