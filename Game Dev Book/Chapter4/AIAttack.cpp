#include "AIAttack.hpp"
#include "AIComponent.hpp"

AIAttack::AIAttack(AIComponent* owner) : AIState(owner) {

}

void AIAttack::Update(float deltaTime) {
	// do some other updating
	// ...

	bool dead = /* Figure out if I'm dead */false;
	if (dead) {
		// tell the AI component to change states
		mOwner->ChangeState("Death");
		return;
	}
}

void AIAttack::OnEnter() {

}

void AIAttack::OnExit() {

}