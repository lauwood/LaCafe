#include "Person.h"


Person::Person(Area* area)
{
	m_isBusy = false;
	m_time = 0;
	p_Area = area;
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

void Person::startWalking()
{
	m_isBusy = false;
	m_isWalking = true;
}
void Person::finishWalking()
{
	m_isWalking = false;
	m_isBusy = true;
}

// Default assume tick is 100 ms (recalculation 10 times per second)
void Person::decrementTimer(int decrementValue)
{
	if (m_isBusy)
		if (m_time > decrementValue) {
			m_time -= decrementValue;
		}
		else {
			// Person may "do nothing" for a few frames
			m_time = 0;
			findNextDestination();
			startWalking();
		}

}