#pragma once

#include "MoveComponent.hpp"
#include <cstdint>

// for binding specific keys to control both forward/backward movement and rotation of owning actor
class InputComponent : public MoveComponent {
public:
	// lower update order to update first
	InputComponent(class Actor* owner);

	void ProcessInput(const struct InputState& state) override;

	// getters/setters
	float GetMaxForwardSpeed() const {
		return mMaxForwardSpeed;
	}

	void SetMaxForwardSpeed(float maxForwardSpeed) {
		mMaxForwardSpeed = maxForwardSpeed;
	}

	float GetMaxAngularSpeed() const {
		return mMaxAngularSpeed;
	}

	void SetMaxAngularSpeed(float maxAngularSpeed) {
		mMaxAngularSpeed = maxAngularSpeed;
	}

	int GetForwardKey() const {
		return mForwardKey;
	}

	void SetForwardKey(int forward) {
		mForwardKey = forward;
	}

	int GetBackwardKey() const {
		return mBackKey;
	}

	void SetBackwardKey(int back) {
		mBackKey = back;
	}

	int GetClockwiseKey() const {
		return mClockwiseKey;
	}

	void SetClockwiseKey(int clockwise) {
		mClockwiseKey = clockwise;
	}

	int GetCounterClockwiseKey() const {
		return mCounterClockwiseKey;
	}

	void SetCounterClockwiseKey(int counter) {
		mCounterClockwiseKey = counter;
	}

private:
	// the maximum forward/angular speeds
	float mMaxForwardSpeed;
	float mMaxAngularSpeed;

	// keys for forward/back movement
	int mForwardKey;
	int mBackKey;

	// keys for angular movement
	int mClockwiseKey;
	int mCounterClockwiseKey;
};