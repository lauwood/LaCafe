#ifndef FLOOR_H
#define FLOOR_H

#include "Main.h"
#include "Model.h"

class Floor : public Model {
public:
	Floor();
	~Floor() { Destroy(); }

private:
	float x, y;
};

#endif