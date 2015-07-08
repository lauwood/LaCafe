#include "GameObjectChair.h"

GameObjectChair::GameObjectChair(int z, int x) : GameObject(z, x) {
}

GameObjectChair::~GameObjectChair() {

}

void GameObjectChair::Initialize(Mesh *ChairModel) {
	//model = Mesh("Models/chair/chair.obj", "Shaders/Banana_vs.glsl", "Shaders/Banana_fs.glsl");
	model = ChairModel;
}

void GameObjectChair::Render() {
	model->SetPosition(vec3(z, 0.5, x));
	model->SetScale(vec3(0.5, 0.5, 0.5));
	model->Render();
}