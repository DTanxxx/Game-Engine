#include "AIPatrol.hpp"
#include "AIComponent.hpp"

AIPatrol::AIPatrol(AIComponent* owner) : AIState(owner) {

}

void AIPatrol::Update(float deltaTime) {
	// do some other updating
	// ...

	bool dead = /* Figure out if I'm dead */false;
	if (dead) {
		// tell the AI component to change states
		mOwner->ChangeState("Death");
		return;
	}

	bool foundPlayer = /* Figure out if AI finds player */false;
	if (foundPlayer) {
		mOwner->ChangeState("Attack");
		return;
	}
}

void AIPatrol::OnEnter() {

}

void AIPatrol::OnExit() {

}