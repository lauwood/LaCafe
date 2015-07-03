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
	else if (m_hasBeenSeated) destinationType = TABLE;
	else destinationType = RECEPTION;

	// Find the next available destination (not necessarily closest)
	for (int z = 0; z < m_area->getHeight() && !foundDestination; z++)
		for (int x = 0; x < m_area->getWidth() && !foundDestination; x++) {
			if (m_area->getTileType(z, x) == destinationType && !m_area->isReserved(z, x)) {
				m_destination.x = x;
				m_destination.z = z;
				foundDestination = true;
			}
	}

	// Set the path of the patron to that of the path array
	m_pathToNextDestination = m_area->getCellPath(m_currentPosition.z, m_currentPosition.x,
		m_destination.z, m_destination.x);
	// Reserve the tile so that no two patrons can take the same tile (even when walking to it)
	if (destinationType == TABLE)
		m_area->reserveTile(m_destination.z, m_destination.x);

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
				m_direction = LEFT; // Arbitrary
				finishWalking();
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
		}

		m_mesh.SetPosition(vec3(m_currentPosition.z + dz, 0.5, m_currentPosition.x + dx));
	}
}

void Patron::finishCurrentTask() {
	findNextDestination();
	setWalking();
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

			m_area->unreserveTile(m_currentPosition.z, m_currentPosition.x);

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
		finishWalking();
		// Set a timer for the next activity
		setTimer();
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

	if (m_isWaiting)
		if (m_time > decrementValue) {
			m_time -= decrementValue;
		}
		else {
			// Mark for deletion, waiting too long
			m_time = 0;
			m_timedOut = true;
			findNextDestination();
			setWalking();
		}
}