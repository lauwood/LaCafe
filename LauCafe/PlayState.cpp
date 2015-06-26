////////////////////////////////////////
// PlayState.cpp
////////////////////////////////////////

#include "PlayState.h"
#include "MeshLoader.h"
#include "StateManager.h"
#include <cstdio>

#define LEFT        1	//0001
#define MIDDLE      2	//0010
#define RIGHT       4	//0100

#define Y_OFFSET 0.5

Mesh banana;

////////////////////////////////////////////////////////////////////////////////

PlayState::PlayState(GLFWwindow* window) : GameState(window) {
	a = Area(10, 10, 0, 0);
	a.fillPaths();
	Initialize();
}

////////////////////////////////////////////////////////////////////////////////

int PlayState::Initialize() {
	glEnable(GL_DEPTH_TEST);

	MouseActiveButton = 0;

	m_Camera = new Camera(); 
	m_Camera->SetPerspective(radians(60.0f), WinX / (float)WinY, 0.01f, 1000);
	//					     Position	  Yaw	 Pitch
	m_Camera->PositionCamera(1.06, 8, 4.41,     1.57,     1.25);

	Skybox.Initialize();
	std::string cmRelPath = "CubeMap/Yokohama3/";
	Skybox.CreateCubeMap((cmRelPath + "negz.jpg").c_str(),  // front
						 (cmRelPath + "posz.jpg").c_str(),  // back
						 (cmRelPath + "posy.jpg").c_str(),  // top
						 (cmRelPath + "negy.jpg").c_str(),  // bottom
						 (cmRelPath + "negx.jpg").c_str(),  // left
						 (cmRelPath + "posx.jpg").c_str()); // right
	Skybox.SetCamera(m_Camera);

	g_Floor.SetCamera(m_Camera);
	g_Floor.SetPosition(vec3(0, 0, 0));

	g_Axis.Initialize(Model::axis, 6, GL_LINES, "Shaders/Shader_vs.glsl", "Shaders/Shader_fs.glsl");
	g_Axis.SetCamera(m_Camera); 
	g_Axis.SetPosition(vec3(0, 0, 0));

	/*banana = Mesh("Models/banana.obj", "Shaders/Banana_vs.glsl", "Shaders/Banana_fs.glsl");
	banana.SetCamera(m_Camera);
	banana.SetPosition(vec3(0, 0, 0));
	banana.SetScale(vec3(0.01, 0.01, 0.01));*/
	
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // set counter-clock-wise vertex order to mean the front
	glClearColor(0.2, 0.2, 0.2, 1.0); // grey background to help spot mistakes

	g_SquarePath = new std::vector<Model>(NUM_OF_SQUARES);
	for (int i = 0; i < NUM_OF_SQUARES; i++) {
		g_SquarePath->at(i).Initialize(Model::square2, 6, GL_TRIANGLES, "Shaders/Shader_vs.glsl", "Shaders/Shader_fs.glsl");
		g_SquarePath->at(i).SetCamera(m_Camera);
		g_SquarePath->at(i).SetPosition(vec3(i / 10, Y_OFFSET, i % 10));
		g_SquarePath->at(i).SetScale(vec3(0.5, 0.5, 0.5));
	}

	return INIT_OK; // OK
}

////////////////////////////////////////////////////////////////////////////////

void PlayState::Input() {
	// Camera output
	/*fprintf(stdout, "Camera POSITION: (%.2f, %.2f, %.2f), YAW: %.2f, PITCH: %.2f\n",
		m_Camera->GetPosition().x,
		m_Camera->GetPosition().y,
		m_Camera->GetPosition().z, 
		m_Camera->GetYaw(),
		m_Camera->GetPitch());*/
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
	if (glfwGetKey(window, GLFW_KEY_R)) {
		m_Camera->MoveUp(m_Camera->GetSpeed());
	}
	if (glfwGetKey(window, GLFW_KEY_F)) {
		m_Camera->MoveDown(m_Camera->GetSpeed());
	}
	if (glfwGetKey(window, GLFW_KEY_UP)) {
		m_Camera->SetPitch(m_Camera->GetPitch() + m_Camera->GetSpeed() * -1);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN)) {
		m_Camera->SetPitch(m_Camera->GetPitch() + m_Camera->GetSpeed());
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT)) {
		m_Camera->SetYaw(m_Camera->GetYaw() + m_Camera->GetSpeed() * -1);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
		m_Camera->SetYaw(m_Camera->GetYaw() + m_Camera->GetSpeed());
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
			//m_Camera->SetViewByMouse(dx, dy); this method sucks
		}
	}

	// Raycast
	if (action == GLFW_PRESS) {
		vec3 ray_wor = GetRayFromMouse((float)nx, (float)ny, WinX, WinY, m_Camera);

		int closest_square_clicked = -1;
		float closest_intersection = 0.0f;
		for (int i = 0; i < NUM_OF_SQUARES; i++) {
			float t_dist = 0.0f;
			if (RayIntersect(m_Camera->GetPosition(), ray_wor, g_SquarePath->at(i).GetPosition(), SquareRadius, &t_dist)) {
				// if more than one sphere is in path of ray, only use the closest one
				if (-1 == closest_square_clicked || t_dist < closest_intersection) {

					a = Area(10, 10, 0, 0);
					a.fillPaths();
					vector<int> pathMap(a.getHeight() * a.getWidth());

					closest_square_clicked = i;
					closest_intersection = t_dist;

					a.setTile(closest_square_clicked / 10, closest_square_clicked % 10, 2);
					a.fillPaths();

					for (int z = 0; z < a.getHeight(); z++)
						for (int x = 0; x < a.getWidth(); x++) {
							if (a.getTileType(z, x) == 2) {
								// If the cell is a potential destination, print the path
								deque<Cell*> p = a.getCellPath(z, x);
								for (size_t j = 0; j < p.size(); j++) {
									Cell* top = a.getCellPath(z, x).at(j);
									// 7 is just a visual symbol to represent the path
									pathMap.at(top->x + a.getHeight() * top->z) = 7;
								}
							}
						}

					for (int i = 0; i < pathMap.size(); i++) {
						if (pathMap[i] == 7) {
							g_SquarePath->at(i).Path(); // uses a shader to recolor found
						}
						else {
							g_SquarePath->at(i).Unpath();
						}
					}
				}
			}
		} // endfor
		SelectedSquare = closest_square_clicked;
	}

	glfwPollEvents();
}
////////////////////////////////////////////////////////////////////////////////

void PlayState::Update() {
}

////////////////////////////////////////////////////////////////////////////////

void PlayState::Draw() {
	// timers
	UpdateFPSCounter(window);

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Skybox.Render();

	glDisable(GL_CULL_FACE);
	g_Floor.Render();

	//banana.Render();
	g_Axis.Render();
		
	for (int i = 0; i < NUM_OF_SQUARES; i++) {
		if (SelectedSquare == i) {
			g_SquarePath->at(i).Select();
		}
		else {
			g_SquarePath->at(i).Unselect();
		}
		g_SquarePath->at(i).Render();
	}

	glfwSwapBuffers(window);
}

////////////////////////////////////////////////////////////////////////////////