////////////////////////////////////////
// EditState.cpp
////////////////////////////////////////

#include "EditState.h"
#include "StateManager.h"
#include "Globals.h"

EditState::EditState(GLFWwindow* window, Restaurant* area) : GameState(window) {
	a = area;
	Initialize();

	selectedItem = WALKABLE;	// Default is "nothing"
}

int EditState::Initialize() {
	return INIT_OK;
}

void EditState::Input() {
	// Change to something else when UI is finished
	selectedItem = OBSTACLE;

	// Raycast
	if (MouseActiveButton) {
		double nx, ny;
		glfwGetCursorPos(window, &nx, &ny);

		vec3 ray_wor = GetRayFromMouse((float)nx, (float)ny, WinX, WinY);
		int closest_square_clicked = -1;
		float closest_intersection = 0.0f;
		for (int i = 0; i < NUM_OF_SQUARES; i++) {
			float t_dist = 0.0f;
			if (RayIntersect(Globals::Camera.GetPosition(), ray_wor, g_SquarePath->at(i).GetPosition(), SquareRadius, &t_dist)) {
				// if more than one sphere is in path of ray, only use the closest one
				if (-1 == closest_square_clicked || t_dist < closest_intersection) {
					closest_square_clicked = i;
					closest_intersection = t_dist;
				}
			}
		} // endfor
		SelectedSquare = closest_square_clicked;
		if (MouseActiveButton & MOUSE_LEFT) {
			if (SelectedSquare != -1 && selectedItem != WALKABLE) { // Don't want to delete things with left click
				g_SquarePath->at(SelectedSquare).Obstacle();
				a->setTile(closest_square_clicked / 10, closest_square_clicked % 10, selectedItem);
			}
		}
		else if (MouseActiveButton & MOUSE_RIGHT) {
			if (SelectedSquare != -1) {
				g_SquarePath->at(SelectedSquare).Unobstacle();
				a->setTile(closest_square_clicked / 10, closest_square_clicked % 10, WALKABLE);
				delete a->g_GameObjects.at(closest_square_clicked / 10 * a->getHeight() + closest_square_clicked % 10);
			}
		}
	}
	glfwPollEvents();
}

void EditState::Update() {

}

void EditState::Draw() {
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	for (int i = 0; i < NUM_OF_SQUARES; i++) {
		if (SelectedSquare == i) {
			g_SquarePath->at(i).Select();
		}
		else {
			g_SquarePath->at(i).Unselect();
		}
		g_SquarePath->at(i).Render();
	}

	for (GameObject *g : a->g_GameObjects) {
		if (g != NULL)
			g->Render();
	}
}