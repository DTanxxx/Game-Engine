#pragma once

#include "Component.hpp"
#include "SDL.h"

class SpriteComponent : public Component {
public:
	// lower draw order corresponds with further back
	SpriteComponent(class Actor* owner, int drawOrder = 100);
	~SpriteComponent();

	virtual void Draw(SDL_Renderer* renderer);
	virtual void SetTexture(SDL_Texture* texture);

	int GetDrawOrder() const {
		return mDrawOrder;
	}

	int GetTexHeight() const {
		return mTexHeight;
	}

	int GetTexWidth() const {
		return mTexWidth;
	}

protected:
	// texture to draw
	SDL_Texture* mTexture;
	// draw order used for painter's algorithm
	int mDrawOrder;
	// width/height of texture
	int mTexHeight;
	int mTexWidth;
};