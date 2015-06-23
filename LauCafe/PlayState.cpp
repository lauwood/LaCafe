////////////////////////////////////////
// PlayState.cpp
////////////////////////////////////////

#include "PlayState.h"
#include "Area.h"
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
	glClearColor(1.0, 1.0, 1.0, 1.0);

	MouseActiveButton = 0;

	m_Camera = new Camera(); 
	m_Camera->SetPerspective(glm::radians(60.0f), ScreenWidth / (float)ScreenHeight, 0.01f, 1000);
	//					     Position	  Yaw	 Pitch
	m_Camera->PositionCamera(0, 1, 5,     0,     0);

	g_Axis.Initialize(Model::axis, 6, GL_LINES, "Shaders/Shader.vertex", "Shaders/Shader.fragment");
	g_Axis.SetCamera(m_Camera); 
	g_Axis.SetPosition(vec3(0, 0, 0));

	g_SampleSquare.Initialize(Model::square, 6, GL_TRIANGLES, "Shaders/Shader.vertex", "Shaders/Shader.fragment");
	g_SampleSquare.SetCamera(m_Camera);
	g_SampleSquare.SetPosition(vec3(3, 0, 0));

	g_SampleSquare2.Initialize(Model::square2, 6, GL_TRIANGLES, "Shaders/Shader.vertex", "Shaders/Shader.fragment");
	g_SampleSquare2.SetCamera(m_Camera);
	g_SampleSquare2.SetPosition(vec3(3, 0, 0));

	g_SampleSquare3 = std::vector<Model>(100);
	for (int i = 0; i < g_SampleSquare3.size(); i++) {
		g_SampleSquare3[i].Initialize(Model::square, 6, GL_TRIANGLES, "Shaders/Shader.vertex", "Shaders/Shader.fragment");
		g_SampleSquare3[i].SetCamera(m_Camera);
		g_SampleSquare3[i].SetPosition(vec3(i%10, i/10, 0));
		g_SampleSquare3[i].SetScale(vec3(0.5, 0.5, 0.5));
	}
	
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
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*
	g_Axis.Render();
	g_SampleSquare.Render();
	g_SampleSquare2.Render();
	*/

	Area a(10, 10, 1, 1);
	a.fillPaths();
	vector<int> v(100);
	a.getPathMap(v);

	for (int i = 0; i < v.size(); i++) {
		if (v[i] == 7) {
			g_SampleSquare3[i].Initialize(Model::square2, 6, GL_TRIANGLES, "Shaders/Shader.vertex", "Shaders/Shader.fragment");
			g_SampleSquare3[i].SetCamera(m_Camera);
			g_SampleSquare3[i].SetPosition(vec3(i % 10, i / 10, 0));
			g_SampleSquare3[i].SetScale(vec3(0.5, 0.5, 0.5));
		}
	}
		
	for (int i = 0; i < g_SampleSquare3.size(); i++)
		g_SampleSquare3[i].Render();

	glfwSwapBuffers(window);
	glfwPollEvents();
}

////////////////////////////////////////////////////////////////////////////////