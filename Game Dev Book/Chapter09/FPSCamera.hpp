#pragma once

#include "CameraComponent.hpp"

class FPSCamera : public CameraComponent {
public:
	FPSCamera(class Actor* owner);  // don't need updateOrder argument here, can just use CameraComponent's default arg value
	~FPSCamera();

	void Update(float deltaTime) override;

	float GetPitch() const {
		return mPitch;
	}

	float GetPitchSpeed() const {
		return mPitchSpeed;
	}

	float GetMaxPitch() const {
		return mMaxPitch;
	}

	void SetPitchSpeed(float speed) {
		mPitchSpeed = speed;
	}

	void SetMaxPitch(float pitch) {
		mMaxPitch = pitch;
	}

private:
	// rotation/sec speed of pitch
	float mPitchSpeed;
	// maximum pitch deviation from forward (in radians)
	float mMaxPitch;
	// current (absolute) pitch
	float mPitch;
};