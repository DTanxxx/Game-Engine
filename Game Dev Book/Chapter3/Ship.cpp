#include "Ship.hpp"
#include "Game.hpp"
#include "SpriteComponent.hpp"
#include "InputComponent.hpp"
#include "CircleComponent.hpp"
#include "Asteroid.hpp"
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

	// Exercise 3.3
	mInputComp->SetMass(1.0f);
	mInputComp->SetMaxForce(100.0f);

	// create a circle component (for collision)
	mCircleComp = new CircleComponent(this);
	mCircleComp->SetRadius(40.0f);

	mLaserCooldown = 0.0f;
	mResetTimer = 2.0f;
	mDead = false;
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
	if (mResetTimer <= 0.0f) {
		OnRevive();
	}
	else if (mDead) {
		mResetTimer -= deltaTime;
		return;
	}

	mLaserCooldown -= deltaTime;

	// check if ship has a forward speed
	//if (!Math::NearZero(mInputComp->GetForwardSpeed())) {
	//	// change texture if so
	//	mSpriteComp->SetTexture(GetGame()->GetTexture("Assets/ShipWithThrust.png"));
	//}
	//else {
	//	// use original idle texture
	//	mSpriteComp->SetTexture(GetGame()->GetTexture("Assets/Ship.png"));
	//}

	// Exercise 3.3
	if (!Math::NearZero(mInputComp->GetVelocity().LengthSq())) {
		mSpriteComp->SetTexture(GetGame()->GetTexture("Assets/ShipWithThrust.png"));
	}
	else {
		mSpriteComp->SetTexture(GetGame()->GetTexture("Assets/Ship.png"));
	}

	// check for ship collision against asteroids
	for (auto ast : GetGame()->GetAsteroids()) {
		if (Intersect(*mCircleComp, *(ast->GetCircle()))) {
			OnDeath();
			return;
		}
	}
}

// Exercise 3.2
void Ship::OnDeath() {
	mDead = true;
	mSpriteComp->SetTexture(nullptr);
}

// Exercise 3.2
void Ship::OnRevive() {
	mResetTimer = 2.0f;
	mLaserCooldown = 0.0f;
	SetPosition(Vector2(512.0f, 384.0f));
	SetRotation(Math::PiOver2);
	mInputComp->SetVelocity(Vector2::Zero);
	mInputComp->SetForce(Vector2::Zero);
	mSpriteComp->SetTexture(GetGame()->GetTexture("Assets/Ship.png"));
	mDead = false;
}