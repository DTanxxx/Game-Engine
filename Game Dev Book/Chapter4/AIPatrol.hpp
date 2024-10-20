#pragma once

#include "AIState.hpp"

// patrol state
class AIPatrol : public AIState {
public:
	AIPatrol(class AIComponent* owner);

	// override with behaviours for this state
	void Update(float deltaTime) override;
	void OnEnter() override;
	void OnExit() override;

	const char* GetName() const override {
		return "Patrol";
	}
};