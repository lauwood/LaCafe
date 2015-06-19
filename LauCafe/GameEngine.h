////////////////////////////////////////
// GameEngine.h
////////////////////////////////////////

#ifndef GAME_ENGINE_H_INCLUDED
#define GAME_ENGINE_H_INCLUDED

#include "Main.h"
#include "GameState.h"
#include "PlayState.h"
#include <thread>
#include <stack>

class GameEngine {
public:
	GameEngine(int argc, char **argv);
	void Initialize();

	// States take care of what the current state the game is in
	void ChangeState(GameState *state);
	void PushState(GameState *state);
	void PopState();
	GameState *PeekState() { return states.top(); }

	void Input();
	void Update();
	void Draw();

	bool Running()	{ return m_running; }
	void Quit();

	GLFWwindow* Window() { return window; }
private:
	GLFWwindow* window;
	bool m_running;
	std::stack<GameState*> states;
};

////////////////////////////////////////////////////////////////////////////////

#endif
