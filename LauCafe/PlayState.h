////////////////////////////////////////
// PlayState.h
////////////////////////////////////////

#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#include "Main.h"
#include "GameState.h"
#include "EditState.h"
#include "Camera.h"
#include "Model.h"
#include "Floor.h"
#include "CubeMap.h"
#include "Raycast.h"
#include <vector>
#include "Area.h"
#include "Patron.h"
#include "Employee.h"
#include "GameObjectTable.h"
#include "GameObjectChair.h"
#include "GameObjectStove.h"
#include "GameObjectPodium.h"

class PlayState : public GameState
{
public:
	PlayState(GLFWwindow* window, Area* area);
	
	int Initialize();

	void Pause()		{ StateRunning = false; }
	void Resume()		{ StateRunning = true; }

	void Input();
	void Update();
	void Draw();

	std::vector<Model>* GetSquarePath() { return g_SquarePath; }
	void SetSquarePath(std::vector<Model> *pSquarePath) { g_SquarePath = pSquarePath; }
private:
	bool StateRunning;

	int SelectedSquare = -1;

	// Input
	double MouseX, MouseY;

	CubeMap Skybox;
	
	Model g_Axis;
	Floor g_Floor;
	Mesh Dude;
	GameObjectPodium g_Podium;
	
	std::vector<GameObjectChair> g_Chairs;
	std::vector<GameObjectTable> g_Tables;
	std::vector<GameObjectStove> g_Stoves;
	std::vector<Model> *g_SquarePath;

	std::vector<Patron*> g_Patron;
	Employee g_Receptionist;

	Area* a;
};

////////////////////////////////////////////////////////////////////////////////

#endif