#include "GameObjectPodium.h"

GameObjectPodium::GameObjectPodium() {
}

GameObjectPodium::~GameObjectPodium() {

}

void GameObjectPodium::Initialize() {
	model = Mesh("Models/Podium.fbx", "Shaders/Banana_vs.glsl", "Shaders/Banana_fs.glsl");
	model.SetPosition(vec3(0, 1, 0));
	model.SetScale(vec3(0.9, 0.9, 0.9));
}

void GameObjectPodium::Render() {
	model.Render();
}