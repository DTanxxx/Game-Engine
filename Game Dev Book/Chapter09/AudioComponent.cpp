#include "AudioComponent.hpp"
#include "Actor.hpp"
#include "Game.hpp"
#include "AudioSystem.hpp"

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
			event.Set3DAttributes(world);
		}
	}
}

SoundEvent AudioComponent::PlayEvent(const std::string& name) {
	SoundEvent e = mOwner->GetGame()->GetAudioSystem()->PlayEvent(name);
	// is this 2D or 3D?
	if (e.Is3D()) {
		mEvents3D.emplace_back(e);
		// set initial 3D attributes
		e.Set3DAttributes(mOwner->GetWorldTransform());
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
