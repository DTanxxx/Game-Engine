#include "InputSystem.hpp"
#include <SDL/SDL_keyboard.h>
#include <cstring>
#include <SDL/SDL_mouse.h>
#include <SDL/SDL_log.h>

ButtonState KeyboardState::GetKeyState(SDL_Scancode keyCode) const {
	if (mPrevState[keyCode] == 0) {
		if (mCurrState[keyCode] == 0) {
			return ENone;
		}
		else {
			return EPressed;
		}
	}
	else {
		// prev state must be 1
		if (mCurrState[keyCode] == 0) {
			return EReleased;
		}
		else {
			return EHeld;
		}
	}
}

bool KeyboardState::GetKeyValue(SDL_Scancode keyCode) const {
	return mCurrState[keyCode] == 1;
}

ButtonState MouseState::GetButtonState(int button) const {
	int mask = SDL_BUTTON(button);
	if ((mask & mPrevButtons) == 0) {
		if ((mask & mCurrButtons) == 0) {
			return ENone;
		}
		else {
			return EPressed;
		}
	}
	else {
		if ((mask & mCurrButtons) == 0) {
			return EReleased;
		}
		else {
			return EHeld;
		}
	}
}

bool MouseState::GetButtonValue(int button) const {
	return (mCurrButtons & SDL_BUTTON(button));
}

ButtonState ControllerState::GetButtonState(SDL_GameControllerButton button) const {
	if (mPrevButtons[button] == 0) {
		if (mCurrButtons[button] == 0) {
			return ENone;
		}
		else {
			return EPressed;
		}
	}
	else {
		if (mCurrButtons[button] == 0) {
			return EReleased;
		}
		else {
			return EHeld;
		}
	}
}

bool ControllerState::GetButtonValue(SDL_GameControllerButton button) const {
	return mCurrButtons[button] == 1;
}

InputSystem::InputSystem() {
}

InputSystem::~InputSystem() {
}

