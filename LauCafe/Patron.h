#ifndef PATRON_H
#define PATRON_H

#include "Area.h"
#include "Person.h"

class Patron : public Person
{
public:
	Patron() {}
	Patron(Area* area, Mesh dude);
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
private:
	bool m_canDelete;
	bool m_hasBeenSeated;
	bool m_hasEaten;
	bool m_timedOut;
};

#endif