#include "AIEnemyMoveState.hpp"
#include "AIComponent.hpp"
#include "Math.hpp"
#include "Actor.hpp"
#include "Game.hpp"
#include "Grid.hpp"
#include "Tile.hpp"

AIEnemyMoveState::AIEnemyMoveState(AIComponent* owner) : AIState(owner) {

}

void AIEnemyMoveState::Update(float deltaTime) {
	// check if this enemy has reached the end tile
	Vector2 diff = mOwner->GetOwningActor()->GetPosition() -
		mOwner->GetOwningActor()->GetGame()->GetGrid()->GetEndTile()->GetPosition();
	if (Math::NearZero(diff.Length(), 10.0f)) {
		// reached end tile, transition to AIEnemyDeathState
		mOwner->ChangeState("Death");
	}
}

void AIEnemyMoveState::OnEnter() {

}

void AIEnemyMoveState::OnExit() {

}