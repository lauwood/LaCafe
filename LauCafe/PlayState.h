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

class PlayState : public GameState
{
public:
	PlayState(GLFWwindow* window);
	
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
	int MouseActiveButton;

	CubeMap Skybox;
	
	Model g_Axis;
	Floor g_Floor;
	
	std::vector<Model> *g_SquarePath;

	Area a;
};

////////////////////////////////////////////////////////////////////////////////

#endif