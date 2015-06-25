////////////////////////////////////////
// PlayState.cpp
////////////////////////////////////////

#include "PlayState.h"
#include "MeshLoader.h"
#include <cstdio>

#define LEFT        1	//0001
#define MIDDLE      2	//0010
#define RIGHT       4	//0100

const int ScreenWidth = 1024;
const int ScreenHeight = 768;

#define NUM_OF_SQUARES 100
#define Y_OFFSET 0.5

Mesh banana;

const float square_radius = 1.0f;
int g_selected_square = -1;

////////////////////////////////////////////////////////////////////////////////

PlayState::PlayState(GLFWwindow* window) : GameState(window) {

	a = Area(10, 10, 0, 0);
	a.setTile(1, 0, 2);
	a.fillPaths();
	Initialize();
}

////////////////////////////////////////////////////////////////////////////////

int PlayState::Initialize() {
	glEnable(GL_DEPTH_TEST);

	MouseActiveButton = 0;

	m_Camera = new Camera(); 
	m_Camera->SetPerspective(glm::radians(60.0f), ScreenWidth / (float)ScreenHeight, 0.01f, 1000);
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

	g_SquarePath = std::vector<Model>(NUM_OF_SQUARES);
	for (int i = 0; i < NUM_OF_SQUARES; i++) {
		g_SquarePath[i].Initialize(Model::square2, 6, GL_TRIANGLES, "Shaders/Shader_vs.glsl", "Shaders/Shader_fs.glsl");
		g_SquarePath[i].SetCamera(m_Camera);
		g_SquarePath[i].SetPosition(vec3(i /10, Y_OFFSET, i % 10));
		g_SquarePath[i].SetScale(vec3(0.5, 0.5, 0.5));
	}
	
	return 1; // OK
}

////////////////////////////////////////////////////////////////////////////////

bool PlayState::RayIntersect(glm::vec3 ray_origin_wor, glm::vec3 ray_direction_wor, glm::vec3 object_center_wor, float object_radius, float* intersection_distance) {
	glm::vec3 distance_to_object = ray_origin_wor - object_center_wor;
	float b = glm::dot(ray_direction_wor, distance_to_object);
	float c = glm::dot(distance_to_object, distance_to_object) - object_radius * object_radius;
	float b_squared_minus_c = b * b - c;
	if (b_squared_minus_c < 0.0f) {
		return false;
	}
	// check for ray hitting twice (in and out of the sphere)
	if (b_squared_minus_c > 0.0f) {
		// get the 2 intersection distances along ray
		float t_a = -b + sqrt(b_squared_minus_c);
		float t_b = -b - sqrt(b_squared_minus_c);
		*intersection_distance = t_b;
		// if behind viewer, throw one or both away
		if (t_a < 0.0) {
			if (t_b < 0.0) {
				return false;
			}
		}
		else if (t_b < 0.0) {
			*intersection_distance = t_a;
		}

		return true;
	}
	// check for ray hitting once (skimming the surface)
	if (0.0f == b_squared_minus_c) {
		// if behind viewer, throw away
		float t = -b + sqrt(b_squared_minus_c);
		if (t < 0.0f) {
			return false;
		}
		*intersection_distance = t;
		return true;
	}
	// note: could also check if ray origin is inside sphere radius
	return false;
}

////////////////////////////////////////////////////////////////////////////////

/* takes mouse position on screen and return ray in world coords */
glm::vec3 PlayState::GetRayFromMouse(float mouse_x, float mouse_y) {
	// screen space (viewport coordinates)
	float x = (2.0f * mouse_x) / ScreenWidth - 1.0f;
	float y = 1.0f - (2.0f * mouse_y) / ScreenHeight;
	float z = 1.0f;
	// normalised device space
	vec3 ray_nds = vec3(x, y, z);
	// clip space
	vec4 ray_clip = vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);
	// eye space
	vec4 ray_eye = inverse(m_Camera->GetProjectionMatrix()) * ray_clip;
	ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
	// world space
	vec3 ray_wor = vec3(inverse(m_Camera->GetViewMatrix()) * ray_eye);
	// don't forget to normalise the vector at some point
	ray_wor = normalize(ray_wor);
	return ray_wor;
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

	if (glfwGetKey(window, GLFW_KEY_R)) {
		a.setTile(1, 0, 2);
		a.fillPaths();
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
		vec3 ray_wor = GetRayFromMouse((float)nx, (float)ny);
		int closest_square_clicked = -1;
		float closest_intersection = 0.0f;
		for (int i = 0; i < NUM_OF_SQUARES; i++) {
			float t_dist = 0.0f;
			if (RayIntersect(m_Camera->GetPosition(), ray_wor, g_SquarePath[i].GetPosition(), square_radius, &t_dist
				)) {
				// if more than one sphere is in path of ray, only use the closest one
				if (-1 == closest_square_clicked || t_dist < closest_intersection) {
					closest_square_clicked = i;
					closest_intersection = t_dist;
					a.setTile(closest_square_clicked / 10-1, closest_square_clicked % 10-1, 2);
					a.fillPaths();
				}
			}
		} // endfor
		fprintf(stdout, "%d square selected\n", closest_square_clicked);
		fprintf(stdout, "%d z, %d x\n", closest_square_clicked / 10, closest_square_clicked % 10);
		g_selected_square = closest_square_clicked;
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

	glDisable(GL_CULL_FACE);
	g_Floor.Render();

	//banana.Render();
	g_Axis.Render();

	vector<int> pathMap(a.getHeight() * a.getWidth());

	for (int z = 0; z < a.getHeight(); z++)
		for (int x = 0; x < a.getWidth(); x++) {
			if (a.getTileType(z, x) == 2) {
				// If the cell is a potential destination, print the path
				deque<Cell*> p = a.getCellPath(z, x);
				for (size_t i = 0; i < p.size(); i++) {
					Cell* top = a.getCellPath(z, x).at(i);
					// 7 is just a visual symbol to represent the path
					pathMap.at(top->x + a.getHeight() * top->z) = 7;
					//cout << "x" << top->x << " z" << top->z << endl;
				}
			}
		}

	for (int i = 0; i < pathMap.size(); i++) {
		if (pathMap[i] == 7) {
			g_SquarePath[i].Path(); // uses a shader to recolor found
		}
		else {
			g_SquarePath[i].Unpath();
		}
	}
		
	for (int i = 0; i < NUM_OF_SQUARES; i++) {
		if (g_selected_square == i) {
			g_SquarePath[i].Select();
		}
		else {
			g_SquarePath[i].Unselect();
		}
		g_SquarePath[i].Render();
	}

	glfwSwapBuffers(window);
	glfwPollEvents();
}

////////////////////////////////////////////////////////////////////////////////