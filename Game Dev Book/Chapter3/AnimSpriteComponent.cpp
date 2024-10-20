#include "AnimSpriteComponent.hpp"
#include "Math.h"

AnimSpriteComponent::AnimSpriteComponent(Actor* owner, int drawOrder) : SpriteComponent(owner, drawOrder) {
	mCurrFrame = 0.0f;
	mAnimFPS = 24.0f;
	mCurrAnimation = 0;
}

void AnimSpriteComponent::SetAnimTextures(const std::vector<SDL_Texture*>& textures) {
	mAnimTextures = textures;
	if (mAnimTextures.size() > 0) {
		// set the active texture to first frame
		mCurrFrame = mAnimations[mCurrAnimation].mStartIndex;
		SetTexture(mAnimTextures[static_cast<int>(mCurrFrame)]);
	}
}

void AnimSpriteComponent::AppendAnimation(int startIndex, int endIndex, bool isLooping) {
	Animation animation{
		startIndex,
		endIndex,
		isLooping
	};
	mAnimations.emplace_back(animation);
}

void AnimSpriteComponent::SetAnimation(int animationIndex) {
	mCurrAnimation = animationIndex;
	mCurrFrame = mAnimations[mCurrAnimation].mStartIndex;
}

void AnimSpriteComponent::Update(float deltaTime) {
	SpriteComponent::Update(deltaTime);

	if (mAnimTextures.size() > 0) {
		// update the current frame based on frame rate and delta time
		mCurrFrame += mAnimFPS * deltaTime;

		Animation currAnim = mAnimations[mCurrAnimation];

		// wrap current frame if needed
		if (static_cast<int>(mCurrFrame) > currAnim.mEndIndex) {
			if (currAnim.mLooping) {
				mCurrFrame = currAnim.mStartIndex;
			}
			else {
				mCurrFrame = currAnim.mEndIndex;
			}
		}

		// set the current texture
		SetTexture(mAnimTextures[static_cast<int>(mCurrFrame)]);
	}
}