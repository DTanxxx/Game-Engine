#include "Actor.hpp"
#include "Game.hpp"
#include <algorithm>

Actor::Actor(Game* game) {
	mGame = game;
	mGame->AddActor(this);

	mState = State::EActive;
	mPosition = Vector3::Zero;
	mScale = 1.0f;
	mRotation = Quaternion::Identity;
	mRecomputeWorldTransform = true;
}

Actor::~Actor() {
	mGame->RemoveActor(this);

	// NOTE: DO NOT DELETE mGame POINTER BECAUSE IT WILL REMOVE THE GAME INSTANCE ITSELF!

	// need to delete components
	// because ~Component calls RemoveComponent, need a different style loop
	while (!mComponents.empty()) {
		delete mComponents.back();
	}
}

void Actor::Update(float deltaTime) {
	if (mState != State::EActive) {
		return;
	}

	ComputeWorldTransform();

	UpdateComponents(deltaTime);
	UpdateActor(deltaTime);

	ComputeWorldTransform();  // call it twice in case the transform changes in the interim
}

void Actor::UpdateComponents(float deltaTime) {
	for (int i = 0; i < mComponents.size(); ++i) {
		mComponents[i]->Update(deltaTime);
	}
}

void Actor::UpdateActor(float deltaTime) {

}

void Actor::ComputeWorldTransform() {
	if (mRecomputeWorldTransform) {
		mRecomputeWorldTransform = false;

		// scale, then rotate, then translate
		mWorldTransform = Matrix4::CreateScale(mScale);
		mWorldTransform *= Matrix4::CreateFromQuaternion(mRotation);
		mWorldTransform *= Matrix4::CreateTranslation(mPosition);

		// inform components world transform has updated
		for (auto comp : mComponents) {
			comp->OnUpdateWorldTransform();
		}
	}
}

Matrix4 Actor::GetWorldTransform() {
	return mWorldTransform;
}

void Actor::ProcessInput(const uint8_t* keyState) {
	// check if the actor's state is active
	if (mState == EActive) {
		// calls ProcessInput() on each of its component
		for (auto comp : mComponents) {
			comp->ProcessInput(keyState);
		}
		
		// calls actor-overridable input logic
		ActorInput(keyState);
	}
}

void Actor::ActorInput(const uint8_t* keyState) {

}

void Actor::AddComponent(Component* component) {
	// find the insertion point in the sorted vector - the first element with a order higher than me
	int updateOrder = component->GetUpdateOrder();
	auto iter = mComponents.begin();
	for (; iter != mComponents.end(); ++iter) {
		if ((*iter)->GetUpdateOrder() > updateOrder) {
			break;
		}
	}
	
	// inserts element before position of iterator
	mComponents.insert(iter, component);
}

void Actor::RemoveComponent(Component* component) {
	auto iter = std::find(mComponents.begin(), mComponents.end(), component);
	if (iter != mComponents.end()) {
		mComponents.erase(iter);
	}
}

// transform the initial forward vector (+x) by the rotation quaternion
Vector3 Actor::GetForward() const {
	return Vector3::Transform(Vector3::UnitX, mRotation);
}
