#include "Raycast.h"

bool RayIntersect(vec3 ray_origin_wor, vec3 ray_direction_wor, vec3 object_center_wor, float object_radius, float* intersection_distance) {
	vec3 distance_to_object = ray_origin_wor - object_center_wor;
	float b = dot(ray_direction_wor, distance_to_object);
	float c = dot(distance_to_object, distance_to_object) - object_radius * object_radius;
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
vec3 GetRayFromMouse(float mouse_x, float mouse_y, float ScreenWidth, float ScreenHeight, Camera *pCamera) {
	// screen space (viewport coordinates)
	float x = (2.0f * mouse_x) / ScreenWidth - 1.0f;
	float y = 1.0f - (2.0f * mouse_y) / ScreenHeight;
	float z = 1.0f;
	// normalised device space
	vec3 ray_nds = vec3(x, y, z);
	// clip space
	vec4 ray_clip = vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);
	// eye space
	vec4 ray_eye = inverse(pCamera->GetProjectionMatrix()) * ray_clip;
	ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
	// world space
	vec3 ray_wor = vec3(inverse(pCamera->GetViewMatrix()) * ray_eye);
	// don't forget to normalise the vector at some point
	ray_wor = normalize(ray_wor);
	return ray_wor;
}