#include "Floor.h"

Floor::Floor() {
	x = 100;
	y = 100;
	Vertex3 FloorCoordinates[6] = {
		vec3(-x, 0.0,  y), vec4(1, 1, 0, 1),
		vec3(-x, 0.0, -y), vec4(1, 1, 0, 1),
		vec3( x, 0.0, -y), vec4(0, 0, 1, 1),
		vec3( x, 0.0, -y), vec4(0, 0, 1, 1),
		vec3( x, 0.0,  y), vec4(1, 1, 0, 1),
		vec3(-x, 0.0,  y), vec4(1, 1, 0, 1) 
	};

	Initialize(FloorCoordinates, 6, GL_TRIANGLES, "Shaders/Shader_vs.glsl", "Shaders/Shader_fs.glsl");
};