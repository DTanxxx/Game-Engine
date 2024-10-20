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
	const Vector3& GetPosition() const {
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
	const Quaternion& GetRotation() const {
		return mRotation;
	}
	void SetRotation(const Quaternion& rotation) {
		mRotation = rotation;
		mRecomputeWorldTransform = true;
	}

	// transform the initial forward vector (+x) by the rotation quaternion
	Vector3 GetForward() const {
		return Vector3::Transform(Vector3::UnitX, mRotation);
	}

	Vector3 GetRight() const {
		// rotate right axis using quaternion rotation
		return Vector3::Transform(Vector3::UnitY, mRotation);
	}

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
	const Matrix4& GetWorldTransform() const {
		return mWorldTransform;
	}

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