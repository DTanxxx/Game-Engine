#pragma once

#include "Actor.hpp"
#include "SoundEvent.hpp"

class CameraActor : public Actor {
public:
	CameraActor(class Game* game);
	~CameraActor();

	void UpdateActor(float deltaTime) override;
	void ActorInput(const uint8_t* keyState) override;

	void SetFootstepSurface(float value);

	Vector3 GetCameraPosition() const {
		return mCameraPos;
	}

private:
	class MoveComponent* mMoveComp;
	class AudioComponent* mAudioComp;
	SoundEvent mFootstep;
	float mLastFootstep;
	Vector3 mCameraPos;
};
