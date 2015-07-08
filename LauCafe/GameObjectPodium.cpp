#include "GameObjectPodium.h"

GameObjectPodium::GameObjectPodium(int z, int x) : GameObject(z, x) {
}

GameObjectPodium::~GameObjectPodium() {

}

void GameObjectPodium::Initialize(Mesh *PodiumModel) {
	//model = Mesh("Models/Podium.fbx", "Shaders/Banana_vs.glsl", "Shaders/Banana_fs.glsl");
	model = PodiumModel;
}

void GameObjectPodium::Render() {
	model->SetPosition(vec3(z, 0, x));
	model->SetScale(vec3(0.9, 0.9, 0.9));
	model->Render();
}