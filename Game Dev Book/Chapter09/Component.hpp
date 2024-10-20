#pragma once

#include <cstdint>

class Component {
public:
	// constructor
	// (the lower the update order, the earlier the component updates)
	Component(class Actor* owner, int updateOrder = 100);
	// destructor
	virtual ~Component();

	// update this component by delta time
	virtual void Update(float deltaTime);

	// process input for this component
	virtual void ProcessInput(const uint8_t* keyState) {}

	// called to notify components when their owning actor's world transform gets updated
	virtual void OnUpdateWorldTransform() {}

	int GetUpdateOrder() const {
		return mUpdateOrder;
	}

protected:
	// owning actor
	class Actor* mOwner;

	// update order of component
	int mUpdateOrder;
};