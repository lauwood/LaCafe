////////////////////////////////////////
// GameState.h
////////////////////////////////////////

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Main.h"
#include "Camera.h"

class StateManager;

class GameState {
public:
	virtual int Initialize() = 0;
	
	virtual void Pause() = 0;
	virtual void Resume() = 0;

	virtual void Input() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	Camera* GetCamera() { return m_Camera; }
	void SetCamera(Camera *pCamera) { m_Camera = pCamera; }
	
	void SetMouseActiveButton(int pMouseActiveButton) { MouseActiveButton = pMouseActiveButton; }
protected:
	int MouseActiveButton = 0;
	GameState(GLFWwindow* w) { window = w; }
	GLFWwindow* window;

	Camera* m_Camera;
};

#endif