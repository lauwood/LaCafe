#ifndef GO_CHAIR_H
#define GO_CHAIR_H

#include "GameObject.h"
#include "MeshLoader.h"

class GameObjectChair : public GameObject {
public:
	GameObjectChair()												{}
//	GameObjectChair(int z, int x, Area* a) : GameObject(z, x, a)	{}
	~GameObjectChair()												{}

	void Initialize(Mesh* ChairModel);
	void Render();

private:
	Mesh *model;
};

#endif