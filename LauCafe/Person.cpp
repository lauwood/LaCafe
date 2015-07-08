#include "Person.h"
#include "TimeManager.h"

Person::Person(Area* area)
{
	m_isBusy = false;
	m_time = 0;
	m_area = area;
}


Person::~Person()
{
	// DO NOT DELETE AREA
}

Direction Person::getDirection(Cell* c1, Cell *c2) {
	if (c1->x < c2->x) {
		return RIGHT;
	}
	else if (c1->z < c2->z) {
		return UP;
	}
	else if (c1-> x > c2->x) {
		return LEFT;
	}
	else {
		return DOWN;
	}
}

// Set the timer for the current activity
void Person::setTimer() {
	srand(time(NULL));
	m_time = (rand() % (MAX_TIME - MIN_TIME + 1)) + MIN_TIME;
}