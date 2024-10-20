#pragma once

#include "AIState.hpp"

// attack state
class AIAttack : public AIState {
public:
	AIAttack(class AIComponent* owner);

	// override with behaviours for this state
	void Update(float deltaTime) override;
	void OnEnter() override;
	void OnExit() override;

	const char* GetName() const override {
		return "Attack";
	}
};