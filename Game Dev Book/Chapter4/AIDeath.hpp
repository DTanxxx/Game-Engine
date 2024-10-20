#pragma once

#include "AIState.hpp"

// death state
class AIDeath : public AIState {
public:
	AIDeath(class AIComponent* owner);

	// override with behaviours for this state
	void Update(float deltaTime) override;
	void OnEnter() override;
	void OnExit() override;

	const char* GetName() const override {
		return "Death";
	}
};