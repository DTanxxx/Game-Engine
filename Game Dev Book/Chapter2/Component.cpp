#include "Component.hpp"
#include "Actor.hpp"

Component::Component(Actor* owner, int updateOrder) {
	mOwner = owner;
	mUpdateOrder = updateOrder;

	// add to actor's vector of components
	owner->AddComponent(this);
}

Component::~Component() {
	mOwner->RemoveComponent(this);

	// NOTE: DO NOT DELETE mOwner POINTER BECAUSE IT WILL REMOVE THE OWNER ACTOR INSTANCE ITSELF
}

void Component::Update(float deltaTime) {

}