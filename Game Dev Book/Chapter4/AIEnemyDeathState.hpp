#pragma once

#include "AIState.hpp"

class AIEnemyDeathState : public AIState {
public:
	AIEnemyDeathState(class AIComponent* owner);

	void Update(float deltaTime) override;
	void OnEnter() override;
	void OnExit() override;

	const char* GetName() const override {
		return "Death";
	}
};