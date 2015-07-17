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

	m_start.x = sx;
	m_start.z = sz;

	// Dynamically allocate the vectors
	v_typeVector = vector<TileType>(width * height);
	v_statusVector = vector<TileStatus>(width * height);
	fill(v_statusVector.begin(), v_statusVector.end(), TILE_OPEN);
	v_decorationVector = vector<int>(width * height);
	g_GameObjects = vector<GameObject*>(width * height);

	// Set the default values of the path length to INT_MAX
	v_pathLengthVector = vector<vector<int>>(width * height);
	for (size_t i = 0; i < v_pathLengthVector.size(); i++) {
		v_pathLengthVector.at(i) = vector<int>(width * height);
		fill(v_pathLengthVector.at(i).begin(), v_pathLengthVector.at(i).end(), INT_MAX);
	}

	// Need to have pointers to Slots because of scope
	v_pathVector = vector<vector<deque<Cell*>>>(width * height);
	for (size_t i = 0; i < v_pathVector.size(); i++) {
		v_pathVector.at(i) = vector<deque<Cell*>>(width * height);
	}

	// Mark the start
	setTile(sz, sx, START);

	recStatus = REC_READY;

	canReachPodium = false;

	fillObjectVector();
}

Area::Area(int height, int width, int sz, int sx, vector<TileType> existingVector)
{
	m_width = width;
	m_height = height;

	m_start.x = sx;
	m_start.z = sz;

	// Dynamically allocate the vectors
	v_typeVector = vector<TileType>(existingVector);
	v_statusVector = vector<TileStatus>(width * height);
	fill(v_statusVector.begin(), v_statusVector.end(), TILE_OPEN);
	v_decorationVector = vector<int>(width * height);
	g_GameObjects = vector<GameObject*>(width * height);

	// Set the default values of the path length to INT_MAX
	v_pathLengthVector = vector<vector<int>>(width * height);
	for (size_t i = 0; i < v_pathLengthVector.size(); i++) {
		v_pathLengthVector.at(i) = vector<int>(width * height);
		fill(v_pathLengthVector.at(i).begin(), v_pathLengthVector.at(i).end(), INT_MAX);
	}

	fillPaths();

	// Need to have pointers to Slots because of scope
	v_pathVector = vector<vector<deque<Cell*>>>(width * height);
	for (size_t i = 0; i < v_pathVector.size(); i++) {
		v_pathVector.at(i) = vector<deque<Cell*>>(width * height);
	}

	// Mark the start
	setTile(sz, sx, START);

	recStatus = REC_READY;

	canReachPodium = false;
	fillObjectVector();
}

Area::~Area()
{
	clearPaths();
}

void Area::fillObjectVector() {
	// Fill in the object array from the floor array
	for (int i = 0; i < getHeight(); i++)
		for (int j = 0; j < getWidth(); j++) {
			TileType tileType = getTileType(i, j);
			switch (tileType) {
			case TABLE:
				g_GameObjects.at(getIndex(i, j)) = new GameObjectTable();
				g_GameObjects.at(getIndex(i, j))->Initialize(NULL);
				break;
			case TABLE_CHAIR:
				g_GameObjects.at(getIndex(i, j)) = new GameObjectChair();
				g_GameObjects.at(getIndex(i, j))->Initialize(ChairModel);
				break;
			case STOVE:
				g_GameObjects.at(getIndex(i, j)) = new GameObjectStove();
				g_GameObjects.at(getIndex(i, j))->Initialize(StoveModel);
				break;
			case RECEPTION:
				g_GameObjects.at(getIndex(i, j)) = new GameObjectPodium();
				g_GameObjects.at(getIndex(i, j))->Initialize(PodiumModel);
				break;
			case OBSTACLE:
				// Use podium as a placeholder model
				g_GameObjects.at(getIndex(i, j)) = new GameObjectPodium();
				g_GameObjects.at(getIndex(i, j))->Initialize(PodiumModel);
				break;
			}
		}
}


