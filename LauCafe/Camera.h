#ifndef CAMERA_H
#define CAMERA_H

#include "Main.h"

using namespace glm;

class Camera {
public:
	Camera();

	mat4 SetPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);

	mat4 GetRotationMatrix();

	mat4 GetProjectionMatrix() { return ProjectionMatrix; }
	mat4 GetViewMatrix();

	void SetPosition(vec3 position)	{ Position = position; }
	vec3 GetPosition()	{ return Position; }

	vec3 GetView();
	vec3 GetUp();
	vec3 GetRight();

	float GetYaw() { return Yaw; }
	void SetYaw(float yaw)	{ Yaw = yaw; }

	float GetPitch() { return Pitch; }
	void SetPitch(float pitch) { Pitch = pitch; }

	void SetSpeed(double speed) { Speed = speed; }
	double GetSpeed() { return Speed; }

	void SetRotationSpeed(double speed) { RotationSpeed = speed; }
	double GetRotationSpeed() { return RotationSpeed; }

	void SetViewByMouse(float mouseX, float mouseY);

	void PositionCamera(float positionX, float positionY, float positionZ, float yaw, float pitch);

	void MoveForward(float speed);

	void MoveBackward(float speed);

	void MoveLeft(float speed);

	void MoveRight(float speed);

	void MoveUp(float speed);

	void MoveDown(float speed);

protected:
	mat4 ProjectionMatrix;								// The camera's projection matrix
	vec3 Position;										// The camera's position

	float Speed = 0.01;									// The camera's speed that is used with a time slice
	float RotationSpeed = 2;							// The rotation speed for the camera
	double MouseSpeed = 0.05;							// The speed for the mouse rotations
	float Yaw = 0;										// The horizontal rotation angle (in radians), y-axis
	float Pitch = 0;									// The vertical rotation angle (in radians, x-axis
};

#endif