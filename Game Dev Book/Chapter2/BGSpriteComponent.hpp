#pragma once

#include "SpriteComponent.hpp"
#include "Math.hpp"
#include <vector>

class BGSpriteComponent : public SpriteComponent {
public:
	// set draw order to default to lower (so it's in the background)
	BGSpriteComponent(class Actor* owner, int drawOrder = 10);

	// update/draw overridden from parent
	void Update(float deltaTime) override;
	void Draw(SDL_Renderer* renderer) override;

	// set the textures used for the background
	void SetBGTextures(const std::vector<SDL_Texture*>& textures);

	// get/set screen size and scroll speed
	void SetScreenSize(const Vector2& size) {
		mScreenSize = size;
	}

	void SetScrollSpeed(float speed) {
		mScrollSpeed = speed;
	}

	float GetScrollSpeed() const {
		return mScrollSpeed;
	}

private:
	// struct to encapsulate each bg image and its offset
	struct BGTexture {
		SDL_Texture* mTexture;
		Vector2 mOffset;
	};

	std::vector<BGTexture> mBGTextures;
	Vector2 mScreenSize;
	float mScrollSpeed;
};