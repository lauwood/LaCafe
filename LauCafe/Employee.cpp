#include "Employee.h"
#include "TimeManager.h"
#include <stdlib.h>
#include <time.h>

Employee::Employee(Area* area) : Person(area)
{
	m_isIdle = true;
	m_carryingFood = false;
	m_isWalking = false;
}


Employee::~Employee()
{
}

void Employee::findNextDestination() {
	// These roles do not move
	if (m_role == COOK || m_role == RECEPTIONIST || m_role == BARISTA) {
		return;
	}
	else {
		if (m_role == WAITER) {
			deque<Cell> stoveCells = m_area->v_doneCookingStoveCells;
			deque<Cell> tableCells = m_area->v_dirtyTableCells;

			if (m_isIdle) {
				// Loop through the vector for a reachable stove
				for (int i = 0; i < stoveCells.size() && m_isIdle; i++) {
					Cell destination(stoveCells.at(i));
					if (m_area->getCellPathLength(
						m_currentPosition.z, m_currentPosition.x, destination.z, destination.x)) {
						m_isBusy = true;
						m_isIdle = false;
						m_isWalking = true;
						stoveCells.erase(stoveCells.begin() + i);
						m_destination = destination;
					}
				}
			}
			else if (m_carryingFood) {
				for (int i = 0; i < tableCells.size(); i++) {
					Cell destination(tableCells.at(i));
					if (m_area->getCellPathLength(
						m_currentPosition.z, m_currentPosition.x, destination.z, destination.x)) {
						m_isWalking = true;
						tableCells.erase(tableCells.begin() + i);
						m_destination = destination;
						break;
					}
				}
			}
		}
		else if (m_role == DISHWASHER) {
			if (m_isIdle) {
				deque<Cell> dirtyCells = m_area->v_dirtyTableCells;
				for (int i = 0; i < dirtyCells.size(); i++) {
					Cell destination(dirtyCells.at(i));
					if (m_area->getCellPathLength(
						m_currentPosition.z, m_currentPosition.x, destination.z, destination.x)) {
						dirtyCells.erase(dirtyCells.begin() + i);
						m_destination = destination;
						m_isIdle = false;
						m_isBusy = true;
						m_isWalking = true;
						break;
					}
				}
			}
		}

		if (m_isIdle) {
			srand(time(NULL));
			
			// Need a better algorithm, this doesn't "scale" with lag
			// Have a chance to idle
			if (rand() % 1000 < 950) {
				// There's nowhere to go and nothing to do, wander around
				int x = -1;
				int z = -1;

				while (m_area->isWalkable(z, x)) {
					x = rand() % m_area->getWidth();
					z = rand() % m_area->getHeight();
				}

				m_destination.x = x;
				m_destination.z = z;

				m_isWalking = true;
			}
			else m_isWalking = false;
		}
	}
}

void Employee::actOrWait() {
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
				m_isBusy = false;
				m_isIdle = true;
				// Prep for the next tick
				findNextDestination();
			}
			else if (m_role == COOK || m_role == BARISTA) {
				m_isBusy = false;
				m_isIdle = true;
				m_area->setTileStatus(m_destination.z, m_destination.x, FOOD_READY);
				Cell stoveCell;
				stoveCell.z = m_destination.z;
				stoveCell.x = m_destination.x;
				m_area->v_doneCookingStoveCells.push_back(stoveCell);
			}
			else {
				// Receptionists only
				m_isBusy = false;
				m_isIdle = true;
				m_area->recStatus = R_READY;
			}
		}
}

void Employee::arrive() {
	m_isWalking = false;

	switch(m_role) {
	case WAITER:
		if (m_isIdle) findNextDestination();
		else if (!m_carryingFood) {
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
	case BARISTA:
		// Do nothing
		break;
	}
}

void Employee::update() {
	/*
	 * Employees should update their status when:
	 *	1. They're idle (obviously), get the next task (if available)
	 *	2. For movers, when they arrive at their random destination
	 *		while wandering while idle
	 */
	if (m_isIdle){
		switch (m_role) {
		case WAITER:
			if (m_area->v_doneCookingStoveCells.size() > 0 || !m_carryingFood)
				findNextDestination();
			break;
		case COOK:
		case BARISTA:
			if (m_area->getWaitingCustomers() > 0 &&
				m_area->getTileStatus(m_destination.z, m_destination.x) == OPEN) {
				m_isIdle = false;
				m_isBusy = true;
				m_area->setTileStatus(m_destination.z, m_destination.x, COOKING);
				m_area->cookForCustomer();
			}
			break;
		case DISHWASHER:
			if (m_area->v_dirtyTableCells.size() > 0 || !m_isIdle)
				findNextDestination();
			break;
		case RECEPTIONIST:
			if (m_area->recStatus == R_JUST_DIRECTED) {
				m_area->recStatus = R_COOLDOWN;
				setTimer();
				m_isIdle = false;
				m_isBusy = true;
			}
		}
	}
	else if (m_isBusy)
		actOrWait();

	return;
}

void Employee::wait() {
	return;
}
