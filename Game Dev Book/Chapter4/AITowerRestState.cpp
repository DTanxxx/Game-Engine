#include "AITowerRestState.hpp"
#include "AIComponent.hpp"

AITowerRestState::AITowerRestState(AIComponent* owner) : AIState(owner) {
	mNextAttack = AttackTime;
}

void AITowerRestState::Update(float deltaTime) {
	mNextAttack -= deltaTime;
	// check if attack cooldown has finished
	if (mNextAttack <= 0.0f) {
		// transition to AITowerFireState
		mOwner->ChangeState("Fire");
		mNextAttack += AttackTime;
	}
}

void AITowerRestState::OnEnter() {

}

void AITowerRestState::OnExit() {

}
