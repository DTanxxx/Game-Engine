#pragma once

#include "Actor.hpp"

class Tower : public Actor {
public:
	Tower(class Game* game);

private:
	class MoveComponent* mMove;
	class AIComponent* mAI;
};