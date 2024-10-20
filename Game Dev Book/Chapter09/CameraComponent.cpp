#include "CameraComponent.hpp"
#include "Game.hpp"
#include "Renderer.hpp"
#include "AudioSystem.hpp"
#include "Actor.hpp"

CameraComponent::CameraComponent(Actor* owner, int updateOrder) : 
	Component(owner, updateOrder) {
}

CameraComponent::~CameraComponent() {
	Component::~Component();
}

void CameraComponent::SetViewMatrix(const Matrix4& view) {
	// pass view matrix to renderer and audio system
	Game* game = mOwner->GetGame();
	game->GetRenderer()->SetViewMatrix(view);
	game->GetAudioSystem()->SetListener(view);
}