bool InputSystem::Initialize() {
	// keyboard
	// assign current state pointer
	mState.Keyboard.mCurrState = SDL_GetKeyboardState(NULL);
	// clear previous state memory
	memset(mState.Keyboard.mPrevState, 0, SDL_NUM_SCANCODES);
	
	// mouse
	mState.Mouse.mPrevButtons = 0;
	mState.Mouse.mCurrButtons = 0;
	mState.Mouse.mIsRelative = false;

	// controller(s)
	for (int i = 0; i < 4; ++i) {
		// get the connected controller, if it exists
		SDL_GameController* cont = SDL_GameControllerOpen(i);
		mControllers.emplace_back(cont);

		// initialize controller state
		ControllerState contState = ControllerState();
		contState.mIsConnected = (cont != nullptr);
		contState.mIndex = i;

		// clear out memory for both mCurrButtons and mPrevButtons
		memset(contState.mCurrButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
		memset(contState.mPrevButtons, 0, SDL_CONTROLLER_BUTTON_MAX);

		mState.Controllers.emplace_back(contState);
	}
	
	return true;
}

void InputSystem::Shutdown() {

}

void InputSystem::SetRelativeMouseMode(bool value) {
	// enables/disables relative mouse mode
	SDL_bool set = value ? SDL_TRUE : SDL_FALSE;
	SDL_SetRelativeMouseMode(set);

	mState.Mouse.mIsRelative = value;
}

void InputSystem::PrepareForUpdate() {
	// copy "current" data to the previous state buffer
	// keyboard
	memcpy(mState.Keyboard.mPrevState, mState.Keyboard.mCurrState, SDL_NUM_SCANCODES);
	
	// mouse
	mState.Mouse.mPrevButtons = mState.Mouse.mCurrButtons;
	// reset scroll wheel data because the mouse wheel event only triggers
	// on frames where the wheel moves
	mState.Mouse.mScrollWheel = Vector2::Zero;
	mState.Mouse.mIsRelative = false;

	// controller(s)
	for (int i = 0; i < mState.Controllers.size(); ++i) {
		memcpy(mState.Controllers[i].mPrevButtons, mState.Controllers[i].mCurrButtons, SDL_CONTROLLER_BUTTON_MAX);
	}
}

void InputSystem::Update() {
	// mouse
	int x = 0;
	int y = 0;
	if (mState.Mouse.mIsRelative) {
		mState.Mouse.mCurrButtons = SDL_GetRelativeMouseState(&x, &y);
	}
	else {
		mState.Mouse.mCurrButtons = SDL_GetMouseState(&x, &y);
	}
	
	mState.Mouse.mMousePos.x = static_cast<float>(x);
	mState.Mouse.mMousePos.y = static_cast<float>(y);

	// controller(s)
	for (int i = 0; i < mState.Controllers.size(); ++i) {
		if (!mState.Controllers[i].GetIsConnected()) {
			// controller is not connected, skip it
			continue;
		}

		// buttons
		for (int j = 0; j < SDL_CONTROLLER_BUTTON_MAX; ++j) {
			mState.Controllers[i].mCurrButtons[j] = SDL_GameControllerGetButton(mControllers[i], SDL_GameControllerButton(j));
		}

		// triggers
		mState.Controllers[i].mLeftTrigger = Filter1D(SDL_GameControllerGetAxis(mControllers[i], SDL_CONTROLLER_AXIS_TRIGGERLEFT));
		mState.Controllers[i].mRightTrigger = Filter1D(SDL_GameControllerGetAxis(mControllers[i], SDL_CONTROLLER_AXIS_TRIGGERRIGHT));

		// sticks
		x = SDL_GameControllerGetAxis(mControllers[i], SDL_CONTROLLER_AXIS_LEFTX);
		y = -SDL_GameControllerGetAxis(mControllers[i], SDL_CONTROLLER_AXIS_LEFTY);  // negate y because SDL reports y-axis where +y is down
		mState.Controllers[i].mLeftStick = Filter2D(x, y);

		x = SDL_GameControllerGetAxis(mControllers[i], SDL_CONTROLLER_AXIS_RIGHTX);
		y = -SDL_GameControllerGetAxis(mControllers[i], SDL_CONTROLLER_AXIS_RIGHTY);
		mState.Controllers[i].mRightStick = Filter2D(x, y);
	}
}

void InputSystem::ProcessEvent(SDL_Event& event, int& contIndex) {
	int controllerIndex;
	SDL_GameController* cont;
	ControllerState contState;

	switch (event.type) {
	case SDL_MOUSEWHEEL:
		mState.Mouse.mScrollWheel = Vector2(
			static_cast<float>(event.wheel.x),
			static_cast<float>(event.wheel.y));
		break;
	case SDL_CONTROLLERDEVICEADDED:
		controllerIndex = event.cdevice.which;
		SDL_Log("Added controller index %i", controllerIndex);
		// get the connected controller
		cont = SDL_GameControllerOpen(controllerIndex);

		if (mControllers.size() < 4) {
			mControllers.emplace_back(cont);
		}
		else {
			// iterate over mControllers and replace the first null with cont
			for (int i = 0; i < mControllers.size(); ++i) {
				if (mControllers[i] == nullptr) {
					mControllers[i] = cont;
					break;
				}
			}
		}
		
		// initialize controller state
		contState = ControllerState();
		contState.mIsConnected = (cont != nullptr);
		contState.mIndex = controllerIndex;

		// clear out memory for both mCurrButtons and mPrevButtons
		memset(contState.mCurrButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
		memset(contState.mPrevButtons, 0, SDL_CONTROLLER_BUTTON_MAX);

		if (mState.Controllers.size() < 4) {
			mState.Controllers.emplace_back(contState);
		}
		else {
			// iterate over mState.Controllers and replace the first !mIsConnected with contState
			for (int i = 0; i < mState.Controllers.size(); ++i) {
				if (!mState.Controllers[i].GetIsConnected()) {
					mState.Controllers[i] = contState;
					break;
				}
			}
		}

		contIndex = controllerIndex;

		break;
	case SDL_CONTROLLERDEVICEREMOVED:
		controllerIndex = event.cdevice.which;
		SDL_Log("Removed controller index %i", controllerIndex);
		// get the controller
		for (int i = 0; i < mState.Controllers.size(); ++i) {
			if (mState.Controllers[i].GetIsConnected() && mState.Controllers[i].GetControllerIndex() == controllerIndex) {
				SDL_GameControllerClose(mControllers[i]);
				mControllers[i] = nullptr;
				mState.Controllers[i].mIsConnected = false;
			}
		}
		
		contIndex = controllerIndex;

		break;
	default:
		break;
	}
}

float InputSystem::Filter1D(int input) {
	// a value < dead zone is interpreted as 0%
	const int deadZone = 250;
	// a value > max value is interpreted as 100%
	const int maxValue = 30000;

	float retVal = 0.0f;

	// take absolute value of input
	int absValue = input > 0 ? input : -input;
	// ignore input within dead zone
	if (absValue > deadZone) {
		// compute fractional value between dead zone and max value
		retVal = static_cast<float>(absValue - deadZone) / static_cast<float>(maxValue - deadZone);

		// make sure sign matches original value
		retVal = input > 0 ? retVal : -1.0f * retVal;

		// clamp between -1.0f and 1.0f
		retVal = Math::Clamp(retVal, -1.0f, 1.0f);
	}
	
	return retVal;
}

Vector2 InputSystem::Filter2D(int inputX, int inputY) {
	const float deadZone = 8000.0f;
	const float maxValue = 30000.0f;

	// make it into 2D vector
	Vector2 dir;
	dir.x = static_cast<float>(inputX);
	dir.y = static_cast<float>(inputY);

	float length = dir.Length();

	// if length < dead zone, should be no input
	if (length < deadZone) {
		dir = Vector2::Zero;
	}
	else {
		// calculate fractional value between dead zone and max value circles
		float f = (length - deadZone) / (maxValue - deadZone);
		// clamp f between 0.0f and 1.0f
		f = Math::Clamp(f, 0.0f, 1.0f);
		// normalize the vector, and then scale it to the fractional value
		dir *= f / length;
	}

	return dir;
}
