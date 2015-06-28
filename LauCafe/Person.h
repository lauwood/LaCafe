#pragma once
#include "Model.h"
#include "Area.h"

class Person :
	public Model
{
public:
	Person(Area* area);
	~Person();

	virtual void findNextDestination() = 0;
	virtual void finishCurrentTask();
	void startWalking();
	void finishWalking();
	void move();
	void decrementTimer(int decrementValue = 100);

private:
	bool m_isBusy;		// Cooking, cleaning, eating, toilet, etc.
	bool m_isWaiting;	// For a seat, for food to cook, for food to arrive
	bool m_isWalking;
	int m_time;			// Time left for current task

	Area* a;			// For pathfinding
	deque<Cell*> path;	// Path to next destination
	Cell m_currentPosition;
	Cell m_destination;
};

