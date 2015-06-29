#include "Patron.h"

Patron::Patron(Area* area, Camera *m_Camera) : Person(area) {
	p_Area = area;
	dude = Mesh("Models/Dude.fbx", "Shaders/Banana_vs.glsl", "Shaders/Banana_fs.glsl");
	dude.SetCamera(m_Camera);
	dude.SetPosition(vec3(0, 0.5, 0));
	dude.SetScale(vec3(0.5, 0.5, 0.5));

	m_currentPosition.x = 0;
	m_currentPosition.z = 0;
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

void Patron::walkToCells() {
	for (; pathIndex < path.size(); pathIndex++) {
		Cell* c = path.at(pathIndex);
		dude.SetPosition(vec3(c->z, 0.5, c->x));
	}
}

void Patron::finishCurrentTask() {}

void Patron::Render() {
	dude.Render();
}