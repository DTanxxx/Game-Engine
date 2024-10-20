#pragma once

#include <string>
#include "Component.hpp"
#include <unordered_map>

// AI state machine
class AIComponent : public Component {
public:
	AIComponent(class Actor* owner);

	void Update(float deltaTime) override;
	void ChangeState(const std::string& name);

	// add a new state to the map
	void RegisterState(class AIState* state);

	class Actor* GetOwningActor() const {
		return mOwner;
	}

private:
	// maps name of state to AIState instance
	std::unordered_map<std::string, class AIState*> mStateMap;
	// current state we are in
	class AIState* mCurrentState;
};