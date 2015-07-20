////////////////////////////////////////
// EditState.h
////////////////////////////////////////

#ifndef EDITSTATE_H
#define EDITSTATE_H

#include "Main.h"
#include "Camera.h"
#include "GameState.h"
#include "Model.h"
#include "Restaurant.h"
#include <vector>

class EditState : public GameState
{
public:
	EditState(GLFWwindow* window, Restaurant* area);

	int Initialize();

	void Pause()	{} // Empty
	void Resume()	{} // because edit doesn't pause or resume

	void Input();
	void Update();
	void Draw();

	std::vector<Model>* GetSquarePath() { return g_SquarePath; }
	void SetSquarePath(std::vector<Model> *pSquarePath) { g_SquarePath = pSquarePath; }
private:
	Restaurant* a;
	int SelectedSquare = -1;
	std::vector<Model> *g_SquarePath;

	TileType selectedItem;
};

#endif