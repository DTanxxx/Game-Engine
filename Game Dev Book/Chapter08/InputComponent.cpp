#include "InputComponent.hpp"
#include "Actor.hpp"
#include "InputSystem.hpp"

InputComponent::InputComponent(Actor* owner) : MoveComponent(owner) {
	mForwardKey = 0;
	mBackKey = 0;
	mClockwiseKey = 0;
	mCounterClockwiseKey = 0;
}

void InputComponent::ProcessInput(const InputState& state) {
	// calculate forward speed for MoveComponent
	float forwardSpeed = 0.0f;
	if (state.Keyboard.GetKeyValue(SDL_Scancode(mForwardKey))) {
		// forward key pressed
		forwardSpeed += mMaxForwardSpeed;
	}
	if (state.Keyboard.GetKeyValue(SDL_Scancode(mBackKey))) {
		// backward key pressed
		forwardSpeed -= mMaxForwardSpeed;
	}
	SetForwardSpeed(forwardSpeed);

	// calculate angular speed for MoveComponent
	float angularSpeed = 0.0f;
	if (state.Keyboard.GetKeyValue(SDL_Scancode(mClockwiseKey))) {
		angularSpeed += mMaxAngularSpeed;
	}
	if (state.Keyboard.GetKeyValue(SDL_Scancode(mCounterClockwiseKey))) {
		angularSpeed -= mMaxAngularSpeed;
	}
	SetAngularSpeed(angularSpeed);
}