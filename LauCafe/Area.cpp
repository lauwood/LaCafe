#include "Area.h"
#include <algorithm>
#include <iostream>

using namespace std;

/*
* The convention used is z, x, where z is the height and x is the width in the matrix
* This is to keep from hopping through arrays
*/

Area::Area(int z, int x, int sz, int sx)
{
	this->m_width = x;
	this->m_height = z;

	// Dynamically allocate the vectors
	floor = vector<int>(x*z);
	// Need to have pointers to Slots because of scope
	paths = vector<stack<struct Cell*>*>(x * z);

	this->start.x = sx;
	this->start.z = sz;

	// Mark the start
	setTile(sz, sx, 1);
	setTile(2, 2, 2);
}

Area::Area(int z, int x, int sz, int sx, vector<int> v)
{
	this->m_width = x;
	this->m_height = z;

	// Dynamically allocate the vectors
	floor = vector<int>(v);
	// Need to have pointers to Slots because of scope
	paths = vector<stack<Cell*>*>(x * z);

	this->start.x = sx;
	this->start.z = sz;

	// Mark the start
	setTile(sz, sx, 1);
}

Area::~Area()
{
	for (size_t i = 0; i < paths.size(); i++) {
		while (!paths[i]->empty()) {
			Cell* c = paths[i]->top();
			delete c;
			paths[i]->pop();
		}
		delete paths[i];
	}
}


/*==============================
==========ACCESSORS=============
==============================*/
bool Area::isInBounds(int z, int x)
{
	return x < m_width && z < m_height && x >= 0 && z >= 0;
}

// 0 is walkable
bool Area::isWalkable(int z, int x)
{
	if (isInBounds(z, x))
		return this->floor[getIndex(z, x)] == 0;

	return false;
}

// Because we are using a vector to represent a 2D array, there must be a bit of math
// to translate from two numbers to one
int Area::getIndex(int z, int x)
{
	return x + m_height * z;
}

/*==============================
===========MUTATORS=============
==============================*/

void Area::setTile(int z, int x, int t)
{
	if (isInBounds(z, x))
		this->floor[getIndex(z, x)] = t;
}

void Area::getPathLengthGrid(vector<int>& pathLengthGrid)
{
	stack<struct Cell> current, next;
	current.push(this->start);

	// Clear the array just in case
	fill(pathLengthGrid.begin(), pathLengthGrid.end(), 0);

	int pathLength = 1;
	int size = this->floor.size();

	do {
		// Have to set next to empty after while comparison
		stack<struct Cell> emptyStack;
		next = emptyStack;

		while (!current.empty()) {
			Cell c = current.top();

			// Mark the distance to adjacent cells (if they're walkable) and push onto next stack
			if (isWalkable(c.z, c.x - 1) && pathLengthGrid[getIndex(c.z, c.x - 1)] == 0) {
				pathLengthGrid[getIndex(c.z, c.x - 1)] = pathLength;
				Cell newSlot;
				newSlot.x = c.x - 1;
				newSlot.z = c.z;
				next.push(newSlot);
			}

			if (isWalkable(c.z, c.x + 1) && pathLengthGrid[getIndex(c.z, c.x + 1)] == 0) {
				pathLengthGrid[getIndex(c.z, c.x + 1)] = pathLength;
				Cell newSlot;
				newSlot.x = c.x + 1;
				newSlot.z = c.z;
				next.push(newSlot);
			}

			if (isWalkable(c.z - 1, c.x) && pathLengthGrid[getIndex(c.z - 1, c.x)] == 0) {
				pathLengthGrid[getIndex(c.z - 1, c.x)] = pathLength;
				Cell newSlot;
				newSlot.x = c.x;
				newSlot.z = c.z - 1;
				next.push(newSlot);
			}

			if (isWalkable(c.z + 1, c.x) && pathLengthGrid[getIndex(c.z + 1, c.x)] == 0) {
				pathLengthGrid[getIndex(c.z + 1, c.x)] = pathLength;
				Cell newSlot;
				newSlot.x = c.x;
				newSlot.z = c.z + 1;
				next.push(newSlot);
			}

			// Pop the current cell, as we're done looking at its neighbors
			current.pop();
		}

		// The current stack is emptied
		pathLength++;
		current = next;
	} while (!next.empty());

	// Print out the distance matrix
	/*
	for (int i = 0; i < this->m_height; i++) {
	cout << "|";
	for (int j = 0; j < this->m_width; j++) {
	cout << pathLengthGrid[getIndex(i, j)] << "|";
	}
	cout << endl;
	}
	cout << endl;
	*/

}

