#include "Patron.h"
#include "TimeManager.h"

Patron::Patron(Area* area, Mesh *model) : Person(area) {
	Cell entrance = area->getStart();
	m_currentPosition.x = entrance.x;
	m_currentPosition.z = entrance.z;

	// Initialize model and shader
	m_mesh = model;
	m_mesh->SetPosition(vec3(m_currentPosition.z, 0.5, m_currentPosition.x));
	m_mesh->SetScale(vec3(0.5, 0.5, 0.5));

	// Initialize status variables
	m_stage = PATRON_INIT;

	// Immediately find the next destination so it can start walking right away
	findNextDestination();		// Will set the walking status

	// Set initial direction to reduce calls in walk
	Cell* c = m_pathToNextDestination.at(m_pathIndex);
	m_direction = getDirection(&m_currentPosition, c);
}

Patron::~Patron() {
	// Nothing dynamically allocated
}

// Set the path to the next available destination
void Patron::findNextDestination() {
	bool foundDestination = false;
	TileType currentTileType = m_area->getTileType(m_currentPosition.z, m_currentPosition.x);
	TileType destinationType;

	switch (m_stage) {
	case PATRON_INIT :
		destinationType = RECEPTION;
		break;
	case PATRON_WAITING_REC :
		destinationType = TABLE_CHAIR;
		break;
	case PATRON_TIMED_OUT :
		destinationType = START;
		break;
	default:
		// This should not be called anytime
		return;
	}

	// Find the next available destination (not necessarily closest)
	for (int z = 0; z < m_area->getHeight() && !foundDestination; z++)
		for (int x = 0; x < m_area->getWidth() && !foundDestination; x++) {
			if (m_area->getTileType(z, x) == destinationType && m_area->getTileStatus(z, x) == TILE_OPEN) {
				m_destination.x = x;
				m_destination.z = z;
				foundDestination = true;
			}
	}

	if (!foundDestination)
		// Still haven't found a destination, wait for next time
		return;

	// Set the path of the patron to that of the path array
	m_pathToNextDestination = m_area->getCellPath(m_currentPosition.z, m_currentPosition.x,
		m_destination.z, m_destination.x);
	// Reserve the tile so that no two patrons can take the same tile (even when walking to it)
	if (destinationType == TABLE_CHAIR) {
		m_tableCell = m_area->getAdjacentTable(m_destination.z, m_destination.x);
		m_area->setTileStatus(m_destination.z, m_destination.x, TILE_RESERVED);
		m_area->setTileStatus(m_tableCell.z, m_tableCell.x, TILE_RESERVED);
	}

	m_isWalking = true;

	// Since we found a destination, update the stage accordingly
	switch (m_stage) {
	case PATRON_INIT :
		m_stage = PATRON_WALKING_REC;
		break;
	case PATRON_WAITING_REC :
		m_isWaiting = false;
		m_stage = PATRON_WALKING_TABLE;
		break;
	case PATRON_TIMED_OUT :
		m_stage = PATRON_WALKING_EXIT;
		break;
	}

	// Clear variable if path was found before
	m_pathIndex = 0;
}

void Patron::actOrWait() {
	switch (m_stage) {
	case PATRON_WAITING_REC:
		// Prep to go to a table if available
		if (m_area->recStatus == REC_READY) {
			m_area->recStatus = REC_JUST_DIRECTED;
			findNextDestination();
		}
		else
			decrementOrTimeOut();
		break;
	case PATRON_WAITING_SERVE:
		if (m_area->getTileStatus(m_tableCell.z, m_tableCell.x) == TILE_TABLE_FOOD_COMING) {
			m_stage = PATRON_WAITING_FOOD;
		}
		else {
			decrementOrTimeOut();
			// Open up the current tiles for future use if timed out
			if (m_stage == PATRON_WALKING_EXIT) { // findNewDestination sets this in decrementOrTimeout
				m_area->setTileStatus(m_tableCell.z, m_tableCell.x, TILE_OPEN);
				m_area->setTileStatus(m_currentPosition.z, m_currentPosition.x, TILE_OPEN);

				for (unsigned int i = 0; i < m_area->v_waitingCustomerCells.size(); i++) {
					Cell vCell = m_area->v_waitingCustomerCells.at(i);
					if (m_currentPosition.z == vCell.z && m_currentPosition.x == vCell.x) {
						// Take the order out of the deque
						m_area->v_waitingCustomerCells.erase(m_area->v_waitingCustomerCells.begin() + i);
						break;
					}
				}

				findNextDestination();
			}
		}
		break;
	case PATRON_WAITING_FOOD:
		if (m_area->getTileStatus(m_tableCell.z, m_tableCell.x) == TILE_TABLE_FOOD) {
			m_stage = PATRON_EATING_FOOD;
			setTimer();
		}
		break;
	case PATRON_EATING_FOOD:
		decrementOrTimeOut();
		// Time out = done eating
		if (m_stage == PATRON_WALKING_EXIT) {
			m_area->setTileStatus(m_tableCell.z, m_tableCell.x, TILE_TABLE_DIRTY);
			m_area->setTileStatus(m_currentPosition.z, m_currentPosition.x, TILE_TABLE_DIRTY);
			m_area->v_dirtyTableCells.push_back(Cell(m_tableCell));

			//TODO: Rating calculations
			//TODO: May need to change the "stage" to something else
			findNextDestination();
		}
		break;
	}
}

void Patron::arrive() {
	m_isWalking = false;
	Cell tableTile;
	tableTile.x = tableTile.z = -1;

	switch (m_stage) {
	case PATRON_WALKING_REC :
		// Arrived at receptionist, wait for a free receptionist/table
		m_stage = PATRON_WAITING_REC;
		m_isWaiting = true;
		break;
	case PATRON_WALKING_TABLE :
		// If arrived at a table, mark it
		// Don't need to mark the chair, it's "reserved" so nobody will walk to it
		tableTile = m_area->getAdjacentTable(m_currentPosition.z, m_currentPosition.x);

		// Set up the table so that waiters will go to it once food is ready
		m_area->v_waitingCustomerCells.push_back(tableTile);
		// Set a timer for the next activity
		setTimer();

		m_stage = PATRON_WAITING_SERVE;
		break;
	case PATRON_WALKING_EXIT :
		m_stage = PATRON_CAN_DELETE;
		break;
	}
}

void Patron::update() {
	if (m_isWalking)
		walk();
	else
		actOrWait();
}

void Patron::decrementOrTimeOut() {
	int decrementValue = TimeManager::Instance().DeltaTime * 1000;

	// Decrement timer and timeout if necessary
	if (m_time > decrementValue) {
		m_time -= decrementValue;
	}
	else {
		m_stage = PATRON_TIMED_OUT;
		findNextDestination();
	}
}