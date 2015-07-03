#ifndef GO_CHAIR_H
#define GO_CHAIR_H

#include "GameObject.h"
#include "MeshLoader.h"

class GameObjectChair : public GameObject {
public:
	GameObjectChair();
	~GameObjectChair();

	void Initialize();
	void Render();

private:
	Mesh model;
};

#endif