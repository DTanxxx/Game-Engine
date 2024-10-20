#pragma once

#include "Actor.hpp"

class CameraActor : public Actor {
public:
	CameraActor(class Game* game);
	~CameraActor();

	void UpdateActor(float deltaTime);
	void ActorInput(const uint8_t* keyState);

private:
	class MoveComponent* mMoveComp;
};
