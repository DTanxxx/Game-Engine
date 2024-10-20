#pragma once

#include "Math.hpp"
#include <string>
#include <unordered_map>

// forward declarations to avoid including FMOD header
namespace FMOD {
	class System;
	namespace Studio {
		class Bank;
		class EventDescription;
		class EventInstance;
		class System;
		class Bus;
	};
};

class AudioSystem {
public:
	AudioSystem(class Game* game);
	~AudioSystem();

	bool Initialize();
	void Shutdown();

	// load/unload banks
	void LoadBank(const std::string& name);
	void UnloadBank(const std::string& name);
	void UnloadAllBanks();

	class SoundEvent PlayEvent(const std::string& name);

	void Update(float deltaTime);

	// for positional audio
	void SetListener(const Matrix4& viewMatrix);

	// control buses
	float GetBusVolume(const std::string& name) const;
	bool GetBusPaused(const std::string& name) const;
	void SetBusVolume(const std::string& name, float volume);
	void SetBusPaused(const std::string& name, bool pause);

protected:
	friend class SoundEvent;  // make SoundEvent a friend of AudioSystem, so only SoundEvent has access to GetEventInstance() in AudioSystem
	
	FMOD::Studio::EventInstance* AudioSystem::GetEventInstance(unsigned int id);

private:
	// tracks the next ID to use for event instances
	static unsigned int sNextID;

	class Game* mGame;

	// FMOD studio system
	FMOD::Studio::System* mSystem;
	// FMOD Low-level system
	FMOD::System* mLowLevelSystem;

	// map of loaded banks
	std::unordered_map<std::string, FMOD::Studio::Bank*> mBanks;
	// map of event name to EventDescription
	std::unordered_map<std::string, FMOD::Studio::EventDescription*> mEvents;
	// map of event instance ID to EventInstance
	std::unordered_map<unsigned int, FMOD::Studio::EventInstance*> mEventInstances;
	// map of buses
	std::unordered_map<std::string, FMOD::Studio::Bus*> mBuses;
};
