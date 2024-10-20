#include "CameraActor.hpp"
#include "Game.hpp"
#include "MoveComponent.hpp"
#include "AudioComponent.hpp"
#include "MeshComponent.hpp"
#include "AudioSystem.hpp"
#include <SDL/SDL.h>
#include "Renderer.hpp"
#include "Mesh.hpp"

CameraActor::CameraActor(Game* game) : Actor(game) {
	mMoveComp = new MoveComponent(this);
	mAudioComp = new AudioComponent(this);
	mLastFootstep = 0.0f;
	mFootstep = mAudioComp->PlayEvent("event:/Footstep");
	mFootstep.SetPaused(true);
	MeshComponent* mc = new MeshComponent(this, game->GetRenderer()->GetMesh("Assets/Sphere.gpmesh"));
}

CameraActor::~CameraActor() {
}

void CameraActor::UpdateActor(float deltaTime) {
	Actor::UpdateActor(deltaTime);

	// play the footstep if we are moving and haven't recently
	mLastFootstep -= deltaTime;
	if (!Math::NearZero(mMoveComp->GetForwardSpeed()) && mLastFootstep <= 0.0f) {
		mLastFootstep = 0.5f;
		mFootstep.SetPaused(false);
		mFootstep.Restart();
	}

	// compute new camera from this actor
	mCameraPos = GetPosition() - GetForward() * 200.0f + Vector3::UnitZ * 100.0f;
	Vector3 target = GetPosition() + GetForward() * 100.0f;
	Vector3 up = Vector3::UnitZ;

	// create view matrix which represents the position and orientation of the camera
	Matrix4 view = Matrix4::CreateLookAt(mCameraPos, target, up);
	GetGame()->GetRenderer()->SetViewMatrix(view);
	GetGame()->GetAudioSystem()->SetListener(view, mMoveComp->GetVelocity());
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

void CameraActor::SetFootstepSurface(float value) {
	// pause here first
	mFootstep.SetPaused(true);
	mFootstep.SetParameter("Surface", value);
}
