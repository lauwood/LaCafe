#ifndef GAMEOBJ_H
#define GAMEOBJ_H

#include "Main.h"
#include "MeshLoader.h"

using namespace glm;

class GameObject {
public:
	GameObject()				{}
	~GameObject()				{}

	virtual void Initialize(Mesh* model) = 0;
	virtual void Render() = 0;

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

	int z;
	int x;
};

#endif