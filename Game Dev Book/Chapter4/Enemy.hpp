#pragma once

#include "Actor.hpp"

class Enemy : public Actor {
public:
	Enemy(class Game* game);
	~Enemy();

	class CircleComponent* GetCircle() {
		return mCircle;
	}

private:
	class CircleComponent* mCircle;
	class AIComponent* mAI;
};