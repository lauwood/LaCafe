#pragma once
#include "Person.h"
#include "Area.h"
#include "MeshLoader.h"
#include <stdlib.h>
#include <time.h>

// To facilitate role switching, no inheritance to each of the roles
enum Role { RECEPTIONIST, COOK, BARISTA, WAITER, DISHWASHER };

class Employee :
	public Person
{
public:
	Employee() {}
	Employee(Area* area);
	~Employee();

	// Implement virtual functions
	void findNextDestination();
	void act();
	void arrive();
	void update();
	void wait();

	void setRole(Role);
private:
	Role m_role;
	bool m_isIdle;
	bool m_carryingFood;
};

