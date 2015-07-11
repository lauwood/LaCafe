////////////////////////////////////////
// core.cpp
////////////////////////////////////////

#include "Main.h"
#include "StateManager.h"

////////////////////////////////////////////////////////////////////////////////

StateManager* Game;

////////////////////////////////////////////////////////////////////////////////

GLFWwindow* InitGL() {
	if (!glfwInit()) {
		fprintf(stdout, "Failed to initialize GLFW\n");
		return NULL;
	}

	GLFWwindow* window = glfwCreateWindow(WinX, WinY, WINDOWTITLE, NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		fprintf(stdout, "Failed to initialize window\n");
		return NULL;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	fprintf(stdout, "Renderer: %s\n", renderer);
	fprintf(stdout, "OpenGL version supported %s\n", version);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stdout, "Error: %s\n", glewGetErrorString(err));
		return NULL;
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	return window;
}

int main(int argc, char **argv) {
	GLFWwindow* Window = InitGL();
	if (!Window) {
		exit(EXIT_FAILURE);
	}

	StateManager* m_StateManager = new StateManager(Window);

	m_StateManager->GameLoop();

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
