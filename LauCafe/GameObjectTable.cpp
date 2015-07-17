#include "GameObjectTable.h"

#define Y_OFFSET 0.75

void GameObjectTable::Initialize(Mesh *TableModel) {
	model.Initialize(Model::square2, 6, GL_TRIANGLES, "Shaders/Banana_vs.glsl", "Shaders/Banana_fs.glsl");
	leg1.Initialize(Model::square, 6, GL_TRIANGLES, "Shaders/Banana_vs.glsl", "Shaders/Banana_fs.glsl");
	leg2.Initialize(Model::square, 6, GL_TRIANGLES, "Shaders/Banana_vs.glsl", "Shaders/Banana_fs.glsl");
	leg3.Initialize(Model::square, 6, GL_TRIANGLES, "Shaders/Banana_vs.glsl", "Shaders/Banana_fs.glsl");
	leg4.Initialize(Model::square, 6, GL_TRIANGLES, "Shaders/Banana_vs.glsl", "Shaders/Banana_fs.glsl");
	model.SetPosition(vec3(z, 1, x));
	leg1.SetPosition(vec3(z-0.4, Y_OFFSET, x-0.4));
	leg2.SetPosition(vec3(z-0.4, Y_OFFSET, x+0.4));
	leg3.SetPosition(vec3(z+0.4, Y_OFFSET, x-0.4));
	leg4.SetPosition(vec3(z+0.4, Y_OFFSET, x+0.4));

	model.SetScale(vec3(0.5, 0.5, 0.5));
	leg1.SetScale(vec3(0.1, 0.5, 0.1));
	leg2.SetScale(vec3(0.1, 0.5, 0.1));
	leg3.SetScale(vec3(0.1, 0.5, 0.1));
	leg4.SetScale(vec3(0.1, 0.5, 0.1));
}

void GameObjectTable::Render() {
	model.Render();

	leg1.Render();
	leg2.Render();
	leg3.Render();
	leg4.Render();
}