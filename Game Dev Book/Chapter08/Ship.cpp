#include "Ship.hpp"
#include "Game.hpp"
#include "SpriteComponent.hpp"
#include "InputSystem.hpp"
#include "Texture.hpp"
#include "Laser.hpp"

Ship::Ship(Game* game, int controllerIndex) : Actor(game) {
	// create a sprite component
	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(game->GetTexture("Assets/Ship.png"));
	
	mSpeed = 400.0f;
	mLaserCooldown = 0.0f;
	mControllerIndex = controllerIndex;
}

void Ship::ActorInput(const InputState& state) {
	for (int i = 0; i < state.Controllers.size(); ++i) {
		ControllerState controllerState = state.Controllers[i];
		if (controllerState.GetControllerIndex() == mControllerIndex && controllerState.GetIsConnected()) {
			// found the controller state
			if (controllerState.GetRightTrigger() > 0.25f &&
				mLaserCooldown <= 0.0f) {
				// create a laser and set its position/rotation to mine
				Laser* laser = new Laser(GetGame());
				laser->SetPosition(GetPosition());
				laser->SetRotation(GetRotation());

				// reset laser cooldown 
				mLaserCooldown = 0.25f;
			}

			mVelocityDir = controllerState.GetLeftStick();
			if (!Math::NearZero(controllerState.GetRightStick().Length())) {
				mRotationDir = controllerState.GetRightStick();
			}

			return;
		}
	}
}

void Ship::UpdateActor(float deltaTime) {
	mLaserCooldown -= deltaTime;

	// update position based on velocity
	Vector2 pos = GetPosition();
	pos += mVelocityDir * mSpeed * deltaTime;
	SetPosition(pos);

	// update rotation
	float angle = Math::Atan2(mRotationDir.y, mRotationDir.x);
	SetRotation(angle);
}
