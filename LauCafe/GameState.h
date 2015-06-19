////////////////////////////////////////
// GameState.h
////////////////////////////////////////

#ifndef GAMESTATE_H_INCLUDED
#define GAMESTATE_H_INCLUDED

#include "Main.h"

class GameState {
public:
	virtual int Initialize() = 0;
	
	/*virtual void Pause() = 0;
	virtual void Resume() = 0;*/

	virtual void Input() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	virtual void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;
	virtual void CharCallback(GLFWwindow* window, unsigned int code) = 0;
	virtual void MouseButton(GLFWwindow* window, int button, int action, int mods) = 0;
	virtual void MouseMotion(GLFWwindow* window, double xpos, double ypos) = 0;
	virtual void MouseScroll(GLFWwindow* window, double xoffset, double yoffset) = 0;

protected:
	GameState(GLFWwindow* w) { window = w; }
	GLFWwindow* window;
};

#endif