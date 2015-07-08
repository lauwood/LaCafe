#include "Employee.h"
#include "TimeManager.h"
#include <stdlib.h>
#include <time.h>

Employee::Employee(Area* area) : Person(area)
{
	m_isBusy = false;
	m_isWalking = false;

	m_waiterStage = WAITER_IDLE;
	m_dishwasherStage = DISHWASHER_IDLE;
}


Employee::~Employee()
{
}

void Employee::findNextDestination() {
	m_pathIndex = 0;
	if (m_role == COOK || m_role == RECEPTIONIST || m_role == BARISTA) {
		return;
	}
	else if (m_role == WAITER) {
		deque<Cell> destinationCells;
		switch (m_waiterStage) {
		case WAITER_IDLE:
			destinationCells = m_area->v_doneCookingStoveCells;
			break;
		case WAITER_CARRYING_FOOD:
			destinationCells = m_area->v_servingCustomerCells;
			break;
		}

		for (int i = 0; i < destinationCells.size(); i++) {
			Cell destination(destinationCells.at(i));
			if (m_area->getCellPathLength(
				m_currentPosition.z, m_currentPosition.x, destination.z, destination.x) > 0) {
				// Only proceed if the destination is reachable!
				m_isBusy = true;
				m_isWalking = true;
				destinationCells.erase(destinationCells.begin() + i);
				m_destination = destination;

				m_pathToNextDestination = m_area->getCellPath(m_currentPosition.z, m_currentPosition.x,
					m_destination.z, m_destination.x);
				break;
			}
		}
	}
	else if (m_role == DISHWASHER) {
		deque<Cell> dirtyCells = m_area->v_dirtyTableCells;
		for (int i = 0; i < dirtyCells.size(); i++) {
			Cell destination(dirtyCells.at(i));
			if (m_area->getCellPathLength(
				m_currentPosition.z, m_currentPosition.x, destination.z, destination.x) > 0) {
				m_isBusy = true;
				m_isWalking = true;
				dirtyCells.erase(dirtyCells.begin() + i);
				m_destination = destination;

				m_pathToNextDestination = m_area->getCellPath(m_currentPosition.z, m_currentPosition.x,
					m_destination.z, m_destination.x);
				break;
			}
		}
	}

	// If an employee reaches this point, nowhere specific to go so wander around
	if (!m_isBusy && !m_isWalking) {
		srand(time(NULL));
		m_isBusy = false;	// Used when this is called after finishing task

		// Need a better algorithm, this doesn't "scale" with lag
		// Have a chance to idle
		// Move after 5 seconds? 5 sec * 60 fps = 300 frames
		if (rand() % 300 < 1) {
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

void Employee::actOrWait() {
	int decrementValue = TimeManager::Instance().DeltaTime * 1000;

	if (m_time > decrementValue) {
		m_time -= decrementValue;
	}
	else {
		// Person may "do nothing" for a frame
		m_time = 0;
		m_isBusy = false;	// Takes care of cook and barista

		switch (m_role) {
		case DISHWASHER:
			m_area->setTileStatus(m_destination.z, m_destination.x, TILE_OPEN);
			m_dishwasherStage = DISHWASHER_IDLE;
			// Prep for the next tick
			findNextDestination();
			break;
		case RECEPTIONIST:
			m_area->recStatus = REC_READY;
			break;
		case COOK:
		case BARISTA:
			Cell stoveCell(m_destination);
			m_area->setTileStatus(m_destination.z, m_destination.x, TILE_STOVE_FOOD);
			m_area->v_doneCookingStoveCells.push_back(stoveCell);
			break;
		}
	}
}

void Employee::arrive() {
	m_isWalking = false;
	switch (m_role)
	{
	case WAITER:
		findNextDestination();
		break;
	case DISHWASHER:
		switch (m_dishwasherStage)
		{
		case DISHWASHER_IDLE:
		case DISHWASHER_CLEANING:
			findNextDestination();
			break;
		case DISHWASHER_WALKING_TABLE:
			m_dishwasherStage = DISHWASHER_CLEANING;
			break;
		}
		break;
	}
}

void Employee::update() {
	switch (m_role)
	{
	case RECEPTIONIST:
		if (m_isBusy)
			actOrWait();
		else if (m_area->recStatus == REC_JUST_DIRECTED) {
			// Initiate cooldown after directing patron
			m_area->recStatus = REC_COOLDOWN;
			m_isBusy = true;
			setTimer();
		}
		break;
	case COOK:
	case BARISTA:
		if (m_area->getTileStatus(m_destination.z, m_destination.x) == TILE_OPEN) {
			// Stove is open, cook if needed
			for (int i = 0; i < m_area->v_waitingCustomerCells.size(); i++) {
				if (m_area->getCellPathLength(m_destination.z, m_destination.x,
					m_area->v_waitingCustomerCells.at(i).z, m_area->v_waitingCustomerCells.at(i).z) > 0) {
					// The stove can reach the table, so the order is ok
					m_isBusy = true;
					m_area->setTileStatus(m_destination.z, m_destination.x, TILE_STOVE_COOKING);
					break;
				}
			}
		}
		break;
	case WAITER:
		if (m_isBusy) {
			if (m_isWalking) walk();
			else actOrWait();
		}
		else if (m_area->v_doneCookingStoveCells.size() > 0 || !m_isWalking)
			// There's somewhere to go!
			findNextDestination();
		else
			walk();
		break;
	case DISHWASHER:
		if (m_isBusy) {
			if (m_isWalking) walk();
			else actOrWait();
		}
		else if (m_area->v_dirtyTableCells.size() > 0 || !m_isWalking)
			// There's somewhere to go!
			findNextDestination();
		else
			walk();
		break;
	default:
		break;
	}
}

void Employee::setRole(Role role) {
	m_role = role;

	if (role == COOK || role == BARISTA)
		m_destination = m_area->getAdjacentStove(m_currentPosition.z, m_currentPosition.x);

	return;
}