#pragma once

#include "Component.hpp"
#include "SoundEvent.hpp"
#include <string>
#include <vector>

// associates sound events with specific actors and updates associated event's 3D attributes
class AudioComponent : public Component {
public:
	AudioComponent(class Actor* owner, int updateOrder = 200);
	~AudioComponent();

	void Update(float deltaTime) override;
	void OnUpdateWorldTransform() override;

	SoundEvent PlayEvent(const std::string& name);
	void StopAllEvents();

private:
	std::vector<SoundEvent> mEvents2D;
	std::vector<SoundEvent> mEvents3D;
};