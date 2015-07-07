#ifndef GO_TABLE_H
#define GO_TABLE_H

#include "GameObject.h"
#include "Model.h"

class GameObjectTable : public GameObject {
public:
	GameObjectTable() {}
	GameObjectTable(int z, int x);
	~GameObjectTable();

	void Initialize();
	void Render();

private:
	Model model;
	Model leg1;
	Model leg2;
	Model leg3;
	Model leg4;
};

#endif