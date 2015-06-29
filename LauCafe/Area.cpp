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
	m_width = width;
	m_height = height;

	start.x = sx;
	start.z = sz;

	// Dynamically allocate the vectors
	floor = vector<TileType>(width * height);

	// Set the default values of the path length to INT_MAX
	pathLength = vector<vector<int>>(width * height);
	for (size_t i = 0; i < pathLength.size(); i++) {
		pathLength.at(i) = vector<int>(width * height);
		fill(pathLength.at(i).begin(), pathLength.at(i).end(), INT_MAX);
	}

	// Need to have pointers to Slots because of scope
	paths = vector<vector<deque<Cell*>>>(width * height);
	for (size_t i = 0; i < paths.size(); i++) {
		paths.at(i) = vector<deque<Cell*>>(width * height);
	}

	// Mark the start
	setTile(sz, sx, START);
}

Area::Area(int height, int width, int sz, int sx, vector<TileType> existingVector)
{
	m_width = width;
	m_height = height;

	start.x = sx;
	start.z = sz;

	// Dynamically allocate the vectors
	floor = vector<TileType>(existingVector);

	// Set the default values of the path length to INT_MAX
	pathLength = vector<vector<int>>(width * height);
	for (size_t i = 0; i < pathLength.size(); i++) {
		pathLength.at(i) = vector<int>(width * height);
		fill(pathLength.at(i).begin(), pathLength.at(i).end(), INT_MAX);
	}

	fillPaths();

	// Need to have pointers to Slots because of scope
	paths = vector<vector<deque<Cell*>>>(width * height);
	for (size_t i = 0; i < paths.size(); i++) {
		paths.at(i) = vector<deque<Cell*>>(width * height);
	}

	// Mark the start
	setTile(sz, sx, START);
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
		return this->floor[getIndex(z, x)] == WALKABLE;

	return false;
}

TileType Area::getTileType(int z, int x)
{
	if (!isInBounds(z, x))
		return INVALID;
	else
		return floor.at(getIndex(z, x));
}


TileType Area::getDestinationType(TileType origin) {
	switch (origin) {
	case START:
	case LOBBYCHAIR:
	case STOVE:
	case BAR:
		return TABLE;
	case TOILET:
	case TABLE:
		return START;
	}

	return INVALID;
}

// Used primarily in pathfinding
int Area::getCellPathLength(int sz, int sx, int dz, int dx)
{
	// Only used in a min comparison, use INT_MAX to use other option
	if (!isInBounds(sz, sx) || !isInBounds(dz, dx) || !isWalkable(dz, dx))
		return INT_MAX;
	else
		return pathLength.at(getIndex(sz, sx)).at(getIndex(dz, dx));
}

