////////////////////////////////////////
// PlayState.cpp
////////////////////////////////////////

#include "PlayState.h"
#include <cstdio>

#define LEFT        1	//0001
#define MIDDLE      2	//0010
#define RIGHT       4	//0100

const int ScreenWidth = 1024;
const int ScreenHeight = 768;

////////////////////////////////////////////////////////////////////////////////

PlayState::PlayState(GLFWwindow* window) : GameState(window) {
	Initialize();
}

////////////////////////////////////////////////////////////////////////////////

int PlayState::Initialize() {
	glEnable(GL_DEPTH_TEST);

	MouseActiveButton = 0;

	m_Camera = new Camera(); 
	m_Camera->SetPerspective(glm::radians(60.0f), ScreenWidth / (float)ScreenHeight, 0.01f, 1000);
	//					     Position	  Yaw	 Pitch
	m_Camera->PositionCamera(0, 1, 5,     0,     0);

	Skybox.Initialize();
	std::string cmRelPath = "CubeMap/Yokohama3/";
	Skybox.CreateCubeMap((cmRelPath + "negz.jpg").c_str(),  // front
						 (cmRelPath + "posz.jpg").c_str(),  // back
						 (cmRelPath + "posy.jpg").c_str(),  // top
						 (cmRelPath + "negy.jpg").c_str(),  // bottom
						 (cmRelPath + "negx.jpg").c_str(),  // left
						 (cmRelPath + "posx.jpg").c_str()); // right
	Skybox.SetCamera(m_Camera);

	g_Axis.Initialize(Model::axis, 6, GL_LINES, "Shaders/Shader_vs.glsl", "Shaders/Shader_fs.glsl");
	g_Axis.SetCamera(m_Camera); 
	g_Axis.SetPosition(vec3(0, 0, 0));

	g_SampleSquare.Initialize(Model::square, 6, GL_TRIANGLES, "Shaders/Shader_vs.glsl", "Shaders/Shader_fs.glsl");
	g_SampleSquare.SetCamera(m_Camera);
	g_SampleSquare.SetPosition(vec3(3, 0, 0));

	g_SampleSquare2.Initialize(Model::square2, 6, GL_TRIANGLES, "Shaders/Shader_vs.glsl", "Shaders/Shader_fs.glsl");
	g_SampleSquare2.SetCamera(m_Camera);
	g_SampleSquare2.SetPosition(vec3(3, 0, 0));
	
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // set counter-clock-wise vertex order to mean the front
	glClearColor(0.2, 0.2, 0.2, 1.0); // grey background to help spot mistakes
	return 1; // OK
}

////////////////////////////////////////////////////////////////////////////////

void PlayState::Input() {
	// Keyboard
	if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (glfwGetKey(window, GLFW_KEY_W)) {
		m_Camera->MoveForward(m_Camera->GetSpeed());
	}
	if (glfwGetKey(window, GLFW_KEY_A)) {
		m_Camera->MoveLeft(m_Camera->GetSpeed());
	}
	if (glfwGetKey(window, GLFW_KEY_S)) {
		m_Camera->MoveBackward(m_Camera->GetSpeed());
	}
	if (glfwGetKey(window, GLFW_KEY_D)) {
		m_Camera->MoveRight(m_Camera->GetSpeed());
	}
	if (glfwGetKey(window, GLFW_KEY_UP)) {
		m_Camera->MoveUp(m_Camera->GetSpeed());
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN)) {
		m_Camera->MoveDown(m_Camera->GetSpeed());
	}

	// Mouse buttons
	int button = 0;
	int action = glfwGetMouseButton(window, button);

	int b;

	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		b = LEFT;
		break;

	case GLFW_MOUSE_BUTTON_MIDDLE:
		b = MIDDLE;
		break;

	case GLFW_MOUSE_BUTTON_RIGHT:
		b = RIGHT;
		break;

	default:
		b = 0;
	}

	if (action == GLFW_PRESS) {
		MouseActiveButton |= b;
	}
	else {
		MouseActiveButton &= ~b;
	}

	// Mouse motions

	double nx, ny;
	glfwGetCursorPos(window, &nx, &ny);
	double dx = nx - MouseX;
	double dy = ny - MouseY;

	MouseX = nx;
	MouseY = ny;

	if (MouseActiveButton & LEFT) {
		if (dx != 0 && dy != 0)
		{
			m_Camera->SetViewByMouse(dx, dy);
		}
	}

	glfwPollEvents();
}
////////////////////////////////////////////////////////////////////////////////

void PlayState::Update() {
}

////////////////////////////////////////////////////////////////////////////////

void PlayState::Draw() {
	// timers
	static double previous_seconds = glfwGetTime();
	double current_seconds = glfwGetTime();
	double elapsed_seconds = current_seconds - previous_seconds;
	previous_seconds = current_seconds;
	UpdateFPSCounter(window);

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Skybox.Render();

	g_Axis.Render();
	g_SampleSquare.Render();
	g_SampleSquare2.Render();

	glfwSwapBuffers(window);
	glfwPollEvents();
}

////////////////////////////////////////////////////////////////////////////////