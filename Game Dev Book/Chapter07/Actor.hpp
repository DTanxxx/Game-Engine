#pragma once

#include <vector>
#include "Math.hpp"
#include "Component.hpp"
#include <cstdint>

class Actor {
public:
	// used to track state of actor
	enum State {
		EActive,
		EPaused,
		EDead
	};

	// constructor/destructor
	Actor(class Game* game);
	virtual ~Actor();

	// update function called from Game (not overridable)
	void Update(float deltaTime);
	// updates all the components attached to the actor (not overridable)
	void UpdateComponents(float deltaTime);
	// any actor-specific update code (overridable)
	virtual void UpdateActor(float deltaTime);
	
	// ProcessInput function called from Game (not overridable)
	void ProcessInput(const uint8_t* keyState);
	// any actor-specific input code (overridable)
	virtual void ActorInput(const uint8_t* keyState);

	// getters/setters
	Vector3 GetPosition() const {
		return mPosition;
	}
	void SetPosition(const Vector3& pos) {
		mPosition = pos;
		mRecomputeWorldTransform = true;
	}
	float GetScale() const {
		return mScale;
	}
	void SetScale(float scale) {
		mScale = scale;
		mRecomputeWorldTransform = true;
	}
	Quaternion GetRotation() const {
		return mRotation;
	}
	void SetRotation(Quaternion rotation) {
		mRotation = rotation;
		mRecomputeWorldTransform = true;
	}

	Vector3 GetForward() const;

	State GetState() const {
		return mState;
	}
	void SetState(State state) {
		mState = state;
	}

	class Game* GetGame() {
		return mGame;
	}

	// add/remove components
	void AddComponent(Component* component);
	void RemoveComponent(Component* component);

	void ComputeWorldTransform();
	Matrix4 GetWorldTransform();

	template<typename T>
	T GetComponent() const {
		for (int i = 0; i < mComponents.size(); ++i) {
			Component* comp = mComponents[i];
			if (static_cast<T>(comp) != nullptr) {
				return static_cast<T>(comp);
			}
		}
		return nullptr;
	}

private:
	// actor's state
	State mState;

	// transform
	Vector3 mPosition;  // center position of actor
	float mScale;  // uniform scale of actor (1.0f for 100%)
	Quaternion mRotation;
	Matrix4 mWorldTransform;  // need 4x4 matrix since the homogenous coords for 3D are (x,y,z,w)
	bool mRecomputeWorldTransform;  // want to recompute world transform matrix only if the actor's position, scale, or rotation changes

	// components held by this actor
	std::vector<Component*> mComponents;

	class Game* mGame;
};