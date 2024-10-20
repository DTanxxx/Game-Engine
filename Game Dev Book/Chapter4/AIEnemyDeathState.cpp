#include "AIEnemyDeathState.hpp"
#include "AIComponent.hpp"
#include "Actor.hpp"

AIEnemyDeathState::AIEnemyDeathState(AIComponent* owner) : AIState(owner) {

}

void AIEnemyDeathState::Update(float deltaTime) {

}

void AIEnemyDeathState::OnEnter() {
	mOwner->GetOwningActor()->SetState(Actor::EDead);
}

void AIEnemyDeathState::OnExit() {

}