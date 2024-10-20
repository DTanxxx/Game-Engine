#pragma once

#include "Actor.hpp"

class Ship : public Actor {
public:
	Ship(class Game* game, int controllerIndex);

	void ActorInput(const struct InputState& state) override;
	void UpdateActor(float deltaTime) override;

	int GetControllerIndex() const {
		return mControllerIndex;
	}

private:
	Vector2 mVelocityDir;
	Vector2 mRotationDir;
	float mSpeed;
	float mLaserCooldown;
	int mControllerIndex;
};