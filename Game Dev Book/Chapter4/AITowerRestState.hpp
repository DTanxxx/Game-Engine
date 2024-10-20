#pragma once

#include "AIState.hpp"

class AITowerRestState : public AIState {
public:
	AITowerRestState(class AIComponent* owner);

	void Update(float deltaTime) override;
	void OnEnter() override;
	void OnExit() override;

	const char* GetName() const override {
		return "Rest";
	}

private:
	float mNextAttack;  // tower attack timer
	const float AttackTime = 2.5f;
};