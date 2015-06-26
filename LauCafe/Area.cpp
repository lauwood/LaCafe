#include "Area.h"
#include <algorithm>
#include <iostream>

using namespace std;

/*
* The convention used is z, x, where z is the height and x is the width in the matrix
* This is to keep from hopping through arrays
*/

Area::Area(int height, int width, int sz, int sx)
{
	this->m_width = width;
	this->m_height = height;

	// Dynamically allocate the vectors
	floor = vector<int>(width * height);
	// Set the default values of the path length to INT_MAX
	pathLength = vector<int>(width * height);
	fill(pathLength.begin(), pathLength.end(), INT_MAX);
	// Need to have pointers to Slots because of scope
	paths = vector<deque<Cell*>>(width * height);

	this->start.x = sx;
	this->start.z = sz;

	// Mark the start
	setTile(sz, sx, 1);
}

Area::Area(int height, int width, int sz, int sx, vector<int> existingVector)
{
	this->m_width = width;
	this->m_height = height;

	// Dynamically allocate the vectors
	floor = vector<int>(existingVector);
	// Set the default values of the path length to INT_MAX
	pathLength = vector<int>(width * height);
	fill(pathLength.begin(), pathLength.end(), INT_MAX);
	// Need to have pointers to Slots because of scope
	paths = vector<deque<Cell*>>(width * height);

	this->start.x = sx;
	this->start.z = sz;

	// Mark the start
	setTile(sz, sx, 1);
}

Area::~Area()
{
	clearPaths();
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

int Area::getTileType(int z, int x)
{
	if (!isInBounds(z, x))
		return -1;
	else
		return floor.at(getIndex(z, x));
}

// Used primarily in pathfinding
int Area::getCellPathLength(int z, int x)
{
	// Only used in a min comparison, use INT_MAX to use other option
	if (!isInBounds(z, x) || !isWalkable(z, x))
		return INT_MAX;
	else
		return pathLength.at(getIndex(z, x));
}

deque<Cell*> Area::getCellPath(int z, int x)
{
	deque<Cell*> d;
	if (!isInBounds(z, x))
		return d;
	else
		return paths.at(getIndex(z, x));
}

/*==============================
===========MUTATORS=============
==============================*/

void Area::setTile(int z, int x, int tileType)
{
	if (isInBounds(z, x))
		this->floor[getIndex(z, x)] = tileType;
}

void Area::fillPathLength()
{
	// Reset the length grid so it doesn't use old values
	fill(pathLength.begin(), pathLength.end(), INT_MAX);

	deque<struct Cell> current, next;
	current.push_back(this->start);

	int length = 1;
	int size = this->floor.size();

	do {
		// Have to set next to empty after while comparison
		deque<struct Cell> emptyDeque;
		next = emptyDeque;

		while (!current.empty()) {
			Cell c = current.back();

			// Mark the distance to adjacent cells (if they're walkable) and push onto next stack
			if (isWalkable(c.z, c.x - 1) && getTileType(c.z, c.x - 1) == 0 && getCellPathLength(c.z, c.x - 1) == INT_MAX) {
				pathLength.at(getIndex(c.z, c.x - 1)) = length;
				Cell newSlot;
				newSlot.x = c.x - 1;
				newSlot.z = c.z;
				next.push_back(newSlot);
			}

			if (isWalkable(c.z, c.x + 1) && getTileType(c.z, c.x + 1) == 0 && getCellPathLength(c.z, c.x + 1) == INT_MAX) {
				pathLength.at(getIndex(c.z, c.x + 1)) = length;
				Cell newSlot;
				newSlot.x = c.x + 1;
				newSlot.z = c.z;
				next.push_back(newSlot);
			}

			if (isWalkable(c.z - 1, c.x) && getTileType(c.z - 1, c.x) == 0 && getCellPathLength(c.z - 1, c.x) == INT_MAX) {
				pathLength.at(getIndex(c.z - 1, c.x)) = length;
				Cell newSlot;
				newSlot.x = c.x;
				newSlot.z = c.z - 1;
				next.push_back(newSlot);
			}

			if (isWalkable(c.z + 1, c.x) && getTileType(c.z + 1, c.x) == 0 && getCellPathLength(c.z + 1, c.x) == INT_MAX) {
				pathLength.at(getIndex(c.z + 1, c.x)) = length;
				Cell newSlot;
				newSlot.x = c.x;
				newSlot.z = c.z + 1;
				next.push_back(newSlot);
			}

			// Pop the current cell, as we're done looking at its neighbors
			current.pop_back();
		}

		// The current stack is emptied
		length++;
		current = next;
	} while (!next.empty());
}

void Area::fillPaths()
{
	// Clear the previously generated path matrix
	clearPaths();
	fillPathLength();
	/*
	* Pseudocode:
	*	1. "Flood" the walkable matrix with shortest distance from the start point.
	*	2. For all the destinations (represented by "2") find the minimum number adjacent
	*	   to it (arbitrarily break ties).
	*	3. Update the current cell to that cell.
	*	4. Push the cell onto the deque, and look for an adjacent cell that has the
	*	   current cell's distance -1.
	*	5. Go to step 3 until the current distance is 0.
	*/

	for (int z = 0; z < m_height; z++)
		for (int x = 0; x < m_width; x++) {
			if (floor[getIndex(z, x)] == 2) {
				// The path matrix doesn't have the distance on the destination cell. Need surrounding minimum
				int minLength1 = min(getCellPathLength(z, x + 1), getCellPathLength(z, x - 1));
				int minLength2 = min(getCellPathLength(z + 1, x), getCellPathLength(z - 1, x));
				int minLength = min(minLength1, minLength2);
				int totalLength = minLength + 1;

				// Current coordinates
				int xx = x;
				int zz = z;

				while (totalLength-- > 0) {
					if (isInBounds(zz, xx - 1) && getCellPathLength(zz, xx - 1) == totalLength) {
						// Push a new cell onto the deque for later use
						Cell* c = new Cell;
						c->x = --xx;
						c->z = zz;
						paths.at(getIndex(z, x)).push_back(c);

						continue;
					}

					if (isInBounds(zz, xx + 1) && getCellPathLength(zz, xx + 1) == totalLength) {
						Cell* c = new Cell;
						c->x = ++xx;
						c->z = zz;
						paths.at(getIndex(z, x)).push_back(c);

						continue;
					}

					if (isInBounds(zz - 1, xx) && getCellPathLength(zz - 1, xx) == totalLength) {
						Cell* c = new Cell;
						c->x = xx;
						c->z = --zz;
						paths.at(getIndex(z, x)).push_back(c);

						continue;
					}

					if (isInBounds(zz + 1, xx) && getCellPathLength(zz + 1, xx) == totalLength) {
						Cell* c = new Cell;
						c->x = xx;
						c->z = ++zz;
						paths.at(getIndex(z, x)).push_back(c);

						continue;
					}
				}
			}
		}
}

void Area::clearPaths() {
	for (size_t i = 0; i < paths.size(); i++) {
		while (!paths.at(i).empty()) {
			Cell* c = paths.at(i).front();
			delete c;
			paths.at(i).pop_front();
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

	for (int z = 0; z < m_height; z++)
		for (int x = 0; x < m_width; x++) {
			if (floor[getIndex(z, x)] == 2) {
				// If the cell is a potential destination, print the path
				for (size_t i = 0; i < paths[getIndex(z, x)].size(); i++) {
					Cell* top = paths[getIndex(z, x)].at(i);
					// 7 is just a visual symbol to represent the path
					pathMap[getIndex(top->z, top->x)] = 7;
				}
			}
		}

	// Print the "7" paths
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