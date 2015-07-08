#include "GameObjectStove.h"

GameObjectStove::GameObjectStove(int z, int x) : GameObject(z, x) {
}

GameObjectStove::~GameObjectStove() {

}

void GameObjectStove::Initialize(Mesh *StoveModel) {
	//model = Mesh("Models/stove.fbx", "Shaders/Banana_vs.glsl", "Shaders/Banana_fs.glsl");
	model = StoveModel;
}

void GameObjectStove::Render() {
	model->SetPosition(vec3(z, 0.5, x));
	model->SetScale(vec3(0.01, 0.01, 0.01));
	model->Render();
}