////////////////////////////////////////
// core.cpp
////////////////////////////////////////

#include "Main.h"
#include "GameEngine.h"

////////////////////////////////////////////////////////////////////////////////

static GameEngine* Game;

////////////////////////////////////////////////////////////////////////////////

static void error_callback(int error, const char* description)									{ fputs(description, stderr); }

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv) {
	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	Game = new GameEngine(argc, argv);	//initialize
	if (Game->PeekState()->Initialize() != 0) {
		fprintf(stderr, "Failed to initialize state\n");
		return -1;
	}

	while (!glfwWindowShouldClose(Game->Window())) {
		Game->Input();
		Game->Update();
		Game->Draw();
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////

void drawAxis(float size) {
	glBegin(GL_LINES);
		glColor4f(1.0f,0.0f,0.0f,1.0f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(size,0.0f,0.0f);

		glColor4f(0.0f,1.0f,0.0f,1.0f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(0.0f,size,0.0f);

		glColor4f(0.0f,0.0f,1.0f,1.0f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(0.0f,0.0f,size);

		glColor4f(1.0f,1.0f,1.0f,1.0f);
	glEnd();
}

////////////////////////////////////////////////////////////////////////////////

void drawWireBox(float xmin,float ymin,float zmin,float xmax,float ymax,float zmax) {
	glPushMatrix();
		glTranslatef(0.5f*(xmin+xmax),0.5f*(ymin+ymax),0.5f*(zmin+zmax));
		glScalef(xmax-xmin,ymax-ymin,zmax-zmin);
		glColor4f(1.0f,1.0f,1.0f,1.0f);
		//glutWireCube(1.0f);
	glPopMatrix();
}

////////////////////////////////////////////////////////////////////////////////

float toRadians(float d) {
	return d * M_PI / 180;
}

float toDegrees(float r) {
	return r * 180 / M_PI;
}

float lerp(float x, float y, float a) {
	return x*(1.0f - a) + y*a;
}

float saturate(float x) {
	if (x < 0) {
		return 0;
	} else if (x > 1) {
		return 1;
	} else {
		return x;
	}
}