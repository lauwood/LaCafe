#ifndef GAMEOBJ_H
#define GAMEOBJ_H

#include "Main.h"
#include "MeshLoader.h"

using namespace glm;

class Area;

class GameObject {
public:
	GameObject() {}
	GameObject(int z, int x, const char* m, Area* a);
	~GameObject();

	void Render();

	vec3 GetPosition() { return position; }
	void SetPosition(vec3 pPosition) { position = pPosition; }

	vec3 GetRotation() { return rotation; }
	void SetRotation(vec3 pRotation) { rotation = pRotation; }

	vec3 GetScale() { return scale; }
	void SetScale(vec3 pScale) { scale = pScale; }

protected:
	vec3 position;
	vec3 rotation;
	vec3 scale;

	Area* a;
	Mesh* model;
	const char* modelPath;	// Should change to an int when we change to itemIds

	int x, z;	// Legacy, remove once legacy code is updated
};

#endif