#pragma once

#include "AIState.hpp"

class AITowerFireState : public AIState {
public:
	AITowerFireState(class AIComponent* owner);

	void Update(float deltaTime) override;
	void OnEnter() override;
	void OnExit() override;

	const char* GetName() const override {
		return "Fire";
	}

private:
	const float AttackRange = 100.0f;
};