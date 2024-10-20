#pragma once

#include "Actor.hpp"

class Laser : public Actor
{
public:
	Laser(class Game* game, float rotation);

	void UpdateActor(float deltaTime) override;

private:
	class CircleComponent* mCircle;
	float mAliveTime;
};