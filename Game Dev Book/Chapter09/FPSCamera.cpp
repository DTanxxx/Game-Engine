#include "FPSCamera.hpp"
#include "Actor.hpp"

FPSCamera::FPSCamera(Actor* owner) : CameraComponent(owner) {
	mPitchSpeed = 0.0f;
	mMaxPitch = Math::Pi / 3.0f;
	mPitch = 0.0f;
}

FPSCamera::~FPSCamera() {
	CameraComponent::~CameraComponent();
}

void FPSCamera::Update(float deltaTime) {
	// call parent update
	CameraComponent::Update(deltaTime);

	// camera position is owner position
	Vector3 cameraPos = mOwner->GetPosition();

	// update pitch based on pitch speed
	mPitch += deltaTime * mPitchSpeed;
	// clamp pitch to [-max, +max]
	mPitch = Math::Clamp(mPitch, -mMaxPitch, mMaxPitch);
	// construct a quaternion representing this pitch rotation which is about owner's right vector
	Quaternion q(mOwner->GetRight(), mPitch);

	// rotate owner's forward vector by this pitch rotation quaternion
	Vector3 viewForward = Vector3::Transform(mOwner->GetForward(), q);
	// target position 100 units in front of view forward
	Vector3 target = cameraPos + viewForward * 100.0f;
	// owner's up vector needs to be transformed too
	Vector3 up = Vector3::Transform(Vector3::UnitZ, q);;
	
	// create look at matrix, set as view
	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
	SetViewMatrix(view);
}
