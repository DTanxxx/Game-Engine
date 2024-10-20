#pragma once

// a state in the AI state machine
class AIState {
public:
	AIState(class AIComponent* owner) : mOwner(owner) {}

	// state-specific behaviour
	virtual void Update(float deltaTime) = 0;  // updates the state per frame
	virtual void OnEnter() = 0;  // implements any entry transition code
	virtual void OnExit() = 0;  // implements any exit transition code

	// getter for string name of state
	virtual const char* GetName() const = 0;

protected:
	class AIComponent* mOwner;  // the associated AIComponent with this state
};