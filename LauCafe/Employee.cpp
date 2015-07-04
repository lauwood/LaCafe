#include "Employee.h"
#include "TimeManager.h"
#include <stdlib.h>
#include <time.h>

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
		m_destination = m_area->getAdjacentStove(m_currentPosition.z, m_currentPosition.x);
	}
	
	bool foundDestination = false;

	if (m_role == WAITER) {
		if (m_isIdle) {
			if (m_area->v_doneCookingStoveCells.size() > 0) {
				m_destination = m_area->v_doneCookingStoveCells.back();
				foundDestination = true;
				m_area->v_doneCookingStoveCells.pop();
			}
		}
		else if (m_carryingFood) {
			if (m_area->v_waitingCustomerCells.size() > 0) {
				m_destination = m_area->v_waitingCustomerCells.back();
				foundDestination = true;
				m_area->v_waitingCustomerCells.pop();
			}
		}
	}
	else if(m_role == DISHWASHER) {
		if (m_area->v_dirtyTableCells.size() > 0) {
			m_destination = m_area->v_dirtyTableCells.back();
			foundDestination = true;
			m_area->v_dirtyTableCells.pop();
		}
	}

	if (!foundDestination) {
		srand(time(NULL));

		// There's nowhere to go and nothing to do, wander around
		int x = -1;
		int z = -1;

		while (m_area->isWalkable(z, x)) {
			x = rand() % m_area->getWidth();
			z = rand() % m_area->getHeight();
		}

		m_destination.x = x;
		m_destination.z = z;
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

			// Waiters don't need to act
			if (m_role == DISHWASHER) {
				m_area->setTileStatus(m_destination.z, m_destination.x, OPEN);
				// Prep for the next tick
				findNextDestination();
				setWalking();
			}
			else if (m_role == COOK || m_role == BARISTA) {
				m_area->setTileStatus(m_destination.z, m_destination.x, FOOD_READY);
				Cell stoveCell;
				stoveCell.z = m_destination.z;
				stoveCell.x = m_destination.x;
				m_area->v_doneCookingStoveCells.push(stoveCell);
			}
		}
}

void Employee::arrive() {
	switch(m_role) {
	case WAITER:
		if (!m_carryingFood) {
			if (m_area->getTileStatus(m_destination.z, m_destination.x) == FOOD_READY) {
				m_area->setTileStatus(m_destination.z, m_destination.x, OPEN);
				m_carryingFood = true;
				findNextDestination();
				m_area->setTileStatus(m_destination.z, m_destination.x, FOOD_COMING);
			}
		}
		else {
			if (m_area->getTileStatus(m_destination.z, m_destination.x) == FOOD_COMING) {
				m_area->setTileStatus(m_destination.z, m_destination.x, EATING);
				m_isIdle = true;
				findNextDestination();
			}
		}
		
		break;
	case DISHWASHER:
		if (m_area->getTileStatus(m_destination.z + 1, m_destination.x) == DIRTY) {
			m_area->setTileStatus(m_destination.z + 1, m_destination.x, CLEANING);
			m_isIdle = true;
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