void Area::fillPaths()
{
	/*
	* Pseudocode:
	*	1. "Flood" the walkable matrix with shortest distance from the start point.
	*	2. For all the destinations (represented by "2") find the minimum number adjacent
	*	   to it (arbitrarily break ties).
	*	3. Update the current cell to that cell.
	*	4. Push the cell onto the stack, and look for an adjacent cell that has the
	*	   current cell's distance -1.
	*	5. Go to step 3 until the current distance is 0.
	*/
	vector<int> pathLengths(m_height * m_width);
	getPathLengthGrid(pathLengths);

	for (int z = 0; z < m_height; z++)
		for (int x = 0; x < m_width; x++) {
			if (floor[getIndex(z, x)] == 2) {
				// The path matrix doesn't have the distance on the destination cell. Need surrounding minimum
				int minLength1 = min(pathLengths[getIndex(z, x + 1)], pathLengths[getIndex(z, x - 1)]);
				int minLength2 = min(pathLengths[getIndex(z + 1, x)], pathLengths[getIndex(z - 1, x)]);
				int minLength = min(minLength1, minLength2);
				int totalLength = minLength + 1;

				// Create a new stack for the path onto which to push cells
				stack<Cell*>* currentStack = new stack<Cell*>;
				// Current coordinates
				int xx = x;
				int zz = z;

				while (totalLength-- > 0) {
					if (isInBounds(zz, xx - 1) && pathLengths[getIndex(zz, xx - 1)] == totalLength) {
						// Push a new cell onto the stack
						Cell* c = new Cell;
						c->x = --xx;
						c->z = zz;
						currentStack->push(c);

						continue;
					}

					if (isInBounds(zz, xx + 1) && pathLengths[getIndex(zz, xx + 1)] == totalLength) {
						Cell* c = new Cell;
						c->x = ++xx;
						c->z = zz;
						currentStack->push(c);

						continue;
					}

					if (isInBounds(zz - 1, xx) && pathLengths[getIndex(zz - 1, xx)] == totalLength) {
						Cell* c = new Cell;
						c->x = xx;
						c->z = --zz;
						currentStack->push(c);

						continue;
					}

					if (isInBounds(zz + 1, xx) && pathLengths[getIndex(zz + 1, xx)] == totalLength) {
						Cell* c = new Cell;
						c->x = xx;
						c->z = ++zz;
						currentStack->push(c);

						continue;
					}
				}

				// The stack is complete, put it in the destination cell
				paths[getIndex(z, x)] = currentStack;
			}
		}
}

/*==============================
===========DEBUGGING============
==============================*/

void Area::printArray()
{
	cout << "Actual Floor Map" << endl;
	for (int z = 0; z < this->m_height; z++) {
		cout << "|";
		for (int x = 0; x < this->m_width; x++) {
			cout << floor[getIndex(z, x)] << "|";
		}
		cout << endl;
	}
}

// Prints all paths that customers take
void Area::printPaths()
{
	cout << "Possible paths taken" << endl;
	vector<int> pathMap(m_width * m_height);

	// Pull the stack from the cell and pop
	for (int z = 0; z < m_height; z++)
		for (int x = 0; x < m_width; x++) {
			if (floor[getIndex(z, x)] == 2) {
				// Copy the stack, we might need it later
				stack<Cell*>* path(paths[getIndex(z, x)]);

				// Don't delete the pointers in the copied stack
				while (!path->empty()) {
					Cell* top = path->top();
					// 7 is just a visual symbol to represent the path
					pathMap[getIndex(top->z, top->x)] = 7;
					path->pop();
				}
			}
		}

	// Print the "8" paths
	for (int z = 0; z < this->m_height; z++) {
		cout << "|";
		for (int x = 0; x < this->m_width; x++) {
			cout << pathMap[getIndex(z, x)] << "|";
		}
		cout << endl;
	}
	cout << endl;
	return;
}

// Prints all paths that customers take
void Area::getPathMap(vector<int>& pathMap)
{
	// Pull the stack from the cell and pop
	for (int z = 0; z < m_height; z++)
		for (int x = 0; x < m_width; x++) {
			if (floor[getIndex(z, x)] == 2) {
				// Copy the stack, we might need it later
				stack<Cell*>* path(paths[getIndex(z, x)]);

				// Don't delete the pointers in the copied stack
				while (!path->empty()) {
					Cell* top = path->top();
					// 7 is just a visual symbol to represent the path
					pathMap[getIndex(top->z, top->x)] = 7;
					path->pop();
				}
			}
		}
}