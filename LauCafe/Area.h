#pragma once

#include <vector>
#include <deque>

using namespace std;

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
	*/
	Area() {}
	Area(int height, int width, int sz, int sx);
	Area(int height, int width, int sz, int sx, vector<int> existingVector);
	~Area();

	// Accessors
	int getHeight() { return m_height; }
	int getWidth() { return m_width; }
	bool isInBounds(int z, int x);
	bool isWalkable(int z, int x);
	int getTileType(int z, int x);
	int getCellPathLength(int z, int x);
	deque<Cell*> getCellPath(int z, int x);

	// Mutators
	void setTile(int z, int x, int tileType);
	void fillPathLength();
	void fillPaths();
	void clearPaths();

	// Debugging info
	void printArray();
	void printPaths();

private:
	// This function is private to prevent accessing the right cell easily
	// Because we are using a vector to represent a 2D array, there must be a bit of math
	// to translate from two numbers to one
	int getIndex(int z, int x) { return x + m_height * z; }

	int m_width;
	int m_height;
	Cell start;

	// Dynamic arrays for expandable restaurants
	vector<int> floor;				// Represents the actual floor of the restaurant
	vector<int> pathLength;			// Represents the actual floor of the restaurant
	vector<deque<Cell*>> paths;		// Holds the shortest distance path for tables only
};