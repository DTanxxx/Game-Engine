#include "CameraActor.hpp"
#include "Game.hpp"
#include "MoveComponent.hpp"
#include <SDL/SDL.h>
#include "Renderer.hpp"

CameraActor::CameraActor(Game* game) : Actor(game) {
	mMoveComp = new MoveComponent(this);
}

CameraActor::~CameraActor() {
}

void CameraActor::UpdateActor(float deltaTime) {
	Actor::UpdateActor(deltaTime);

	// compute new camera from this actor
	Vector3 cameraPos = GetPosition();
	Vector3 target = GetPosition() + GetForward() * 100.0f;
	Vector3 up = Vector3::UnitZ;

	// create view matrix which represents the position and orientation of the camera
	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
	GetGame()->GetRenderer()->SetViewMatrix(view);
}

void CameraActor::ActorInput(const uint8_t* keys) {
	float forwardSpeed = 0.0f;
	float angularSpeed = 0.0f;

	// wasd movement
	if (keys[SDL_SCANCODE_W]) {
		forwardSpeed += 300.0f;
	}
	if (keys[SDL_SCANCODE_S]) {
		forwardSpeed -= 300.0f;
	}
	if (keys[SDL_SCANCODE_D]) {
		angularSpeed += Math::TwoPi;
	}
	if (keys[SDL_SCANCODE_A]) {
		angularSpeed -= Math::TwoPi;
	}

	mMoveComp->SetForwardSpeed(forwardSpeed);
	mMoveComp->SetAngularSpeed(angularSpeed);
}