/*==============================
==========ACCESSORS=============
==============================*/

TileType Area::getTileType(int z, int x)
{
	if (!isInBounds(z, x))
		return INVALID_TYPE;
	else
		return v_typeVector.at(getIndex(z, x));
}

TileType Area::getDestinationType(TileType origin) {
	switch (origin) {
	case START:
		return RECEPTION;
	case RECEPTION:
	case STOVE:
	case BAR:
		return TABLE;
	case TOILET:
	case TABLE:
	case TABLE_CHAIR:
		return START;
	}

	return INVALID_TYPE;
}

// Used primarily in pathfinding
int Area::getCellPathLength(int sz, int sx, int dz, int dx)
{
	// Only used in a min comparison, use INT_MAX to use other option
	if (!isInBounds(sz, sx) || !isInBounds(dz, dx) || !isWalkable(dz, dx))
		return INT_MAX;
	else
		return v_pathLengthVector.at(getIndex(sz, sx)).at(getIndex(dz, dx));
}

// Get the path from (sx, sz) to (dx, dz)
deque<Cell*> Area::getCellPath(int sz, int sx, int dz, int dx)
{
	deque<Cell*> d;
	if (!isInBounds(sz, sx) || !isInBounds(dz, dz))
		return d;
	else
		return v_pathVector.at(getIndex(sz, sx)).at(getIndex(dz, dx));
}

Cell Area::getAdjacentTable(int z, int x) {
	Cell table;
	table.x = table.z = -1;

	if (getTileType(z - 1, x) == TABLE) {
		table.z = z - 1;
		table.x = x;
	}
	else if (getTileType(z + 1, x) == TABLE) {
		table.z = z + 1;
		table.x = x;
	}
	else if (getTileType(z, x + 1) == TABLE) {
		table.z = z;
		table.x = x + 1;
	}
	else if (getTileType(z, x - 1) == TABLE) {
		table.z = z;
		table.x = x - 1;
	}

	return table;
}

Cell Area::getAdjacentChair(int z, int x) {
	Cell chair;
	chair.x = chair.z = -1;

	if (getTileType(z - 1, x) == TABLE_CHAIR) {
		chair.z = z - 1;
		chair.x = x;
	}
	else if (getTileType(z + 1, x) == TABLE_CHAIR) {
		chair.z = z + 1;
		chair.x = x;
	}
	else if (getTileType(z, x + 1) == TABLE_CHAIR) {
		chair.z = z;
		chair.x = x + 1;
	}
	else if (getTileType(z, x - 1) == TABLE_CHAIR) {
		chair.z = z;
		chair.x = x - 1;
	}

	return chair;
}

Cell Area::getAdjacentStove(int z, int x) {
	Cell stove;
	stove.x = stove.z = -1;

	if (getTileType(z - 1, x) == STOVE || getTileType(z - 1, x) == BAR) {
		stove.z = z - 1;
		stove.x = x;
	}
	else if (getTileType(z + 1, x) == STOVE || getTileType(z + 1, x) == BAR) {
		stove.z = z + 1;
		stove.x = x;
	}
	else if (getTileType(z, x + 1) == STOVE || getTileType(z, x + 1) == BAR) {
		stove.z = z;
		stove.x = x + 1;
	}
	else if (getTileType(z, x - 1) == STOVE || getTileType(z, x - 1) == BAR) {
		stove.z = z;
		stove.x = x - 1;
	}

	return stove;
}

bool Area::isInBounds(int z, int x)
{
	return x < m_width && z < m_height && x >= 0 && z >= 0;
}

// 0 is walkable
bool Area::isWalkable(int z, int x)
{
	if (isInBounds(z, x))
		return v_typeVector[getIndex(z, x)] == WALKABLE;

	return false;
}

