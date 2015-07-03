#ifndef GO_STOVE_H
#define GO_STOVE_H

#include "GameObject.h"
#include "MeshLoader.h"

class GameObjectStove : public GameObject {
public:
	GameObjectStove();
	~GameObjectStove();

	void Initialize();
	void Render();

private:
	Mesh model;
};

#endif