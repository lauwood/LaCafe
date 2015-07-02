#include "Employee.h"
#include "TimeManager.h"

Employee::Employee(Area* area, Camera *m_Camera)
{
	m_isIdle = true;
	m_carryingFood = false;
}


Employee::~Employee()
{
}

void Employee::findNextDestination() {
	// These roles do not move
	if (m_role == COOK || m_role == RECEPTIONIST || m_role == BARISTA) {
		m_destination = m_currentPosition;
	}


}

void Employee::finishCurrentTask() {
	return;
}

void Employee::act() {
	int decrementValue = TimeManager::Instance().DeltaTime * 1000;
	if (m_isBusy)
		if (m_time > decrementValue) {
			m_time -= decrementValue;
		}
		else {
			// Person may "do nothing" for a frame
			m_time = 0;

			m_area->setTileStatus(m_destination.z, m_destination.x, OPEN);

			// Prep for the next tick
			findNextDestination();
			setWalking();
		}
}

void Employee::arrive() {
	switch(m_role) {
	case WAITER:
		if (m_area->getTileStatus(m_destination.z + 1, m_destination.x) == FOOD_READY) {
			m_area->setTileStatus(m_destination.z + 1, m_destination.x, OPEN);
			findNextDestination();
		}
		break;
	case DISHWASHER:
		if (m_area->getTileStatus(m_destination.z + 1, m_destination.x) == DIRTY) {
			m_area->setTileStatus(m_destination.z + 1, m_destination.x, CLEANING);
			findNextDestination();
		}
		break;
	case COOK:
		// Do nothing
		break;
	}
}

void Employee::update() {
	if (m_isIdle){
		findNextDestination();
	}
	return;
}

void Employee::wait() {
	return;
}
