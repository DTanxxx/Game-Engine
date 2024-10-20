#pragma once

#include "Component.hpp"
#include "Math.hpp"

// allows actors to move forward at a certain speed and update the rotation
class MoveComponent : public Component {
public:
	// lower update order to update first
	MoveComponent(class Actor* owner, int updateOrder = 10);

	void Update(float deltaTime) override;

	float GetAngularSpeed() const {
		return mAngularSpeed;
	}

	float GetForwardSpeed() const {
		return mForwardSpeed;
	}

	void SetAngularSpeed(float speed) {
		mAngularSpeed = speed;
	}

	void SetForwardSpeed(float speed) {
		mForwardSpeed = speed;
	}

	void SetMass(float mass) {
		mMass = mass;
	}

	void SetVelocity(const Vector2& vel) {
		mVelocity = vel;
	}

	const Vector2& GetVelocity() const {
		return mVelocity;
	}

	void SetForce(const Vector2& force) {
		mSumForces = force;
	}

	// to apply an impulse, call this function once; to apply a constant force, call this function every frame for that force
	void AddForce(const Vector2& force);

private:
	// controls rotation (radians/second)
	float mAngularSpeed;
	// controls forward movement (units/second)
	float mForwardSpeed;
	
	float mMass;
	Vector2 mSumForces;
	Vector2 mVelocity;
};