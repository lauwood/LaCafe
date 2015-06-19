#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

////////////////////////////////////////////////////////////////////////////////
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <algorithm>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define WINDOWTITLE	"LauCafe"

#define M_PI	3.14159f
#define WIN_X	1024
#define WIN_Y	768

void drawAxis(float size);
void drawWireBox(float xmin, float ymin, float zmin, float xmax, float ymax, float zmax);

float toRadians(float d);
float toDegrees(float r);
float lerp(float x, float y, float a);
float saturate(float x);

#endif