TileStatus Area::getTileStatus(int z, int x) {
	if (isInBounds(z, x))
		return v_statusVector.at(getIndex(z, x));
	else
		return TILE_INVALID_STATUS;
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
	if (!isInBounds(z, x))
		return;
	
	v_typeVector[getIndex(z, x)] = tileType;

	switch (tileType) {
	case TABLE:
		g_GameObjects.at(getIndex(z, x)) = new GameObjectTable();
		g_GameObjects.at(getIndex(z, x))->Initialize(NULL);
		break;
	case TABLE_CHAIR:
		g_GameObjects.at(getIndex(z, x)) = new GameObjectChair();
		g_GameObjects.at(getIndex(z, x))->Initialize(ChairModel);
		break;
	case STOVE:
		g_GameObjects.at(getIndex(z, x)) = new GameObjectStove();
		g_GameObjects.at(getIndex(z, x))->Initialize(StoveModel);
		break;
	case RECEPTION:
		g_GameObjects.at(getIndex(z, x)) = new GameObjectPodium();
		g_GameObjects.at(getIndex(z, x))->Initialize(PodiumModel);
		break;
	case OBSTACLE:
		// Use podium as a placeholder model
		g_GameObjects.at(getIndex(z, x)) = new GameObjectPodium();
		g_GameObjects.at(getIndex(z, x))->Initialize(PodiumModel);
		break;
	default:
		if (!g_GameObjects.at(getIndex(z, x)) == NULL)
			delete g_GameObjects.at(getIndex(z, x));
		g_GameObjects.at(getIndex(z, x)) = NULL;
	}
}

void Area::setTileStatus(int z, int x, TileStatus status) {
	if (isInBounds(z, x))
		v_statusVector.at(getIndex(z, x)) = status;
}

void Area::fillPaths()
{
	// Clear the previously generated path matrix
	clearPaths();
	fillPathLength();

	canReachPodium = false;

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

	Cell podiumCell;

	for (int z = 0; z < m_height; z++)
		for (int x = 0; x < m_width; x++)
			for (int i = 0; i < m_height; i++)
				for (int j = 0; j < m_width; j++) {
					// The path matrix doesn't have the distance on the destination cell. Need surrounding minimum
					int minLength1 = std::min(getCellPathLength(z, x, i, j + 1),
						getCellPathLength(z, x, i, j - 1));
					int minLength2 = std::min(getCellPathLength(z, x, i + 1, j),
						getCellPathLength(z, x, i - 1, j));
					int minLength = std::min(minLength1, minLength2);
					int totalLength = minLength + 1;

					// Current coordinates
					int xx = j;
					int zz = i;

					Cell* cell = new Cell;
					cell->x = xx;
					cell->z = zz;
					// Only push the chair or start tile onto the path
					if (getTileType(zz, xx) == TABLE_CHAIR || getTileType(zz, xx) == START)
						v_pathVector.at(getIndex(z, x)).at(getIndex(i, j)).push_front(cell);
					else if (getTileType(zz, xx) == RECEPTION) {
						podiumCell.x = xx;
						podiumCell.z = zz;
					}

					while (totalLength-- > 0) {
						if (isInBounds(zz, xx - 1) && getCellPathLength(z, x, zz, xx - 1) == totalLength) {
							// Push a new cell onto the deque for later use
							Cell* c = new Cell;
							c->x = --xx;
							c->z = zz;
							v_pathVector.at(getIndex(z, x)).at(getIndex(i, j)).push_front(c);

							continue;
						}

						if (isInBounds(zz, xx + 1) && getCellPathLength(z, x, zz, xx + 1) == totalLength) {
							Cell* c = new Cell;
							c->x = ++xx;
							c->z = zz;
							v_pathVector.at(getIndex(z, x)).at(getIndex(i, j)).push_front(c);

							continue;
						}

						if (isInBounds(zz - 1, xx) && getCellPathLength(z, x, zz - 1, xx) == totalLength) {
							Cell* c = new Cell;
							c->x = xx;
							c->z = --zz;
							v_pathVector.at(getIndex(z, x)).at(getIndex(i, j)).push_front(c);

							continue;
						}

						if (isInBounds(zz + 1, xx) && getCellPathLength(z, x, zz + 1, xx) == totalLength) {
							Cell* c = new Cell;
							c->x = xx;
							c->z = ++zz;
							v_pathVector.at(getIndex(z, x)).at(getIndex(i, j)).push_front(c);

							continue;
						}
					}
				}

	int podiumLength1 = getCellPathLength(m_start.z, m_start.x, podiumCell.z + 1, podiumCell.x);
	int podiumLength2 = getCellPathLength(m_start.z, m_start.x, podiumCell.z - 1, podiumCell.x);
	int podiumLength3 = getCellPathLength(m_start.z, m_start.x, podiumCell.z, podiumCell.x + 1);
	int podiumLength4 = getCellPathLength(m_start.z, m_start.x, podiumCell.z, podiumCell.x - 1);
	canReachPodium = (podiumLength1 < INT_MAX) || (podiumLength2 < INT_MAX) ||
		(podiumLength3 < INT_MAX) || (podiumLength4 < INT_MAX);
}