// Get the path from (sx, sz) to (dx, dz)
deque<Cell*> Area::getCellPath(int sz, int sx, int dz, int dx)
{
	deque<Cell*> d;
	if (!isInBounds(sz, sx) || !isInBounds(dz, dz))
		return d;
	else
		return paths.at(getIndex(sz, sx)).at(getIndex(dz, dx));
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

void Area::setTile(int z, int x, TileType tileType)
{
	if (isInBounds(z, x))
		this->floor[getIndex(z, x)] = tileType;
}

void Area::fillPathLength()
{
	for (int i = 0; i < m_height; i++) {
		for (int j = 0; j < m_width; j++) {
			deque<Cell> current, next;

			Cell currentCell;
			currentCell.x = j;
			currentCell.z = i;
			current.push_back(currentCell);

			int length = 1;
			int size = this->floor.size();

			do {
				// Have to set next to empty after while comparison
				deque<struct Cell> emptyDeque;
				next = emptyDeque;

				while (!current.empty()) {
					Cell c = current.back();

					// Check if adjacent cells are walkable and unmarked
					if (isWalkable(c.z, c.x - 1) &&
						getCellPathLength(currentCell.z, currentCell.x, c.z, c.x - 1) == INT_MAX) {
						// Set the length in the "origin" cell's matrix's destination cell and push it to the deque
						pathLength.at(getIndex(currentCell.z, currentCell.x))
							.at(getIndex(c.z, c.x - 1)) = length;
						Cell newSlot;
						newSlot.x = c.x - 1;
						newSlot.z = c.z;
						next.push_back(newSlot);
					}

					if (isWalkable(c.z, c.x + 1) &&
						getCellPathLength(currentCell.z, currentCell.x, c.z, c.x + 1) == INT_MAX) {
						pathLength.at(getIndex(currentCell.z, currentCell.x))
							.at(getIndex(c.z, c.x + 1)) = length;
						Cell newSlot;
						newSlot.x = c.x + 1;
						newSlot.z = c.z;
						next.push_back(newSlot);
					}

					if (isWalkable(c.z - 1, c.x) &&
						getCellPathLength(currentCell.z, currentCell.x, c.z - 1, c.x) == INT_MAX) {
						pathLength.at(getIndex(currentCell.z, currentCell.x))
							.at(getIndex(c.z - 1, c.x)) = length;
						Cell newSlot;
						newSlot.x = c.x;
						newSlot.z = c.z - 1;
						next.push_back(newSlot);
					}

					if (isWalkable(c.z + 1, c.x) &&
						getCellPathLength(currentCell.z, currentCell.x, c.z + 1, c.x) == INT_MAX) {
						pathLength.at(getIndex(currentCell.z, currentCell.x))
							.at(getIndex(c.z + 1, c.x)) = length;
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
	}
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
			TileType tileType = floor[getIndex(z, x)];
			TileType destinationType = getDestinationType(tileType);

			for (int i = 0; i < m_height; i++)
				for (int j = 0; j < m_width; j++) {
					if (floor[getIndex(i, j)] == destinationType) {
						// The path matrix doesn't have the distance on the destination cell. Need surrounding minimum
						int minLength1 = min(getCellPathLength(z, x, i, j + 1),
							getCellPathLength(z, x, i, j - 1));
						int minLength2 = min(getCellPathLength(z, x, i + 1, j),
							getCellPathLength(z, x, i - 1, j));
						int minLength = min(minLength1, minLength2);
						int totalLength = minLength + 1;

						// Current coordinates
						int xx = j;
						int zz = i;

						Cell* cell = new Cell;
						cell->x = xx;
						cell->z = zz;
						paths.at(getIndex(z, x)).at(getIndex(i, j)).push_front(cell);

						while (totalLength-- > 0) {
							if (isInBounds(zz, xx - 1) && getCellPathLength(z, x, zz, xx - 1) == totalLength) {
								// Push a new cell onto the deque for later use
								Cell* c = new Cell;
								c->x = --xx;
								c->z = zz;
								paths.at(getIndex(z, x)).at(getIndex(i, j)).push_front(c);

								continue;
							}

							if (isInBounds(zz, xx + 1) && getCellPathLength(z, x, zz, xx + 1) == totalLength) {
								Cell* c = new Cell;
								c->x = ++xx;
								c->z = zz;
								paths.at(getIndex(z, x)).at(getIndex(i, j)).push_front(c);

								continue;
							}

							if (isInBounds(zz - 1, xx) && getCellPathLength(z, x, zz - 1, xx) == totalLength) {
								Cell* c = new Cell;
								c->x = xx;
								c->z = --zz;
								paths.at(getIndex(z, x)).at(getIndex(i, j)).push_front(c);

								continue;
							}

							if (isInBounds(zz + 1, xx) && getCellPathLength(z, x, zz + 1, xx) == totalLength) {
								Cell* c = new Cell;
								c->x = xx;
								c->z = ++zz;
								paths.at(getIndex(z, x)).at(getIndex(i, j)).push_front(c);

								continue;
							}
						}
					}
				}
		}
}

void Area::clearPaths() {
	for (size_t i = 0; i < paths.size(); i++) {
		for (size_t j = 0; j < paths.size(); j++)
			while (!paths.at(i).at(j).empty()) {
				Cell* c = paths.at(i).at(j).front();
				delete c;
				paths.at(i).at(j).pop_front();
			}
	}

	for (size_t i = 0; i < pathLength.size(); i++)
		for (size_t j = 0; j < pathLength.size(); j++)
			pathLength.at(i).at(j) = INT_MAX;
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
			if (floor[getIndex(z, x)] == START) {
				// If the cell is a potential destination, print the path
				for (int i = 0; i < m_height; i++)
					for (int j = 0; j < m_width; j++) {
						for (int k = 0; k < paths.at(getIndex(z, x)).at(getIndex(i, j)).size(); k++) {
							Cell* top = paths.at(getIndex(z, x)).at(getIndex(i, j)).at(k);
							// 7 is just a visual symbol to represent the path
							pathMap[getIndex(top->z, top->x)] = 7;
						}
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