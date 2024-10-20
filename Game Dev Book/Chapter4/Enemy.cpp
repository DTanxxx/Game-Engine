#include "Enemy.hpp"
#include "Game.hpp"
#include "SpriteComponent.hpp"
#include "NavComponent.hpp"
#include "Grid.hpp"
#include "Tile.hpp"
#include "CircleComponent.hpp"
#include "AIComponent.hpp"
#include "AIEnemyMoveState.hpp"
#include "AIEnemyDeathState.hpp"
#include <algorithm>

Enemy::Enemy(class Game* game) : Actor(game) {
	// add to enemy vector
	game->GetEnemies().emplace_back(this);

	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(game->GetTexture("Assets/Airplane.png"));

	// set position at start tile
	SetPosition(GetGame()->GetGrid()->GetStartTile()->GetPosition());

	// set up a nav component at the start tile
	NavComponent* nc = new NavComponent(this);
	nc->SetForwardSpeed(150.0f);
	nc->StartPath(GetGame()->GetGrid()->GetStartTile());

	// set up a circle for collision
	mCircle = new CircleComponent(this);
	mCircle->SetRadius(25.0f);

	// Exercise 4.1
	mAI = new AIComponent(this);
	mAI->RegisterState(new AIEnemyMoveState(mAI));
	mAI->RegisterState(new AIEnemyDeathState(mAI));
	mAI->ChangeState("Move");
}

Enemy::~Enemy() {
	// remove from enemy vector
	auto iter = std::find(GetGame()->GetEnemies().begin(), GetGame()->GetEnemies().end(), this);
	GetGame()->GetEnemies().erase(iter);
	Actor::~Actor();
}
