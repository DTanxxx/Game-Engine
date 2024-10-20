#include "CircleComponent.hpp"

CircleComponent::CircleComponent(Actor* owner) : Component(owner) {
	mRadius = 0.0f;
}

const Vector2& CircleComponent::GetCenter() const {
	return mOwner->GetPosition();
}

bool Intersect(const CircleComponent& a, const CircleComponent& b) {
    // calculate distance squared
    Vector2 diff = a.GetCenter() - b.GetCenter();
    float distSq = diff.LengthSq();

    // calculate sum of radii squared
    float radiiSq = a.GetRadius() + b.GetRadius();
    radiiSq *= radiiSq;

    // two circle colliders only intersect if the (squared) distance between the centers is less than or equal to the (squared) sum of radii
    return distSq <= radiiSq;
}