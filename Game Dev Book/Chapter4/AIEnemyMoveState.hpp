#pragma once

#include "AIState.hpp"

class AIEnemyMoveState : public AIState {
public:
	AIEnemyMoveState(class AIComponent* owner);

	void Update(float deltaTime) override;
	void OnEnter() override;
	void OnExit() override;

	const char* GetName() const override {
		return "Move";
	}
};