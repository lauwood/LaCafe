////////////////////////////////////////
// PlayState.h
////////////////////////////////////////

#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#include "Main.h"
#include "GameState.h"
#include "Camera.h"
#include "Model.h"
#include "Floor.h"
#include "CubeMap.h"
#include <vector>
#include "Area.h"

#define LEFT        1
#define MIDDLE      2
#define RIGHT       3

class PlayState : public GameState
{
public:
	PlayState(GLFWwindow* window);
	
	int Initialize();

	void Input();
	void Update();
	void Draw();
private:
	bool RayIntersect(glm::vec3, glm::vec3, glm::vec3, float, float*);
	glm::vec3 GetRayFromMouse(float, float);

	// Window management
	Camera* m_Camera;
	float ratio;
	int WinX, WinY;

	// Input
	double MouseX, MouseY;
	int MouseActiveButton;

	CubeMap Skybox;
	
	Model g_Axis;
	Floor g_Floor;
	
	std::vector<Model> g_SquarePath;

	Area a;
};

////////////////////////////////////////////////////////////////////////////////

#endif