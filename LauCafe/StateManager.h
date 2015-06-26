////////////////////////////////////////
// StateManager.h
////////////////////////////////////////

#ifndef STATE_MAN_H
#define GSTATE_MAN_H

#include "Main.h"
#include "GameState.h"
#include "PlayState.h"
#include "EditState.h"
#include <stack>

class StateManager {
public:
	StateManager(GLFWwindow* window);
	void Initialize();

	// States take care of what the current state the game is in
	void PushState(GameState *state)		{ states.push(state); }
	void PopState()							{ states.pop(); }
	GameState* PeekState()					{ return states.top(); }

	void GameLoop();
	void Input()							{ PeekState()->Input(); }
	void Update()							{ PeekState()->Update(); }
	void Draw()								{ PeekState()->Draw(); }

	bool Running()							{ return m_running; }
	void Quit();

	GLFWwindow* GetWindow()					{ return window; }
	void SetWindow(GLFWwindow* pWindow)		{ window = pWindow; }

	void MouseButtonCallback(GLFWwindow*, int button, int action, int mods);
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	bool b_EditMode;
private:
	int MouseActiveButton = 0;
	GLFWwindow* window;
	bool m_running;
	std::stack<GameState*> states;
};

////////////////////////////////////////////////////////////////////////////////

#endif
