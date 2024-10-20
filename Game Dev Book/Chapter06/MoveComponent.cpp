#include "MoveComponent.hpp"
#include "Actor.hpp"

MoveComponent::MoveComponent(Actor* owner, int updateOrder) : Component(owner, updateOrder) {
	mAngularSpeed = 0.0f;
	mForwardSpeed = 0.0f;
}

void MoveComponent::Update(float deltaTime) {
	// check if corresponding speeds are near zero for forward and angular; if so then don't bother updating the position or rotation of actor
	if (!Math::NearZero(mAngularSpeed)) {
		Quaternion rot = mOwner->GetRotation();
		float angle = mAngularSpeed * deltaTime;
		
		// create new quaternion for incremental rotation (rotate about z axis)
		Quaternion inc(Vector3::UnitZ, angle);

		// concatenate old and new quaternions
		rot = Quaternion::Concatenate(rot, inc);

		mOwner->SetRotation(rot);
	}

	if (!Math::NearZero(mForwardSpeed)) {
		Vector3 pos = mOwner->GetPosition();
		pos += mForwardSpeed * deltaTime * mOwner->GetForward();
		
		// screen wrapping
		if (pos.x < -512.0f) { 
			pos.x = 510.0f; 
		}
		else if (pos.x > 512.0f) {
			pos.x = -510.0f; 
		}
		
		if (pos.y < -384.0f) { 
			pos.y = 382.0f; 
		}
		else if (pos.y > 384.0f) {
			pos.y = -382.0f;
		}

		mOwner->SetPosition(pos);
	}
}