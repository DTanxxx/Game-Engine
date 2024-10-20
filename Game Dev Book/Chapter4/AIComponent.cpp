#include "AIComponent.hpp"
#include "Actor.hpp"
#include "AIState.hpp"
#include "SDL_log.h"

AIComponent::AIComponent(Actor* owner) : Component(owner) {
	mCurrentState = nullptr;
}

void AIComponent::RegisterState(AIState* state) {
	mStateMap.emplace(state->GetName(), state);
}

void AIComponent::Update(float deltaTime) {
	// calls Update() on the current state
	if (mCurrentState) {
		mCurrentState->Update(deltaTime);
	}
}

void AIComponent::ChangeState(const std::string& name) {
	// first exit the current state
	if (mCurrentState) {
		mCurrentState->OnExit();
	}

	// try to find the new state from the map
	auto iter = mStateMap.find(name);
	if (iter != mStateMap.end()) {
		// found the new state, change mCurrentState to the new state and call OnEnter() on this new state
		mCurrentState = iter->second;
		// we are entering the new state
		mCurrentState->OnEnter();
	}
	else {
		// output error message and set mCurrentState to nullptr
		SDL_Log("Could not find AIState %s in state map", name.c_str());
		mCurrentState = nullptr;
	}
}