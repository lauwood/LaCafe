#include "GameObject.h"
#include "Area.h"

GameObject::GameObject(int z, int x, const char* m, Area* a) {
	// For deletion and insertion into the Mesh unordered map
	this->a = a;
	this->modelPath = m;

	model = *(a->getMesh(modelPath));
	
	model.SetPosition(vec3(z, 0.5, x));

	// Hopefully have a uniform scale with updated models
	if (m == "Models/chair/chair.obj")
		model.SetScale(vec3(0.5, 0.5, 0.5));
	else if (m == "Models/stove.fbx")
		model.SetScale(vec3(0.01, 0.01, 0.01));
	else if (m == "Models/Podium.fbx")
		model.SetScale(vec3(0.9, 0.9, 0.9));
}

GameObject::~GameObject() {
	a->decrementMesh(modelPath);
}

void GameObject::Render() {
	model.Render();
}