#include "GameObjectChair.h"

GameObjectChair::GameObjectChair(int z, int x) : GameObject(z, x) {
}

GameObjectChair::~GameObjectChair() {

}

void GameObjectChair::Initialize() {
	model = Mesh("Models/chair/chair.obj", "Shaders/Banana_vs.glsl", "Shaders/Banana_fs.glsl"); 
	model.SetPosition(vec3(x, 0.5, z));
	model.SetScale(vec3(0.5, 0.5, 0.5));
}

void GameObjectChair::Render() {
	model.Render();
}