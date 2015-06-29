#pragma once

#include <vector>
#include <deque>

using namespace std;

#define WALKABLE	0
#define START		1
#define OBSTACLE	2

#define TABLE		10
#define LOBBYCHAIR	11

#define STOVE		20
#define BAR			21

#define TOILET		30

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
	Area(int height, int width, int sz, int sx, vector<int> existingVector);
	~Area();

	// Accessors
	Cell getStart() { return start; }
	int getHeight() { return m_height; }
	int getWidth() { return m_width; }
	bool isInBounds(int z, int x);
	bool isWalkable(int z, int x);
	int getTileType(int z, int x);
	int getCellPathLength(int sz, int sx, int dz, int dx);
	deque<Cell*> getCellPath(int sz, int sx, int dz, int dx);

	// Mutators
	void setTile(int z, int x, int tileType);
	void fillPaths();
	void clearPaths();

	// Debugging info
	void printArray();
	void printPaths();

private:
	// This function is private to prevent accessing the right cell easily
	int getIndex(int z, int x);
	void fillPathLength();

	Cell start;
	int m_width;
	int m_height;

	// Dynamic arrays for expandable restaurants
	vector<int> floor;					// Represents the actual floor of the restaurant
	vector<vector<int>> pathLength;		// Represents the path lengths to the appropriate destinations
	vector<vector<deque<Cell*>>> paths;			// Holds the shortest distance path for tables only
};