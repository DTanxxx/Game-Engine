#include "MoveComponent.hpp"
#include "Actor.hpp"
#include <SDL/SDL.h>

MoveComponent::MoveComponent(Actor* owner, int updateOrder) : Component(owner, updateOrder) {
	mAngularSpeed = 0.0f;
	mForwardSpeed = 0.0f;
}

void MoveComponent::Update(float deltaTime) {
	// check if corresponding speeds are near zero for forward and angular; if so then don't bother updating the position or rotation of actor
	if (!Math::NearZero(mAngularSpeed)) {
		float rot = mOwner->GetRotation();
		rot += mAngularSpeed * deltaTime;
		mOwner->SetRotation(rot);
	}

	if (!Math::NearZero(mForwardSpeed)) {
		Vector2 pos = mOwner->GetPosition();
		pos += mForwardSpeed * deltaTime * mOwner->GetForward();
		
		// screen wrapping (for asteroids)
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