#include "Camera.h"
#include "TimeManager.h"

Camera::Camera()
{
	Position = vec3(0.0, 0.0, 0.0);
}

mat4 Camera::SetPerspective(float fov, float aspectRatio, float nearPlane, float farPlane)
{
	// Create the projection matrix with GLM's perspective function, replacing gluPerspective.
	ProjectionMatrix = perspective(fov, aspectRatio, nearPlane, farPlane);

	return ProjectionMatrix;
}

// This function sets the camera's position and rotation values
void Camera::PositionCamera(float positionX, float positionY, float positionZ, float yaw, float pitch)
{
	// Assign our camera variables
	Position = vec3(positionX, positionY, positionZ);
	Yaw = yaw;
	Pitch = pitch;
}

// This builds and returns a rotation matrix from the yaw and pitch rotations
mat4 Camera::GetRotationMatrix()
{
	// Create an identity matrix
	mat4 rotationMatrix(1.0f);

	// Add the Pitch rotation along the x-axis
	rotationMatrix = rotate(rotationMatrix, Pitch, vec3(1, 0, 0));

	// Add the Yaw rotation along the y-axis
	rotationMatrix = rotate(rotationMatrix, Yaw, vec3(0, 1, 0));

	// Return the final rotation matrix that stores our camera rotations
	return rotationMatrix;
}

mat4 Camera::GetRotationYMatrix()
{
	// Create an identity matrix
	mat4 rotationMatrix(1.0f);

	// Add the Yaw rotation along the y-axis
	rotationMatrix = rotate(rotationMatrix, Yaw, vec3(0, 1, 0));

	// Return the final rotation matrix that stores our camera rotations
	return rotationMatrix;
}

// This returns the camera's view matrix that is used to pass to our shaders
mat4 Camera::GetViewMatrix()
{
	// Return a view matrix by multiplying our rotation matrix by the inverse of a translation matrix
	return GetRotationMatrix() * inverse(translate(mat4(), Position));
}

// This returns the current view vector (the direction the camera is looking) from the rotation matrix
vec3 Camera::GetView()
{
	// Calculate the view vector by taking the inverse of our rotation matrix multiplied by a vector 
	// looking down the negative z-axis.
	vec4 viewVector = inverse(GetRotationMatrix()) * vec4(0, 0, -1, 1);

	// We are using a 4x4 matrix so our result is a vec4, but just turn it back into a vec3
	return vec3(viewVector);
}

// This returns the camera's right vector (the direction pointing perpendicular to the view and up vector)
vec3 Camera::GetUp()
{
	// Calculate the up vector by taking the inverse of the rotation matrix multiplied by a 
	// default up vector.
	vec4 upVector = inverse(GetRotationMatrix()) * vec4(0, 1, 0, 1);

	// We are using a 4x4 matrix so our result is a vec4, but just cast it back to a vec3
	return vec3(upVector);
}

// This returns the camera's right vector (the direction pointing perpendicular to the view and up vector)
vec3 Camera::GetRight()
{
	// Calculate the right vector by taking the inverse of the rotation matrix multiplied by a 
	// default right vector.
	vec4 rightVector = inverse(GetRotationMatrix()) * vec4(1, 0, 0, 1);

	// We are using a 4x4 matrix so our result is a vec4, but just cast it back to a vec3
	return vec3(rightVector);
}

// This will move the camera forward or backward depending on the speed
void Camera::MoveForward(float speed)
{
	speed *= TimeManager::Instance().DeltaTime;
	// Get our normalized view vector (The direction we are facing)
	vec3 viewVector = GetView();

	// Move our camera position forward or backward along the view vector
	Position.x += viewVector.x * speed;		// Add our acceleration to our position's X
	Position.z += viewVector.z * speed;		// Add our acceleration to our position's Z
}

void Camera::MoveBackward(float speed)
{
	speed *= TimeManager::Instance().DeltaTime;
	// Get our normalized view vector (The direction we are facing)
	vec3 viewVector = GetView();

	// Move our camera position forward or backward along the view vector
	Position.x -= viewVector.x * speed;		// Add our acceleration to our position's X
	Position.z -= viewVector.z * speed;		// Add our acceleration to our position's Z
}

// This will strafe the camera left or right depending on the speed
void Camera::MoveLeft(float speed)
{
	speed *= TimeManager::Instance().DeltaTime;
	// Get our normalized right vector (The direction perpendicular to our view)
	vec3 rightVector = GetRight();

	// Move our camera position right or left along the right vector
	Position.x -= rightVector.x * speed;		// Add our acceleration to our position's X
	Position.z -= rightVector.z * speed;		// Add our acceleration to our position's Z
}

void Camera::MoveRight(float speed)
{
	speed *= TimeManager::Instance().DeltaTime;
	// Get our normalized right vector (The direction perpendicular to our view)
	vec3 rightVector = GetRight();

	// Move our camera position right or left along the right vector
	Position.x += rightVector.x * speed;		// Add our acceleration to our position's X
	Position.z += rightVector.z * speed;		// Add our acceleration to our position's Z
}

void Camera::MoveUp(float speed)
{
	speed *= TimeManager::Instance().DeltaTime;
	vec3 upVector = GetUp();

	// Move our camera position up or down along the right vector
	Position.y += upVector.x * speed;		// Add our acceleration to our position's Y
}

void Camera::MoveDown(float speed)
{
	speed *= TimeManager::Instance().DeltaTime;
	vec3 upVector = GetUp();

	// Move our camera position up or down along the right vector
	Position.y -= upVector.x * speed;		// Add our acceleration to our position's Y
}

// This sets the pitch and yaw of our camera from the mouse x and y deltas from the last frame
void Camera::SetViewByMouse(float xOffset, float yOffset)
{
	// Rotate the yaw by the mouse's x offset, multiplied by a sensitivity speed setting
	Yaw += xOffset * MouseSpeed;

	// Rotate the pitch by the mouse's y offset, multiplied by a sensitivity speed setting
	Pitch += yOffset * MouseSpeed;

	// If the yaw goes above 360 degrees (2 PI in radians) then set it back to 0
	if (Yaw > 2 * PI)
		Yaw = 0;

	// Cap the yaw between 0 and 360
	if (Yaw < 0)
		Yaw = 2 * PI;

	// Let's cap the pitch to 75 degrees up and -75 degrees down
	if (Pitch > radians(75.0f))
		Pitch = radians(75.0f);
	if (Pitch < radians(-75.0f))
		Pitch = radians(-75.0f);
}
