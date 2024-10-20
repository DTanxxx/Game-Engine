#include "Dummy.hpp"
#include "AnimSpriteComponent.hpp"
#include "Game.hpp"

Dummy::Dummy(Game* game) : Actor(game) {
	mRightSpeed = 0.0f;
	mDownSpeed = 0.0f;

	// create an animated sprite component
	AnimSpriteComponent* asc = new AnimSpriteComponent(this);

	std::vector<SDL_Texture*> allTextures;
	for (int i = 0; i < 3; ++i) {
		std::vector<SDL_Texture*> tempAnims;
		switch (i) {
		case 0:
			tempAnims = {
				game->GetTexture("Assets/Character01.png"),
				game->GetTexture("Assets/Character02.png"),
				game->GetTexture("Assets/Character03.png"),
				game->GetTexture("Assets/Character04.png"),
				game->GetTexture("Assets/Character05.png"),
				game->GetTexture("Assets/Character06.png")
			};
			asc->AppendAnimation(0, 5, true);
			break;
		case 1:
			tempAnims = {
				game->GetTexture("Assets/Character06.png"),
				game->GetTexture("Assets/Character07.png"),
				game->GetTexture("Assets/Character08.png"),
				game->GetTexture("Assets/Character09.png"),
				game->GetTexture("Assets/Character10.png"),
				game->GetTexture("Assets/Character11.png"),
				game->GetTexture("Assets/Character12.png"),
				game->GetTexture("Assets/Character13.png"),
				game->GetTexture("Assets/Character14.png"),
				game->GetTexture("Assets/Character15.png")
			};
			asc->AppendAnimation(6, 15, true);
			break;
		case 2:
			tempAnims = {
				game->GetTexture("Assets/Character16.png"),
				game->GetTexture("Assets/Character17.png"),
				game->GetTexture("Assets/Character18.png")
			};
			asc->AppendAnimation(16, 18, true);
			break;
		}
		allTextures.insert(allTextures.end(), tempAnims.begin(), tempAnims.end());
	}
	asc->SetAnimTextures(allTextures);
	asc->SetAnimation(1);
}

void Dummy::ProcessKeyboard(const uint8_t* state) {
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

void Dummy::UpdateActor(float deltaTime) {
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