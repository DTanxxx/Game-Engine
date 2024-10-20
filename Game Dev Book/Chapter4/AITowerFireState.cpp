#include "AITowerFireState.hpp"
#include "AIComponent.hpp"
#include "Math.hpp"
#include "Actor.hpp"
#include "Game.hpp"
#include "Enemy.hpp"
#include "Bullet.hpp"

AITowerFireState::AITowerFireState(AIComponent* owner) : AIState(owner) {

}

void AITowerFireState::Update(float deltaTime) {
	// fire a bullet at an enemy (if applicable), then transition back to AITowerRestState
	Enemy* e = mOwner->GetOwningActor()->GetGame()->GetNearestEnemy(mOwner->GetOwningActor()->GetPosition());
	// check if there is an enemy
	if (e != nullptr) {
		// vector from tower to enemy
		Vector2 dir = e->GetPosition() - mOwner->GetOwningActor()->GetPosition();
		float dist = dir.Length();
		// check if enemy is within firing proximity
		if (dist < AttackRange) {
			// rotate to face enemy
			mOwner->GetOwningActor()->SetRotation(Math::Atan2(-dir.y, dir.x));

			// spawn bullet at tower position facing enemy
			Bullet* b = new Bullet(mOwner->GetOwningActor()->GetGame());
			b->SetPosition(mOwner->GetOwningActor()->GetPosition());
			b->SetRotation(mOwner->GetOwningActor()->GetRotation());
		}
	}

	mOwner->ChangeState("Rest");
}

void AITowerFireState::OnEnter() {

}

void AITowerFireState::OnExit() {

}
