#ifndef MAIN_H
#define MAIN_H

////////////////////////////////////////////////////////////////////////////////
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#define WINDOWTITLE	"LauCafe"

#define INIT_OK 1;
#define INIT_FAIL 0;

#define PI	3.14159f

#define MOUSE_LEFT        1	//0001
#define MOUSE_MIDDLE      2	//0010
#define MOUSE_RIGHT       4	//0100

const int WinX = 1024;
const int WinY = 768;

#define NUM_OF_SQUARES 100
const float SquareRadius = 1.0f;

#endif
