#pragma once
#include "Model.h"
#include "Area.h"
#include <stdlib.h>
#include <time.h>

#define MIN_TIME 5000
#define MAX_TIME 10000

enum Direction {LEFT, RIGHT, UP, DOWN};

class Person
{
public:
	Person() {}
	Person(Area* area);
	~Person();

	virtual void findNextDestination() = 0;
	virtual void finishCurrentTask() = 0;
	virtual void arrive() = 0;
	virtual void update() = 0;
	Direction getDirection(Cell* c1, Cell* c2);
	void setWalking();
	void finishWalking();
	void move();
	void decrementTimer();
	void setTimer();

protected:
	bool m_isBusy;		// Cooking, cleaning, eating, toilet, etc.
	bool m_isWaiting;	// For a seat, for food to cook, for food to arrive
	bool m_isWalking;
	int m_time;			// Time left for current task

	Area* p_Area;			// For pathfinding
	deque<Cell*> path;	// Path to next destination
	Cell m_currentPosition;
	Cell m_destination;

	int m_direction;
	float m_distance = 0;
};

