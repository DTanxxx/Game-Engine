#include "FPSActor.hpp"
#include "Game.hpp"
#include "MoveComponent.hpp"
#include "FPSCamera.hpp"
#include "Renderer.hpp"
#include "Mesh.hpp"
#include "MeshComponent.hpp"

FPSActor::FPSActor(Game* game) : Actor(game) {
	mMoveComp = new MoveComponent(this);
	mCameraComp = new FPSCamera(this);

	// create a mesh for the rifle model
	mFPSModel = new Actor(game);
	mFPSModel->SetScale(0.75f);
	mMeshComp = new MeshComponent(mFPSModel, game->GetRenderer()->GetMesh("Assets/Rifle.gpmesh"));
}

FPSActor::~FPSActor() {
	Actor::~Actor();
}

void FPSActor::UpdateActor(float deltaTime) {
	Actor::UpdateActor(deltaTime);

	// update position of FPS model relative to actor position
	const Vector3 modelOffset(Vector3(10.0f, 10.0f, -10.0f));
	Vector3 modelPos = GetPosition();
	modelPos += GetForward() * modelOffset.x;
	modelPos += GetRight() * modelOffset.y;
	modelPos.z += modelOffset.z;
	mFPSModel->SetPosition(modelPos);

	// initialize rotation to actor rotation
	Quaternion q = GetRotation();
	// rotate by pitch from camera
	q = Quaternion::Concatenate(q, Quaternion(GetRight(), mCameraComp->GetPitch()));
	mFPSModel->SetRotation(q);
}

void FPSActor::ActorInput(const uint8_t* keyState) {
	float forwardSpeed = 0.0f;
	float strafeSpeed = 0.0f;

	// wasd movement
	if (keyState[SDL_SCANCODE_W]) {
		forwardSpeed += 400.0f;
	}
	if (keyState[SDL_SCANCODE_S]) {
		forwardSpeed -= 400.0f;
	}
	if (keyState[SDL_SCANCODE_A]) {
		strafeSpeed -= 400.0f;
	}
	if (keyState[SDL_SCANCODE_D]) {
		strafeSpeed += 400.0f;
	}

	mMoveComp->SetForwardSpeed(forwardSpeed);
	mMoveComp->SetStrafeSpeed(strafeSpeed);

	// convert mouse left/right movements to angular speed
	// first get relative mouse movement (between frames)
	int x, y;
	Uint32 buttons = SDL_GetRelativeMouseState(&x, &y);
	// assume mouse movement is usually between -500 and +500 (between frames)
	const int maxMouseSpeed = 500;
	// rotation/sec at maximum speed
	const float maxAngularSpeed = Math::Pi * 8;

	float angularSpeed = 0.0f;
	if (x != 0) {
		// convert to approximately [-1.0, 1.0]
		angularSpeed = static_cast<float>(x) / maxMouseSpeed;
		// multiply by rotation/sec
		angularSpeed *= maxAngularSpeed;
	}

	mMoveComp->SetAngularSpeed(angularSpeed);

	// update FPS camera's pitch using mouse up/down movements
	const float maxPitchSpeed = Math::Pi * 8;
	float pitchSpeed = 0.0f;
	if (y != 0) {
		// convert to approximately [-1.0, 1.0]
		pitchSpeed = static_cast<float>(y) / maxMouseSpeed;
		pitchSpeed *= maxPitchSpeed;
	}

	mCameraComp->SetPitchSpeed(pitchSpeed);
}