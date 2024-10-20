#include "Ship.hpp"
#include "AnimSpriteComponent.hpp"
#include "Game.hpp"

Ship::Ship(Game* game) : Actor(game) {
	mRightSpeed = 0.0f;
	mDownSpeed = 0.0f;

	// create an animated sprite component
	AnimSpriteComponent* asc = new AnimSpriteComponent(this);

	std::vector<SDL_Texture*> allTextures = {
		game->GetTexture("Assets/Ship01.png"),
		game->GetTexture("Assets/Ship02.png"),
		game->GetTexture("Assets/Ship03.png"),
		game->GetTexture("Assets/Ship04.png")
	};
	asc->AppendAnimation(0, 3, true);
	asc->SetAnimTextures(allTextures);
	asc->SetAnimation(0);
}

void Ship::ProcessKeyboard(const uint8_t* state) {
	mRightSpeed = 0.0f;
	mDownSpeed = 0.0f;

	// up/down input
	if (state[SDL_SCANCODE_W]) {
		mDownSpeed -= 300.0f;
	}

	if (state[SDL_SCANCODE_S]) {
		mDownSpeed += 300.0f;
	}

	// right/left input
	if (state[SDL_SCANCODE_A]) {
		mRightSpeed -= 250.0f;
	}

	if (state[SDL_SCANCODE_D]) {
		mRightSpeed += 250.0f;
	}
}

void Ship::UpdateActor(float deltaTime) {
	Actor::UpdateActor(deltaTime);

	// update position based on speeds and delta time
	Vector2 pos = GetPosition();
	pos.x += mRightSpeed * deltaTime;
	pos.y += mDownSpeed * deltaTime;

	// restrict position to left half of screen
	if (pos.y < 25.0f) {
		pos.y = 25.0f;
	}
	else if (pos.y > 768.0f - 25.0f) {
		pos.y = 768.0f - 25.0f;
	}

	if (pos.x > 500.0f) {
		pos.x = 500.0f;
	}
	else if (pos.x < 25.0f) {
		pos.x = 25.0f;
	}

	SetPosition(pos);
}