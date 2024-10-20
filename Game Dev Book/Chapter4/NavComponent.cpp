#include "NavComponent.hpp"
#include "Tile.hpp"
#include "Actor.hpp"

NavComponent::NavComponent(Actor* owner, int updateOrder) : MoveComponent(owner, updateOrder) {
	mNextNode = nullptr;
}

void NavComponent::TurnTo(const Vector2& pos) {
	// vector from me to pos
	Vector2 dir = pos - mOwner->GetPosition();
	
	// new angle is atan2 of this dir vector (negate y because +y is down on screen)
	float angle = Math::Atan2(-dir.y, dir.x);
	mOwner->SetRotation(angle);
}

void NavComponent::Update(float deltaTime) {
	if (mNextNode) {
		// check whether the actor has reached mNextNode
		// if we have reached the next node, advance along path
		Vector2 diff = mOwner->GetPosition() - mNextNode->GetPosition();
		if (Math::NearZero(diff.Length(), 2.0f)) {
			mNextNode = mNextNode->GetParent();
			TurnTo(mNextNode->GetPosition());
		}
	}
	
	// this moves the actor forward
	MoveComponent::Update(deltaTime);
}

void NavComponent::StartPath(const Tile* start) {
	// initialize mNextNode to the next node in the path and rotate this actor to face that node
	mNextNode = start->GetParent();
	TurnTo(mNextNode->GetPosition());
}