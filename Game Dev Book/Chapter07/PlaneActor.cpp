#include "PlaneActor.hpp"
#include "Game.hpp"
#include "Mesh.hpp"
#include "MeshComponent.hpp"
#include "Renderer.hpp"

PlaneActor::PlaneActor(Game* game) : Actor(game) {
	SetScale(10.0f);
	MeshComponent* mc = new MeshComponent(this, 
		GetGame()->GetRenderer()->GetMesh("Assets/Plane.gpmesh"));
}

PlaneActor::~PlaneActor() {
}