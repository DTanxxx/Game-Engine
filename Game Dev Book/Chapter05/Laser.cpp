#include "Laser.hpp"
#include "Game.hpp"
#include "SpriteComponent.hpp"
#include "MoveComponent.hpp"
#include "CircleComponent.hpp"
#include "Texture.hpp"
#include "Asteroid.hpp"

Laser::Laser(Game* game, float rotation) : Actor(game) {
	// create a sprite component
	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(game->GetTexture("Assets/Laser.png"));

	// create a circle component (for collision)
	mCircle = new CircleComponent(this);
	mCircle->SetRadius(11.0f);

	SetRotation(rotation);

	// create a move component, and set a forward speed
	MoveComponent* mc = new MoveComponent(this);
	mc->SetAngularSpeed(0.0f);
	mc->SetForwardSpeed(800.0f);
	
	mAliveTime = 1.0f;
}

void Laser::UpdateActor(float deltaTime) {
	// if we run out of time, laser is dead
	mAliveTime -= deltaTime;
	if (mAliveTime <= 0.0f) {
		// remove this laser
		SetState(EDead);
		return;
	}

	// test for intersection against asteroids
	// do we intersect with an asteroid?
	for (auto ast : GetGame()->GetAsteroids()) {
		if (Intersect(*mCircle, *(ast->GetCircle()))) {
			// if this laser intersects with an asteroid, set ourselves and the asteroid to dead
			SetState(EDead);
			ast->SetState(EDead);
			return;
		}
	}
}