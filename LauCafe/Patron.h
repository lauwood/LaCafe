#ifndef PATRON_H
#define PATRON_H

#include "MeshLoader.h"
#include "Area.h"
#include "Person.h"

class Patron : public Person
{
public:
	Patron() {}
	Patron(Area* area, Camera *m_Camera);
	~Patron();

	// Implement virtual functions
	void findNextDestination();
	void finishCurrentTask();
	void act();
	void arrive();
	void update();

	void walk();
	void wait();

	bool canDelete() { return m_canDelete; }

	void Render();
private:
	Mesh m_mesh;
	int m_pathIndex;
	bool m_canDelete;
};

#endif