#include "Ship.hpp"
#include "Game.hpp"
#include "SpriteComponent.hpp"
#include "InputComponent.hpp"
#include "CircleComponent.hpp"
#include "Asteroid.hpp"
#include "Texture.hpp"
#include "Laser.hpp"

Ship::Ship(Game* game) : Actor(game) {
	// create a sprite component
	mSpriteComp = new SpriteComponent(this);
	mSpriteComp->SetTexture(game->GetTexture("Assets/Ship.png"));

	// create an input component and set keys/speed
	mInputComp = new InputComponent(this);
	mInputComp->SetMaxAngularSpeed(Math::TwoPi);
	mInputComp->SetMaxForwardSpeed(300.0f);
	mInputComp->SetForwardKey(SDL_SCANCODE_W);
	mInputComp->SetBackwardKey(SDL_SCANCODE_S);
	mInputComp->SetClockwiseKey(SDL_SCANCODE_D);
	mInputComp->SetCounterClockwiseKey(SDL_SCANCODE_A);

	// create a circle component (for collision)
	mCircleComp = new CircleComponent(this);
	mCircleComp->SetRadius(40.0f);

	mLaserCooldown = 0.0f;
}

void Ship::ActorInput(const uint8_t* keyState) {
	if (keyState[SDL_SCANCODE_SPACE] && mLaserCooldown <= 0.0f && !mDead) {
		// create a laser and set its position/rotation to mine
		Laser* laser = new Laser(GetGame(), GetRotation());
		laser->SetPosition(GetPosition());

		// reset laser cooldown (half second)
		mLaserCooldown = 0.5f;
	}
}

void Ship::UpdateActor(float deltaTime) {
	mLaserCooldown -= deltaTime;
}