void Area::clearPaths() {
	for (size_t i = 0; i < v_pathVector.size(); i++) {
		for (size_t j = 0; j < v_pathVector.size(); j++)
			while (!v_pathVector.at(i).at(j).empty()) {
				Cell* c = v_pathVector.at(i).at(j).front();
				delete c;
				v_pathVector.at(i).at(j).pop_front();
			}
	}

	for (size_t i = 0; i < v_pathLengthVector.size(); i++)
		for (size_t j = 0; j < v_pathLengthVector.size(); j++)
			v_pathLengthVector.at(i).at(j) = INT_MAX;
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
			int size = v_typeVector.size();

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
						v_pathLengthVector.at(getIndex(currentCell.z, currentCell.x))
							.at(getIndex(c.z, c.x - 1)) = length;
						Cell newSlot;
						newSlot.x = c.x - 1;
						newSlot.z = c.z;
						next.push_back(newSlot);
					}

					if (isWalkable(c.z, c.x + 1) &&
						getCellPathLength(currentCell.z, currentCell.x, c.z, c.x + 1) == INT_MAX) {
						v_pathLengthVector.at(getIndex(currentCell.z, currentCell.x))
							.at(getIndex(c.z, c.x + 1)) = length;
						Cell newSlot;
						newSlot.x = c.x + 1;
						newSlot.z = c.z;
						next.push_back(newSlot);
					}

					if (isWalkable(c.z - 1, c.x) &&
						getCellPathLength(currentCell.z, currentCell.x, c.z - 1, c.x) == INT_MAX) {
						v_pathLengthVector.at(getIndex(currentCell.z, currentCell.x))
							.at(getIndex(c.z - 1, c.x)) = length;
						Cell newSlot;
						newSlot.x = c.x;
						newSlot.z = c.z - 1;
						next.push_back(newSlot);
					}

					if (isWalkable(c.z + 1, c.x) &&
						getCellPathLength(currentCell.z, currentCell.x, c.z + 1, c.x) == INT_MAX) {
						v_pathLengthVector.at(getIndex(currentCell.z, currentCell.x))
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

/*==============================
===========DEBUGGING============
==============================*/

void Area::printArray()
{
	cout << "Actual Floor Map" << endl;
	for (int z = 0; z < this->m_height; z++) {
		cout << "|";
		for (int x = 0; x < this->m_width; x++) {
			cout << v_typeVector[getIndex(z, x)] << "|";
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
			if (v_typeVector[getIndex(z, x)] == START) {
				// If the cell is a potential destination, print the path
				for (int i = 0; i < m_height; i++)
					for (int j = 0; j < m_width; j++) {
						for (unsigned int k = 0; k < v_pathVector.at(getIndex(z, x)).at(getIndex(i, j)).size(); k++) {
							Cell* top = v_pathVector.at(getIndex(z, x)).at(getIndex(i, j)).at(k);
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