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

	void SetStrafeSpeed(float speed) {
		mStrafeSpeed = speed;
	}

private:
	// controls rotation (radians/second)
	float mAngularSpeed;
	// controls forward movement (units/second)
	float mForwardSpeed;
	// controls left/right strafing
	float mStrafeSpeed;
};