#pragma once

#include <string>
#include "Math.hpp"

class SoundEvent {
public:
	SoundEvent();
	~SoundEvent();

	// returns true if associated FMOD event still exists
	bool IsValid();
	// restart event from beginning
	void Restart();
	// stop this event
	void Stop(bool allowFadeOut = true);

	// setters
	void SetPaused(bool pause);
	void SetVolume(float value);
	void SetPitch(float value);
	void SetParameter(const std::string& name, float value);

	// getters
	bool GetPaused() const;
	float GetVolume() const;
	float GetPitch() const;
	float GetParameter(const std::string& name);

	// positional
	bool Is3D() const;
	void Set3DAttributes(const Matrix4& worldTrans);

protected:
	// make this constructor protected and AudioSystem a friend so that only
	// AudioSystem can access this constructor
	friend class AudioSystem;

	SoundEvent(class AudioSystem* system, unsigned int id);

private:
	class AudioSystem* mSystem;
	unsigned int mID;
};