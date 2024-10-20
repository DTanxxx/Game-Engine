#pragma once

#include "SpriteComponent.hpp"
#include <vector>

class AnimSpriteComponent : public SpriteComponent {
public:
	AnimSpriteComponent(class Actor* owner, int drawOrder = 100);

	// update animation every frame (overridden from component)
	void Update(float deltaTime) override;

	// set the textures used for animation
	void SetAnimTextures(const std::vector<SDL_Texture*>& textures);

	// this must be called BEFORE SetAnimTextures()
	void AppendAnimation(int startIndex, int endIndex, bool isLooping);

	// set/get the animation FPS
	void SetAnimation(int animationIndex);

	float GetAnimFPS() const {
		return mAnimFPS;
	}

	void SetAnimFPS(float fps) {
		mAnimFPS = fps;
	}

private:
	struct Animation {
		int mStartIndex;
		int mEndIndex;
		bool mLooping;
	};

	// all textures in the animation
	std::vector<Animation> mAnimations;
	std::vector<SDL_Texture*> mAnimTextures;
	int mCurrAnimation;

	// current frame displayed
	float mCurrFrame;

	// animation frame rate
	float mAnimFPS;
};