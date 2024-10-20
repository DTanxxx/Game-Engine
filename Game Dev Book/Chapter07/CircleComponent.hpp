#pragma once

#include "Component.hpp"
#include "Math.hpp"
#include "Actor.hpp"

class CircleComponent : public Component {
public:
	CircleComponent(class Actor* owner);

	void SetRadius(float radius) {
		mRadius = radius;
	}

	float GetRadius() const {
		return mRadius * mOwner->GetScale();
	}

	const Vector3& GetCenter() const;  // the center will be the position of owning actor

private:
	float mRadius;
};

bool Intersect(const CircleComponent& a, const CircleComponent& b);