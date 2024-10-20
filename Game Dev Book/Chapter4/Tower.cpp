#include "Tower.hpp"
#include "SpriteComponent.hpp"
#include "MoveComponent.hpp"
#include "Game.hpp"
#include "AIComponent.hpp"
#include "AITowerRestState.hpp"
#include "AITowerFireState.hpp"

Tower::Tower(Game* game) : Actor(game) {
	SpriteComponent* sc = new SpriteComponent(this, 200);  // draw towers in front of everything
	sc->SetTexture(game->GetTexture("Assets/Tower.png"));

	mMove = new MoveComponent(this);

	// Exercise 4.1
	mAI = new AIComponent(this);
	mAI->RegisterState(new AITowerRestState(mAI));
	mAI->RegisterState(new AITowerFireState(mAI));
	mAI->ChangeState("Rest");
}
