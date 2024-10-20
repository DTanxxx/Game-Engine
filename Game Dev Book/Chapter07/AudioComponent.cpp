#include "AudioComponent.hpp"
#include "MoveComponent.hpp"
#include "Component.hpp"
#include "Actor.hpp"
#include "Game.hpp"
#include "Math.hpp"
#include "AudioSystem.hpp"
#include "CameraActor.hpp"

AudioComponent::AudioComponent(Actor* owner, int updateOrder) : Component(owner, updateOrder) {
}

AudioComponent::~AudioComponent() {
	StopAllEvents();
}

void AudioComponent::Update(float deltaTime) {
	Component::Update(deltaTime);

	// remove invalid 2D events
	auto it = mEvents2D.begin();
	while (it != mEvents2D.end()) {
		if (!it->IsValid()) {
			it = mEvents2D.erase(it);
		}
		else {
			it += 1;
		}
	}

	// remove invalid 3D events
	it = mEvents3D.begin();
	while (it != mEvents3D.end()) {
		if (!it->IsValid()) {
			it = mEvents3D.erase(it);
		}
		else {
			it += 1;
		}
	}
}

void AudioComponent::OnUpdateWorldTransform() {
	// update 3D events' world transforms
	Matrix4 world = mOwner->GetWorldTransform();
	for (auto& event : mEvents3D) {
		if (event.IsValid()) {
			MoveComponent* moveComp = mOwner->GetComponent<MoveComponent*>();

			// Exercise 7.2
			// calculate virtual sound position
			CameraActor* cam = mOwner->GetGame()->GetCameraActor();
			if (cam) {
				Vector3 playerPos = cam->GetPosition();
				Vector3 cameraPos = cam->GetCameraPosition();
				Vector3 soundPos = mOwner->GetPosition();

				Vector3 playerToSound = soundPos - playerPos;
				Vector3 cameraToSound = soundPos - cameraPos;

				// length of playerToSound is the desired distance for attenuation
				// normalized cameraToSound is the correct heading
				Vector3 virtualPos = cameraPos + cameraToSound * (1.0f / cameraToSound.Length()) * playerToSound.Length();

				if (moveComp != nullptr) {
					event.Set3DAttributes(mOwner->GetWorldTransform(), moveComp->GetVelocity(), virtualPos, true);
				}
				else {
					event.Set3DAttributes(mOwner->GetWorldTransform(), Vector3::Zero, virtualPos, true);
				}
			}
			else {
				if (moveComp != nullptr) {
					event.Set3DAttributes(mOwner->GetWorldTransform(), moveComp->GetVelocity());
				}
				else {
					event.Set3DAttributes(mOwner->GetWorldTransform(), Vector3::Zero);
				}
			}
		}
	}
}

SoundEvent AudioComponent::PlayEvent(const std::string& name) {
	SoundEvent e = mOwner->GetGame()->GetAudioSystem()->PlayEvent(name);
	// is this 2D or 3D?
	if (e.Is3D()) {
		mEvents3D.emplace_back(e);
		// set initial 3D attributes
		MoveComponent* moveComp = mOwner->GetComponent<MoveComponent*>();

		// Exercise 7.2
		// calculate virtual sound position
		CameraActor* cam = mOwner->GetGame()->GetCameraActor();
		if (cam) {
			Vector3 playerPos = cam->GetPosition();
			Vector3 cameraPos = cam->GetCameraPosition();
			Vector3 soundPos = mOwner->GetPosition();

			Vector3 playerToSound = soundPos - playerPos;
			Vector3 cameraToSound = soundPos - cameraPos;

			// length of playerToSound is the desired distance for attenuation
			// normalized cameraToSound is the correct heading
			Vector3 virtualPos = cameraPos + cameraToSound * (1.0f / cameraToSound.Length()) * playerToSound.Length();

			if (moveComp != nullptr) {
				e.Set3DAttributes(mOwner->GetWorldTransform(), moveComp->GetVelocity(), virtualPos, true);
			}
			else {
				e.Set3DAttributes(mOwner->GetWorldTransform(), Vector3::Zero, virtualPos, true);
			}
		}
		else {
			if (moveComp != nullptr) {
				e.Set3DAttributes(mOwner->GetWorldTransform(), moveComp->GetVelocity());
			}
			else {
				e.Set3DAttributes(mOwner->GetWorldTransform(), Vector3::Zero);
			}
		}
	}
	else {
		mEvents2D.emplace_back(e);
	}

	return e;
}

void AudioComponent::StopAllEvents() {
	// stop all sounds
	for (auto& e : mEvents2D) {
		e.Stop();
	}
	for (auto& e : mEvents3D) {
		e.Stop();
	}

	// clear events
	mEvents2D.clear();
	mEvents3D.clear();
}
