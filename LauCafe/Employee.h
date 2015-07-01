#pragma once
#include "Person.h"
#include "Area.h"
#include "MeshLoader.h"
#include <stdlib.h>
#include <time.h>

enum Role { RECEPTIONIST, COOK, WAITER, DISHWASHER };

class Employee :
	public Person
{
public:
	Employee() {}
	Employee(Area* area, Camera *m_Camera);
	~Employee();

	// Implement virtual functions
	void findNextDestination();
	void finishCurrentTask();
	void act();
	void arrive();
	void update();
	void wait();

private:
	Role m_role;
};

