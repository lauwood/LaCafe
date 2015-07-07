#include "GameObjectPodium.h"

GameObjectPodium::GameObjectPodium(int z, int x) : GameObject(z, x) {
}

GameObjectPodium::~GameObjectPodium() {

}

void GameObjectPodium::Initialize() {
	model = Mesh("Models/Podium.fbx", "Shaders/Banana_vs.glsl", "Shaders/Banana_fs.glsl");
	model.SetPosition(vec3(z, 0, x));
	model.SetScale(vec3(0.9, 0.9, 0.9));
}

void GameObjectPodium::Render() {
	model.Render();
}