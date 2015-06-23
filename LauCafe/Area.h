#pragma once

#include <stack>
#include <vector>
using namespace std;

class Area
{
public:
	Area(int z, int x, int sz, int sx);
	Area(int z, int x, int sz, int sx, vector<int> v);
	~Area();

	// Accessors
	bool isInBounds(int z, int x);
	bool isWalkable(int z, int x);
	int getIndex(int z, int x);

	// Mutators
	void setTile(int z, int x, int t);
	void fillPaths();
// Represents a single tile in the array
	struct Cell {
		int x;
		int z;
	};
	// Debugging info
	void printArray();
	void printPaths();
	void getPathMap(vector<int>& pathMap);
	vector<stack<Cell*>*> paths;	// Holds the shortest distance path for tables only

private:
	void getPathLengthGrid(vector<int>& pathLengthGrid);

	

	int m_width;
	int m_height;
	Cell start;

	// Dynamic arrays for expandable restaurants
	vector<int> floor;				// Represents the actual floor of the restaurant
};