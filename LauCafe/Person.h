#pragma once
#include "Model.h"
#include "Area.h"

class Person
{
public:
	Person() {}
	Person(Area* area);
	~Person();

	virtual void findNextDestination() = 0;
	virtual void finishCurrentTask() = 0;
	void startWalking();
	void finishWalking();
	void move();
	void decrementTimer(int decrementValue = 100);

protected:
	bool m_isBusy;		// Cooking, cleaning, eating, toilet, etc.
	bool m_isWaiting;	// For a seat, for food to cook, for food to arrive
	bool m_isWalking;
	int m_time;			// Time left for current task

	Area* p_Area;			// For pathfinding
	deque<Cell*> path;	// Path to next destination
	Cell m_currentPosition;
	Cell m_destination;
};

