////////////////////////////////////////
// GameState.h
////////////////////////////////////////

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Main.h"

class GameState {
public:
	virtual int Initialize() = 0;
	
	/*virtual void Pause() = 0;
	virtual void Resume() = 0;*/

	virtual void Input() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

protected:
	GameState(GLFWwindow* w) { window = w; }
	GLFWwindow* window;
};

#endif