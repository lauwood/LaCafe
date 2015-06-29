////////////////////////////////////////
// PlayState.cpp
////////////////////////////////////////

#include "PlayState.h"
#include "MeshLoader.h"
#include "StateManager.h"
#include <cstdio>

#define Y_OFFSET 0.5

////////////////////////////////////////////////////////////////////////////////

PlayState::PlayState(GLFWwindow* window, Area* area) : GameState(window) {
	a = area;
	a->fillPaths();
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

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // set counter-clock-wise vertex order to mean the front
	glClearColor(1.0, 1.0, 1.0, 1.0); // grey background to help spot mistakes

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

void PlayState::Input(double delta) {
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
		m_Camera->MoveForward(m_Camera->GetSpeed() * delta);
	}
	if (glfwGetKey(window, GLFW_KEY_A)) {
		m_Camera->MoveLeft(m_Camera->GetSpeed() * delta);
	}
	if (glfwGetKey(window, GLFW_KEY_S)) {
		m_Camera->MoveBackward(m_Camera->GetSpeed() * delta);
	}
	if (glfwGetKey(window, GLFW_KEY_D)) {
		m_Camera->MoveRight(m_Camera->GetSpeed() * delta);
	}
	if (glfwGetKey(window, GLFW_KEY_R)) {
		m_Camera->MoveUp(m_Camera->GetSpeed() * delta);
	}
	if (glfwGetKey(window, GLFW_KEY_F)) {
		m_Camera->MoveDown(m_Camera->GetSpeed() * delta);
	}
	if (glfwGetKey(window, GLFW_KEY_UP)) {
		m_Camera->SetPitch(m_Camera->GetPitch() + m_Camera->GetSpeed() * -1 * delta);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN)) {
		m_Camera->SetPitch(m_Camera->GetPitch() + m_Camera->GetSpeed() * delta);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT)) {
		m_Camera->SetYaw(m_Camera->GetYaw() + m_Camera->GetSpeed() * -1 * delta);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
		m_Camera->SetYaw(m_Camera->GetYaw() + m_Camera->GetSpeed() * delta);
	}

	// Mouse motions

	/* Find mouse displacement
	MouseX = nx;
	MouseY = ny;
	double dx = nx - MouseX;
	double dy = ny - MouseY;*/

	// Raycast
	if (MouseActiveButton & MOUSE_LEFT) {
		double nx, ny;

		glfwGetCursorPos(window, &nx, &ny);

		vec3 ray_wor = GetRayFromMouse((float)nx, (float)ny, WinX, WinY, m_Camera);

		int closest_square_clicked = -1;
		float closest_intersection = 0.0f;
		for (int i = 0; i < NUM_OF_SQUARES; i++) {
			float t_dist = 0.0f;
			if (RayIntersect(m_Camera->GetPosition(), ray_wor, g_SquarePath->at(i).GetPosition(), SquareRadius, &t_dist)) {
				// if more than one sphere is in path of ray, only use the closest one
				if (-1 == closest_square_clicked || t_dist < closest_intersection) {
					closest_square_clicked = i;
					closest_intersection = t_dist;

					// Clear the previous paths
					for (int k = 0; k < a->getHeight(); k++)
						for (int l = 0; l < a->getWidth(); l++) {
							if (a->getTileType(k, l) == TABLE)
								a->setTile(k, l, 0);
							g_SquarePath->at(l + a->getHeight() * k).Unpath();
						}

					// To only allow one destination
					if (a->getTileType(closest_square_clicked / 10, closest_square_clicked % 10) == WALKABLE)
						a->setTile(closest_square_clicked / 10, closest_square_clicked % 10, TABLE);

					a->fillPaths();

					Cell* c = nullptr;
					for (int z = 0; z < a->getHeight(); z++)
						for (int x = 0; x < a->getWidth(); x++)
							if (a->getTileType(z, x) == START) {
								c = new Cell;
								c->x = x;
								c->z = z;
							}

					if (c != nullptr) {
						for (int z = 0; z < a->getHeight(); z++)
							for (int x = 0; x < a->getWidth(); x++) {
								if (a->getTileType(z, x) == TABLE) {
									// If the cell is a potential destination, print the path
									deque<Cell*> p = a->getCellPath(c->z, c->x, z, x);
									for (size_t j = 0; j < p.size(); j++) {
										Cell* top = p.at(j);
										// 7 is just a visual symbol to represent the path
										//pathMap.at(top->x + a.getHeight() * top->z) = 7;
										g_SquarePath->at(top->x + a->getHeight() * top->z).Path();
									}
								}
							}
						delete c;
					}
				}
			}
		} // endfor
		SelectedSquare = closest_square_clicked;

		g_Patron.push_back(new Patron(a, m_Camera));
		//fprintf(stdout, "center of square: x: %.2f, z: %.2f", g_SquarePath->at(SelectedSquare).GetPosition().x, g_SquarePath->at(SelectedSquare).GetPosition().z);
	}

	glfwPollEvents();
}
////////////////////////////////////////////////////////////////////////////////

void PlayState::Update(double delta) { 
	for (int i = 0; i < g_Patron.size(); i++) {
		if (g_Patron.at(i)->isFinished()) {
			// Delete the patron now that it's done eating/waiting
			delete g_Patron.at(i);
			g_Patron.erase(g_Patron.begin() + i);
		}
		else {
			g_Patron.at(i)->update(delta);
		}
	}
	
}

////////////////////////////////////////////////////////////////////////////////

void PlayState::Draw() {
	// timers
	UpdateFPSCounter(window);

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Skybox.Render();

	glDisable(GL_CULL_FACE);
	g_Floor.Render();
	for (int i = 0; i < g_Patron.size(); i++)
		g_Patron.at(i)->Render();
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