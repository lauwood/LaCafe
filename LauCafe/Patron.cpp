#include "Patron.h"
#include "TimeManager.h"

Patron::Patron(Area* area, Camera *m_Camera) : Person(area) {
	p_Area = area;
	finished = false;
	m_isWalking = true;

	dude = Mesh("Models/Dude.fbx", "Shaders/Banana_vs.glsl", "Shaders/Banana_fs.glsl");
	dude.SetCamera(m_Camera);
	dude.SetPosition(vec3(0, 0.5, 0));
	dude.SetScale(vec3(0.5, 0.5, 0.5));

	Cell entrance = area->getStart();
	m_currentPosition.x = entrance.x;
	m_currentPosition.z = entrance.z;

	findNextDestination();
}

Patron::~Patron() {

}

void Patron::findNextDestination() {
	TileType currentTileType = p_Area->getTileType(m_currentPosition.z, m_currentPosition.x);
	TileType destinationType = p_Area->getDestinationType(currentTileType);

	for (int z = 0; z < p_Area->getHeight(); z++)
		for (int x = 0; x < p_Area->getWidth(); x++) {
			if (p_Area->getTileType(z, x) == destinationType) {
				m_destination.x = x;
				m_destination.z = z;
			}
	}
	path = p_Area->getCellPath(m_currentPosition.z, m_currentPosition.x, m_destination.z, m_destination.x);

	pathIndex = 0;
}

void Patron::walkToCells() {
	double delta = TimeManager::Instance().DeltaTime;
	if(pathIndex < path.size()) {
		double dx, dz;
		dx = dz = 0;

		m_distance += delta;

		Cell* c = path.at(pathIndex);
		Direction currentDirection = getDirection(&m_currentPosition, c);
		while (m_distance >= 1) {
			m_distance = m_distance + delta - 1;

			m_currentPosition = *c;
			if (++pathIndex < path.size()) {
				c = path.at(pathIndex);
			}
			else {
				m_distance = 0;
				currentDirection = LEFT; // Arbitrary
				finishWalking();
				arrive();
			}
		}
		
		currentDirection = getDirection(&m_currentPosition, c);
		switch (currentDirection) {
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

		dude.SetPosition(vec3(m_currentPosition.z + dz, 0.5, m_currentPosition.x + dx));
	}
}

void Patron::Render() {
	dude.Render();
}

void Patron::finishCurrentTask() {
	findNextDestination();
	setWalking();
}

void Patron::arrive() {
	pathIndex = 0;
	Cell start = p_Area->getStart();
	if (m_currentPosition.x == start.x && m_currentPosition.z == start.z) {
		// Customer is done and we can mark this for deletion
		finished = true;
	}
	else {
		// Set a timer for the next activity
		setTimer();
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
			findNextDestination();
			setWalking();
			walkToCells();
		}
}

void Patron::wait() {
	int decrementValue = TimeManager::Instance().DeltaTime;

	if (m_isWaiting)
		if (m_time > decrementValue) {
			m_time -= decrementValue;
		}
		else {
			// Mark for deletion, waiting too long
			m_time = 0;
			finished = true;
		}
}

void Patron::update() {
	if (m_isWalking)
		walkToCells();
	else if (m_isBusy)
		act();
	else if (m_isWaiting)
		wait();
	return;
}