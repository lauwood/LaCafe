#include "Patron.h"
#include "TimeManager.h"

Patron::Patron(Area* area, Mesh dude) : Person(area) {
	// Initialize model and shader
	m_mesh = Mesh(dude, "Shaders/Banana_vs.glsl", "Shaders/Banana_fs.glsl");
	m_mesh.SetPosition(vec3(0, 0.5, 0));
	m_mesh.SetScale(vec3(0.5, 0.5, 0.5));

	// Initialize status variables
	m_canDelete = false;
	m_isWalking = true;
	m_hasBeenDirected = false;
	m_hasBeenSeated = false;
	m_hasEaten = false;
	m_timedOut = false;

	Cell entrance = area->getStart();
	m_currentPosition.x = entrance.x;
	m_currentPosition.z = entrance.z;

	// Immediately find the next destination so it can start walking right away
	findNextDestination();

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

	if (m_hasEaten || m_timedOut) destinationType = START;
	else if (m_hasBeenDirected) destinationType = TABLE_CHAIR;
	else destinationType = RECEPTION;

	// Find the next available destination (not necessarily closest)
	for (int z = 0; z < m_area->getHeight() && !foundDestination; z++)
		for (int x = 0; x < m_area->getWidth() && !foundDestination; x++) {
			if (m_area->getTileType(z, x) == destinationType && m_area->getTileStatus(z, x) == OPEN) {
				m_destination.x = x;
				m_destination.z = z;
				foundDestination = true;
			}
	}

	if (!foundDestination)
		// Still haven't found a destination, leave!
		m_destination = m_area->getStart();

	// Set the path of the patron to that of the path array
	m_pathToNextDestination = m_area->getCellPath(m_currentPosition.z, m_currentPosition.x,
		m_destination.z, m_destination.x);
	// Reserve the tile so that no two patrons can take the same tile (even when walking to it)
	if (destinationType == TABLE_CHAIR) {
		m_tableCell = m_area->getAdjacentTable(m_destination.z, m_destination.x);
		m_area->setTileStatus(m_destination.z, m_destination.x, RESERVED);
		m_area->setTileStatus(m_tableCell.z, m_tableCell.x, RESERVED);
	}

	m_isWalking = true;

	// Clear variable if path was found before
	m_pathIndex = 0;
}

// Translate the model across the appropriate axis and speed
// Calls arrival function when path is exhausted
void Patron::walk() {
	double delta = TimeManager::Instance().DeltaTime;
	if(m_pathIndex < m_pathToNextDestination.size()) {
		double dx, dz;
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

		m_mesh.SetPosition(vec3(m_currentPosition.z + dz, 0.5, m_currentPosition.x + dx));
	}
}

void Patron::act() {
	int decrementValue = TimeManager::Instance().DeltaTime * 1000;
	if (m_isBusy)
		if (m_time > decrementValue) {
			m_time -= decrementValue;
		}
		else {
			// Person may "do nothing" for a frame
			m_time = 0;

			m_area->setTileStatus(m_tableCell.z, m_tableCell.x, DIRTY);

			// Prep for the next tick
			findNextDestination();
			setWalking();
		}
}

void Patron::arrive() {
	m_pathIndex = 0;
	Cell start = m_area->getStart();
	if (m_currentPosition.x == start.x && m_currentPosition.z == start.z) {
		// Customer is done and we can mark this for deletion
		m_canDelete = true;
	}
	else {
		if (!m_hasBeenSeated) {
			// If arrived at a table, mark it
			// Don't need to mark the chair, it's "reserved" so nobody will walk to it
			Cell tableTile = m_area->getAdjacentTable(m_currentPosition.z, m_currentPosition.x);
			m_area->setTileStatus(tableTile.z, tableTile.x, WAITING);
			m_hasBeenSeated = true;

			// Increment so cooks get to work ASAP
			m_area->seatCustomer();

			// Set up the table so that waiters will go to it once food is ready
			m_area->v_waitingCustomerCells.push_back(tableTile);

			finishWalking();
			// Set a timer for the next activity
			setTimer();
		}
	}
}

void Patron::update() {
	if (m_isWalking)
		walk();
	else if (m_isBusy)
		act();
	else if (m_isWaiting)
		wait();
	return;
}

void Patron::wait() {
	int decrementValue = TimeManager::Instance().DeltaTime * 1000;

	if (!m_hasBeenDirected && m_area->recStatus == R_READY) {
		m_hasBeenDirected = true;
		findNextDestination();
		m_area->recStatus = R_JUST_DIRECTED;
	}
	else if (m_isWaiting)
		if (m_area->getTileStatus(m_tableCell.z, m_tableCell.x) == FOOD_READY) {
			// Food arrived
			m_isWaiting = false;
			m_isBusy = true;
			setTimer();
			m_area->setTileStatus(m_tableCell.z, m_tableCell.x, EATING);
		}
		else if (m_time > decrementValue) {
			m_time -= decrementValue;
		}
		else {
			// If food is coming, don't leave
			if (m_hasBeenSeated && m_area->getTileStatus(m_tableCell.z, m_tableCell.x) == FOOD_COMING)
				return;
			// Mark for deletion, waiting too long
			m_time = 0;
			m_timedOut = true;
			findNextDestination();
			setWalking();

			// Unreserve the table and chair
			m_area->setTileStatus(m_currentPosition.z, m_currentPosition.x, OPEN);
			m_area->setTileStatus(m_tableCell.z, m_tableCell.x, OPEN);
		}
}