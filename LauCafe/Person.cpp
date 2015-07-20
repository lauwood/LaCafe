#include "Person.h"
#include "TimeManager.h"

Person::Person(Restaurant* area)
{
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

// Translate the model across the appropriate axis and speed
// Calls arrival function when path is exhausted
void Person::walk() {
	double delta = TimeManager::Instance().DeltaTime;
	if (m_pathIndex < m_pathToNextDestination.size()) {
		dx = dz = 0;

		m_distance += delta;

		Cell* c = m_pathToNextDestination.at(m_pathIndex);
		// While loop is for in case of major lag and < 1 fps instead of if
		while (m_distance >= 1) {
			m_distance -= 1;
			// Advance a cell
			m_currentPosition = *c;
			if (++m_pathIndex < m_pathToNextDestination.size()) {
				c = m_pathToNextDestination.at(m_pathIndex);
				m_direction = getDirection(&m_currentPosition, c);
			}
			else {
				// Hit the end already
				m_distance = 0;
				m_direction = STAY;
				arrive();
			}
		}

		switch (m_direction) {
		case LEFT:
			dx = -m_distance;
			break;
		case RIGHT:
			dx = m_distance;
			break;
		case UP:
			dz = m_distance;
			break;
		case DOWN:
			dz = -m_distance;
			break;
		case STAY:
			// Don't move if staying right after arriving
			// Patron will "freeze" for a few frames
			break;
		}

	}
}