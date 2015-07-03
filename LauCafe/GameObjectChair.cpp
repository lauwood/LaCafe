#include "GameObjectChair.h"

GameObjectChair::GameObjectChair() {
}

GameObjectChair::~GameObjectChair() {

}

void GameObjectChair::Initialize() {
	model = Mesh("Models/chair/chair.obj", "Shaders/Banana_vs.glsl", "Shaders/Banana_fs.glsl"); 
	model.SetPosition(vec3(0, 0.5, 0));
	model.SetScale(vec3(0.5, 0.5, 0.5));
}

void GameObjectChair::Render() {
	model.Render();
}