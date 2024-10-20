#pragma once

class Component {
public:
	// constructor
	// (the lower the update order, the earlier the component updates)
	Component(class Actor* owner, int updateOrder = 100);
	// destructor
	virtual ~Component();

	// update this component by delta time
	virtual void Update(float deltaTime);

	int GetUpdateOrder() const {
		return mUpdateOrder;
	}

protected:
	// owning actor
	class Actor* mOwner;

	// update order of component
	int mUpdateOrder;
};