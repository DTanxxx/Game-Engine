#include "MoveComponent.hpp"
#include "Actor.hpp"

MoveComponent::MoveComponent(Actor* owner, int updateOrder) : Component(owner, updateOrder) {
	mAngularSpeed = 0.0f;
	mForwardSpeed = 0.0f;
}

void MoveComponent::Update(float deltaTime) {
	// position += deltaTime * forwardSpeed * forwardVector
	// rotation += angularSpeed * deltaTime;

	// check if corresponding speeds are near zero for forward and angular; if so then don't bother updating the position or rotation of actor
	if (!Math::NearZero(mAngularSpeed)) {
		float rot = mOwner->GetRotation();
		rot += mAngularSpeed * deltaTime;
		mOwner->SetRotation(rot);
	}

	if (!Math::NearZero(mForwardSpeed)) {
		Vector2 pos = mOwner->GetPosition();
		pos += mForwardSpeed * deltaTime * mOwner->GetForward();
		mOwner->SetPosition(pos);
	}
}