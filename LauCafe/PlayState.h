////////////////////////////////////////
// PlayState.h
////////////////////////////////////////

#ifndef PLAYSTATE_H_INCLUDED
#define PLAYSTATE_H_INCLUDED

#include "Main.h"
#include "GameState.h"
#include "Camera.h"

class Camera;

class PlayState : public GameState
{
public:
	PlayState(GLFWwindow* window);
	
	int Initialize();
	int InitGL();

	void Input();
	void Update();
	void Draw();

	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void CharCallback(GLFWwindow* window, unsigned int code);
	void MouseButton(GLFWwindow* window, int button, int action, int mods);
	void MouseMotion(GLFWwindow* window, double xpos, double ypos);
	void MouseScroll(GLFWwindow* window, double xoffset, double yoffset);
private:
	// Window management
	float ratio;
	int WindowHandle;
	int WinX, WinY;

	// Input
	bool LeftDown, MiddleDown, RightDown, LeftDownTwo, BothDown;
	double MouseX, MouseY;
};

////////////////////////////////////////////////////////////////////////////////

#endif