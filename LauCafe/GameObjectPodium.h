#ifndef GO_PODIUM_H
#define GO_PODIUM_H

#include "GameObject.h"
#include "MeshLoader.h"

class GameObjectPodium : public GameObject {
public:
	GameObjectPodium()		{}
	~GameObjectPodium()		{}

	void Initialize(Mesh *PodiumModel);
	void Render();

private:
	Mesh *model;
};

#endif