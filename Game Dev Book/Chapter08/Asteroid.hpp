#pragma once

#include "Actor.hpp"

class Asteroid : public Actor {
public:
	Asteroid(class Game* game);
	~Asteroid();

	class CircleComponent* GetCircle() const;

private:
	class CircleComponent* mCircle;
};