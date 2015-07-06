#include "GameObjectStove.h"

GameObjectStove::GameObjectStove(int z, int x) : GameObject(z, x) {
}

GameObjectStove::~GameObjectStove() {

}

void GameObjectStove::Initialize() {
	model = Mesh("Models/stove.fbx", "Shaders/Banana_vs.glsl", "Shaders/Banana_fs.glsl");
	model.SetPosition(vec3(0, 0.5, 0));
	model.SetScale(vec3(0.01, 0.01, 0.01));
}

void GameObjectStove::Render() {
	model.Render();
}