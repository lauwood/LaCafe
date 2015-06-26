////////////////////////////////////////
// StateManager.cpp
////////////////////////////////////////

#include "StateManager.h"

static StateManager* StateMan;

////////////////////////////////////////////////////////////////////////////////

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)				{ glViewport(0, 0, width, height); }
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)		{ StateMan->KeyCallback(window, key, scancode, action, mods); }

////////////////////////////////////////////////////////////////////////////////

void StateManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_F1:
			if (b_EditMode) {
				// delete state edit mode
				PopState();
				PeekState()->Resume();
				b_EditMode = false;
			} else {
				PeekState()->Pause();
				EditState *State_EditMode= new EditState(window);
				State_EditMode->SetSquarePath(((PlayState*)PeekState())->GetSquarePath());
				State_EditMode->SetCamera(PeekState()->GetCamera());
				PushState(State_EditMode);
				b_EditMode = true;
			}
			fprintf(stdout, "F1 pressed");
			break;
		case GLFW_KEY_F2:
			fprintf(stdout, "F2 pressed");
			break;
		}
	}
}

StateManager::StateManager(GLFWwindow* window) {
	b_EditMode = false;

	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);
	glfwSetKeyCallback(window, key_callback);

	SetWindow(window);
	PlayState *state = new PlayState(GetWindow());
	PushState(state);
	StateMan = this;
}


////////////////////////////////////////////////////////////////////////////////

void StateManager::GameLoop() {
	while (!glfwWindowShouldClose(GetWindow())) {
		Input();
		Update();
		Draw();
	}
}

////////////////////////////////////////////////////////////////////////////////

void StateManager::Quit() {
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////