#include "Actor.hpp"
#include "Game.hpp"
#include "Component.hpp"
#include <algorithm>

Actor::Actor(Game* game) {
	mGame = game;
	mGame->AddActor(this);

	mState = State::EActive;
	mPosition = Vector2::Zero;
	mScale = 1.0f;
	mRotation = 0.0f;
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

	UpdateComponents(deltaTime);
	UpdateActor(deltaTime);
}

void Actor::UpdateComponents(float deltaTime) {
	for (int i = 0; i < mComponents.size(); ++i) {
		mComponents[i]->Update(deltaTime);
	}
}

void Actor::UpdateActor(float deltaTime) {

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