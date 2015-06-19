////////////////////////////////////////
// GameEngine.cpp
////////////////////////////////////////

#include "GameEngine.h"

static GameState* State;

////////////////////////////////////////////////////////////////////////////////

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)		{ State->KeyCallback(window, key, scancode, action, mods); }
void character_callback(GLFWwindow* window, unsigned int codepoint)						{ State->CharCallback(window, codepoint); }
void mouse_motion(GLFWwindow* window, double xpos, double ypos)							{ State->MouseMotion(window, xpos, ypos); }
void mouse_button(GLFWwindow* window, int button, int action, int mods)					{ State->MouseButton(window, button, action, mods); }
void mouse_scroll(GLFWwindow* window, double xoffset, double yoffset)					{ State->MouseScroll(window, xoffset, yoffset); }
void framebuffer_size_callback(GLFWwindow* window, int width, int height)				{ glViewport(0, 0, width, height); }

////////////////////////////////////////////////////////////////////////////////

GameEngine::GameEngine(int argc, char **argv) {
	// pass in glfwGetPrimaryMonitor() to first null for fullscreen
	window = glfwCreateWindow(1024, 768, WINDOWTITLE, NULL, NULL);
	//window = glfwCreateWindow(1920, 1080, WINDOWTITLE, glfwGetPrimaryMonitor(), NULL);

	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	PlayState *state = new PlayState(window);
	PushState(state);
	ChangeState(state);
	
	// Callbacks
	glfwSetKeyCallback(window, ::key_callback);
	glfwSetCharCallback(window, ::character_callback);
	glfwSetCursorPosCallback(window, ::mouse_motion);
	glfwSetMouseButtonCallback(window, ::mouse_button);
	glfwSetScrollCallback(window, ::mouse_scroll);
	glfwSetFramebufferSizeCallback(window, ::framebuffer_size_callback);
}

////////////////////////////////////////////////////////////////////////////////

void GameEngine::ChangeState(GameState* state) {
	State = state;
}

////////////////////////////////////////////////////////////////////////////////

void GameEngine::PushState(GameState* state) {
	states.push(state);
}

////////////////////////////////////////////////////////////////////////////////

void GameEngine::PopState() {
	states.pop();
}

////////////////////////////////////////////////////////////////////////////////

void GameEngine::Input() {
	State->Input();
}

////////////////////////////////////////////////////////////////////////////////

void GameEngine::Update() {
	State->Update();
}

////////////////////////////////////////////////////////////////////////////////

void GameEngine::Draw() {
	State->Draw();
}

////////////////////////////////////////////////////////////////////////////////

void GameEngine::Quit() {
	m_running = false;
	glFinish();
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////