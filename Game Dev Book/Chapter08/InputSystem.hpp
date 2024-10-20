#pragma once

#include <SDL/SDL_scancode.h>
#include <SDL/SDL_gamecontroller.h>
#include "Math.hpp"
#include <SDL/SDL_events.h>
#include <vector>

// different button states
enum ButtonState {
	ENone,
	EPressed,
	EReleased,
	EHeld
};

// helper for keyboard input
class KeyboardState {
public:
	// friend so InputSystem can easily update it
	friend class InputSystem;
	// get just the boolean true/false value of key
	bool GetKeyValue(SDL_Scancode keyCode) const;
	// get a state based on current and previous frame
	ButtonState GetKeyState(SDL_Scancode keyCode) const;

private:
	// current state pointer
	const Uint8* mCurrState;
	// previous frame state
	Uint8 mPrevState[SDL_NUM_SCANCODES];
};

// helper for mouse input
class MouseState {
public:
	friend class InputSystem;

	// for mouse positions
	const Vector2& GetPosition() const {
		return mMousePos;
	}

	const Vector2& GetScrollWheel() const {
		return mScrollWheel;
	}

	bool IsRelative() const {
		return mIsRelative;
	}

	// for buttons
	bool GetButtonValue(int button) const;
	ButtonState GetButtonState(int button) const;

private:
	// store current mouse position
	Vector2 mMousePos;
	// store button data
	Uint32 mCurrButtons;
	Uint32 mPrevButtons;
	// motion of scroll wheel
	Vector2 mScrollWheel;

	// are we in relative mouse mode
	bool mIsRelative;
};

// helper for controller input
class ControllerState {
public:
	friend class InputSystem;

	// for buttons
	bool GetButtonValue(SDL_GameControllerButton button) const;
	ButtonState GetButtonState(SDL_GameControllerButton button) const;

	bool GetIsConnected() const {
		return mIsConnected;
	}

	float GetLeftTrigger() const {
		return mLeftTrigger;
	}

	float GetRightTrigger() const {
		return mRightTrigger;
	}

	const Vector2& GetLeftStick() const {
		return mLeftStick;
	}

	const Vector2& GetRightStick() const {
		return mRightStick;
	}

	int GetControllerIndex() const {
		return mIndex;
	}

private:
	// current/previous buttons
	Uint8 mCurrButtons[SDL_CONTROLLER_BUTTON_MAX];
	Uint8 mPrevButtons[SDL_CONTROLLER_BUTTON_MAX];
	// is this controller connected?
	bool mIsConnected;
	
	// left/right triggers
	float mLeftTrigger;
	float mRightTrigger;

	// left/right sticks
	Vector2 mLeftStick;
	Vector2 mRightStick;

	// controller index
	int mIndex;
};

// wrapper that contains current state of input
struct InputState {
	KeyboardState Keyboard;
	MouseState Mouse;
	std::vector<ControllerState> Controllers;
};

class InputSystem {
public:
	InputSystem();
	~InputSystem();

	bool Initialize();
	void Shutdown();

	// called right before SDL_PollEvents loop
	void PrepareForUpdate();
	// called right after SDL_PollEvents loop
	void Update();
	// called to process an SDL event in input system
	void ProcessEvent(SDL_Event& event, int& contIndex);

	void SetRelativeMouseMode(bool value);

	const InputState& GetState() const {
		return mState;
	}

private:
	float Filter1D(int input);
	Vector2 Filter2D(int inputX, int inputY);

private:
	InputState mState;
	std::vector<SDL_GameController*> mControllers;
};