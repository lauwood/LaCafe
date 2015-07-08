#pragma once
#include "Area.h"
#include "MeshLoader.h"
#include <stdlib.h>
#include <time.h>

#define MAX_TIME 2000
#define MIN_TIME 1000

enum Direction { LEFT, RIGHT, UP, DOWN, STAY };

class Person
{
public:
	Person() {}
	Person(Area* area);
	~Person();

	// Virtual functions for all people
	virtual void findNextDestination() = 0;
	virtual void actOrWait() = 0;
	virtual void arrive() = 0;
	virtual void update() = 0;

	Direction getDirection(Cell* c1, Cell* c2);

	void setTimer();
	void Render() { m_mesh.Render(); }

protected:
	Mesh m_mesh;			// Holds model information for the person
	bool m_isIdle;
	bool m_isBusy;			// Cooking, cleaning, eating, toilet, etc.
	bool m_isWaiting;		// For a seat, for food to cook, for food to arrive
	bool m_isWalking;
	int m_pathIndex;
	int m_time;				// Time left for current task
	Direction m_direction;	// Used to determine which way the model moves every tick
	float m_distance = 0;	// 0 - 1, used to determine when direction has to be recalculated

	Area* m_area;							// For pathfinding
	deque<Cell*> m_pathToNextDestination;	// Path to next destination
	Cell m_currentPosition;
	Cell m_destination;

};

