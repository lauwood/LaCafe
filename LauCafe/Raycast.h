#ifndef RAYCAST_H
#define RAYCAST_H

#include "Main.h"
#include "Camera.h"

using namespace glm;

bool RayIntersect(vec3 ray_origin_wor, vec3 ray_direction_wor, vec3 object_center_wor, float object_radius, float* intersection_distance);
vec3 GetRayFromMouse(float mouse_x, float mouse_y, float ScreenWidth, float ScreenHeight);

#endif