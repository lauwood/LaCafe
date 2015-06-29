#include "Patron.h"

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
	for (int z = 0; z < p_Area->getHeight(); z++)
		for (int x = 0; x < p_Area->getWidth(); x++) {
			if (p_Area->getTileType(z, x) == TABLE) {
				m_destination.x = x;
				m_destination.z = z;
			}
	}
	std::deque<Cell*> DequeOfCells = p_Area->getCellPath(m_currentPosition.z, 
		m_currentPosition.x, m_destination.z, m_destination.x);

	path = DequeOfCells;
	pathIndex = 0;
}

void Patron::walkToCells(double delta) {
	if((size_t) pathIndex < path.size()) {
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

void Patron::update(double delta) {
	if (m_isWalking)
		walkToCells(delta);
	else if (m_isBusy)
		decrementTimer(delta);
	else if (m_isWaiting)
		decrementTimer(delta);
	return;
}