#include "GameObject.h"
#include "Area.h"

GameObject::GameObject(int z, int x, const char* m, Area* a) {
	// For deletion and insertion into the Mesh unordered map
	this->a = a;
	this->modelPath = m;

	model = a->getMesh(modelPath);
	
	model->SetPosition(vec3(z, 0.5, x));
	model->SetScale(vec3(0.5, 0.5, 0.5));
}

GameObject::~GameObject() {
	a->decrementMesh(modelPath);
}

void GameObject::Render() {
	model->Render();
}