#version 400

in vec3 vp;
uniform mat4 projectionMatrix, viewMatrix;
out vec3 texcoords;

void main () {
	texcoords = vp;
	gl_Position = projectionMatrix * viewMatrix * vec4 (vp, 1.0);
}