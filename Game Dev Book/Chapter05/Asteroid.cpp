#include "Asteroid.hpp"
#include "Game.hpp"
#include "SpriteComponent.hpp"
#include "MoveComponent.hpp"
#include "CircleComponent.hpp"
#include "Texture.hpp"
#include "Random.hpp"

Asteroid::Asteroid(Game* game) : Actor(game) {
	// initialize to random position/orientation
	Vector2 randPos = Random::GetVector(Vector2::Zero, Vector2(1024.0f, 768.0f));
	SetPosition(randPos);
	SetRotation(Random::GetFloatRange(0.0f, Math::TwoPi));

	// create a sprite component, and set texture
	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(game->GetTexture("Assets/Asteroid.png"));
	
	// create a move component, and set a forward speed (150 pixels/second)
	MoveComponent* mc = new MoveComponent(this);
	mc->SetForwardSpeed(150.0f);

	// create a circle component (for collision), and set a radius
	mCircle = new CircleComponent(this);
	mCircle->SetRadius(40.0f);

	// add to mAsteroids in game
	game->AddAsteroid(this);
}

Asteroid::~Asteroid() {
	GetGame()->RemoveAsteroid(this);
	Actor::~Actor();
}

CircleComponent* Asteroid::GetCircle() const {
	return mCircle;
}