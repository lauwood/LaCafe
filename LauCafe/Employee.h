#pragma once
#include "Person.h"
#include "Area.h"
#include "MeshLoader.h"
#include <stdlib.h>
#include <time.h>

// To facilitate role switching, no inheritance to each of the roles
enum Role { RECEPTIONIST, COOK, BARISTA, WAITER, DISHWASHER };

enum WaiterStage { WAITER_IDLE, WAITER_WALKING_STOVE, WAITER_CARRYING_FOOD };
enum DishwasherStage { DISHWASHER_IDLE, DISHWASHER_WALKING_TABLE, DISHWASHER_CLEANING };

class Employee :
	public Person
{
public:
	Employee() {}
	Employee(Area* area);
	~Employee();

	// Implement virtual functions
	void findNextDestination();
	void actOrWait();
	void arrive();
	void update();

	void setRole(Role role);
private:
	bool m_isBusy;			// Cooking, cleaning, eating, toilet, etc.
	Role m_role;

	WaiterStage m_waiterStage;
	DishwasherStage m_dishwasherStage;
};

