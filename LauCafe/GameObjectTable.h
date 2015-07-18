#ifndef GO_TABLE_H
#define GO_TABLE_H

#include "GameObject.h"
#include "Model.h"
#include "MeshLoader.h"

class GameObjectTable : public GameObject {
public:
	GameObjectTable()												{}
	GameObjectTable(int z, int x, Area* a);
	~GameObjectTable()												{}

	void Render();

private:
	Model model;
	Model leg1;
	Model leg2;
	Model leg3;
	Model leg4;
};

#endif