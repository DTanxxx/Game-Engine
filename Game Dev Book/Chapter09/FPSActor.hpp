#pragma once

#include "Actor.hpp"

// this class implements first-person movement
class FPSActor : public Actor {
public:
	FPSActor(class Game* game);
	~FPSActor();

	void UpdateActor(float deltaTime) override;
	void ActorInput(const uint8_t* keyState) override;

private:
	class MoveComponent* mMoveComp;
	class FPSCamera* mCameraComp;
	class Actor* mFPSModel;
	class MeshComponent* mMeshComp;
};