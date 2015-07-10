#ifndef PATRON_H
#define PATRON_H

#include "Area.h"
#include "Person.h"

// Waiting serve = waiting for a cook
// Waiting food = waiting for food to arrive
enum PatronStage {
	PATRON_INIT, PATRON_WALKING_REC,
	PATRON_WAITING_REC, PATRON_WALKING_TABLE,
	PATRON_WAITING_SERVE, PATRON_WAITING_FOOD, PATRON_EATING_FOOD,
	PATRON_WALKING_EXIT, PATRON_CAN_DELETE, PATRON_TIMED_OUT
};

class Patron : public Person
{
public:
	Patron() {}
	Patron(Area* area, Mesh *model);
	~Patron();

	// Implement virtual functions
	void findNextDestination();
	void actOrWait();
	void arrive();
	void update();

	void decrementOrTimeOut();
	bool canDelete() { return m_stage == PATRON_CAN_DELETE; }
private:

	Cell m_tableCell;
	PatronStage m_stage;
};

#endif