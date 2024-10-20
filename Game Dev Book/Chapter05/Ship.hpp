#pragma once

#include "Actor.hpp"

class Ship : public Actor {
public:
	Ship(class Game* game);

	void ActorInput(const uint8_t* keyState) override;
	void UpdateActor(float deltaTime) override;

	void OnDeath();
	void OnRevive();

private:
	float mLaserCooldown;
	float mResetTimer;
	bool mDead;

	class InputComponent* mInputComp;
	class SpriteComponent* mSpriteComp;
	class CircleComponent* mCircleComp;
};