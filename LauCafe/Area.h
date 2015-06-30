#pragma once

#include <vector>
#include <deque>

using namespace std;

enum TileType { WALKABLE, START, OBSTACLE, TABLE, LOBBYCHAIR, STOVE, BAR, TOILET, INVALID};

// Represents a single tile in the array
struct Cell {
	int x;
	int z;
};

class Area
{
public:
	/*
	* The convention used in the function parameters is:
	* z: the vertical coordinate
	* x: the horizontal coordinate
	* The matrices used to represent the floor and paths are technically 1D
	*   vectors, but they are accessed using the getIndex(z,x) function
	* sz: the vertical coordinate of the start/origin cell
	* sx: the horizontal coordinate of the start/origin cell
	* dz: the vertical coordinate of the destination cell
	* dx: the horizontal coordinate of the destination cell
	*/

	Area() {}; // Used for avoiding a compile-time error
	Area(int height, int width, int sz, int sx);
	Area(int height, int width, int sz, int sx, vector<TileType> existingVector);
	~Area();

	// Accessors
	Cell getStart() { return m_start; }
	int getHeight() { return m_height; }
	int getWidth() { return m_width; }

	TileType getTileType(int z, int x);
	TileType getDestinationType(TileType);
	int getCellPathLength(int sz, int sx, int dz, int dx);
	deque<Cell*> getCellPath(int sz, int sx, int dz, int dx);

	bool isInBounds(int z, int x);
	bool isWalkable(int z, int x);
	bool isReserved(int z, int x);

	// Mutators
	void setTile(int z, int x, TileType tileType);
	void reserveTile(int z, int x);
	void unreserveTile(int z, int x);
	void fillPaths();
	void clearPaths();

	// Debugging info
	void printArray();
	void printPaths();

private:
	// This function is private to prevent accessing the right cell easily
	int getIndex(int z, int x);
	void fillPathLength();

	Cell m_start;
	int m_width;
	int m_height;

	// Dynamic arrays for expandable restaurants
	vector<TileType> v_typeVector;				// Represents the actual floor of the restaurant
	vector<vector<int>> v_pathLengthVector;		// Represents the path lengths to the appropriate destinations
	vector<vector<deque<Cell*>>> v_pathVector;	// Holds the shortest distance path for tables only
	vector<bool> v_reservationVector;			// Tells whether or not a cell is being used for an action
	vector<int> v_decorationVector;				// Tells whether or not a cell is being used for an action
};