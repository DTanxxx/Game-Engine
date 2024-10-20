#include "Bullet.hpp"
#include "SpriteComponent.hpp"
#include "MoveComponent.hpp"
#include "CircleComponent.hpp"
#include "Game.hpp"
#include "Enemy.hpp"

Bullet::Bullet(Game* game) : Actor(game) {
	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(game->GetTexture("Assets/Projectile.png"));

	MoveComponent* mc = new MoveComponent(this);
	mc->SetForwardSpeed(400.0f);

	mCircle = new CircleComponent(this);
	mCircle->SetRadius(5.0f);

	mLiveTime = 1.0f;
}

void Bullet::UpdateActor(float deltaTime) {
	Actor::UpdateActor(deltaTime);

	// check for collision vs enemies
	for (Enemy* e : GetGame()->GetEnemies()) {
		if (Intersect(*mCircle, *(e->GetCircle()))) {
			// we both die on collision
			e->SetState(EDead);
			SetState(EDead);
			break;
		}
	}

	mLiveTime -= deltaTime;
	if (mLiveTime <= 0.0f) {
		// time limit hit, die
		SetState(EDead);
	}
}