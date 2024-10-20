#include "MoveComponent.hpp"
#include "Actor.hpp"

MoveComponent::MoveComponent(Actor* owner, int updateOrder) : Component(owner, updateOrder) {
	mAngularSpeed = 0.0f;
	mForwardSpeed = 0.0f;
	mSumForces = Vector2::Zero;
	mVelocity = Vector2::Zero;
}

void MoveComponent::AddForce(const Vector2& force) {
	mSumForces += force;
}

void MoveComponent::Update(float deltaTime) {
	// Exercise 3.3
	// calculate acceleration
	Vector2 acceleration{
		mSumForces.x / mMass,
		mSumForces.y / mMass
	};

	// Euler integration
	// update velocity
	mVelocity += acceleration * deltaTime;
	// update position 
	Vector2 pos = mOwner->GetPosition();
	pos += mVelocity * deltaTime;

	// screen wrapping code only for asteroids
	if (pos.x < 0.0f) {
		pos.x = 1022.0f;
	}
	else if (pos.x > 1024.0f) {
		pos.x = 2.0f;
	}

	if (pos.y < 0.0f) {
		pos.y = 766.0f;
	}
	else if (pos.y > 768.0f) {
		pos.y = 2.0f;
	}

	// set position
	mOwner->SetPosition(pos);
	// set rotation
	if (!Math::NearZero(mVelocity.LengthSq())) {
		// we have a velocity
		Vector2 forwardDir = Vector2::Normalize(mVelocity);
		float angle = Math::Atan2(-forwardDir.y, forwardDir.x);
		mOwner->SetRotation(angle);
	}
	
	// clear mSumForces for next frame
	mSumForces = Vector2::Zero;

	/*
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

		// screen wrapping code only for asteroids
		if (pos.x < 0.0f) {
			pos.x = 1022.0f;
		}
		else if (pos.x > 1024.0f) {
			pos.x = 2.0f;
		}

		if (pos.y < 0.0f) {
			pos.y = 766.0f;
		}
		else if (pos.y > 768.0f) {
			pos.y = 2.0f;
		}

		mOwner->SetPosition(pos);
	}
	*/
}