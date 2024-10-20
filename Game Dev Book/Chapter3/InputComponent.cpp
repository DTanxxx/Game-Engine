#include "InputComponent.hpp"
#include "Actor.hpp"

InputComponent::InputComponent(Actor* owner) : MoveComponent(owner) {
	mForwardKey = 0;
	mBackKey = 0;
	mClockwiseKey = 0;
	mCounterClockwiseKey = 0;
}

void InputComponent::ProcessInput(const uint8_t* keyState) {
	/*
	// calculate forward speed for MoveComponent
	float forwardSpeed = 0.0f;
	if (keyState[mForwardKey]) {
		// forward key pressed
		forwardSpeed += mMaxForwardSpeed;
	}
	if (keyState[mBackKey]) {
		// backward key pressed
		forwardSpeed -= mMaxForwardSpeed;
	}
	SetForwardSpeed(forwardSpeed);

	// calculate angular speed for MoveComponent
	float angularSpeed = 0.0f;
	if (keyState[mClockwiseKey]) {
		angularSpeed -= mMaxAngularSpeed;
	}
	if (keyState[mCounterClockwiseKey]) {
		angularSpeed += mMaxAngularSpeed;
	}
	SetAngularSpeed(angularSpeed);
	*/

	// Exercise 3.3
	Vector2 resultantForce = Vector2::Zero;
	if (keyState[mForwardKey]) {
		resultantForce.y -= mMaxForce;
	}
	if (keyState[mBackKey]) {
		resultantForce.y += mMaxForce;
	}
	if (keyState[mClockwiseKey]) {
		resultantForce.x += mMaxForce;
	}
	if (keyState[mCounterClockwiseKey]) {
		resultantForce.x -= mMaxForce;
	}

	AddForce(resultantForce);
}