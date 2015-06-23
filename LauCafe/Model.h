#ifndef MODEL_H
#define MODEL_H

#define GLM_FORCE_RADIANS								// Make sure GLM is using radians instead of degrees

#include "Main.h"
#include "Shader.h"										// Used for loading and running our shaders
#include "Camera.h"										// Used to get access to the view and projection matrices
#include <string>

class Vertex3
{
public:
	vec3 xyz;											// The x,y,z position
	vec4 rgba;											// The r,g,b,a (Red, Green, Blue, Alpha) color values
};

// The Axis class to handle initializing, rendering and destroying of a 3D object
class Model
{
public:
	static Vertex3 axis[6];
	static Vertex3 square[6];
	static Vertex3 square2[6];

	// These are created to standardize the index properties used for vertex properties.
	static const GLuint VertexIndex = 0;				// This is for the x,y,z vertices
	static const GLuint ColorIndex = 1;					// This is for the r,g,b,a colors
	static const GLuint TextureIndex = 2;				// This is for the u,v texture coordinates
	static const GLuint NormalIndex = 3;				// This is for the x,y,z normals

	// Set the scale of our model to 100% by default
	Model()		{ scale = glm::vec3(1.0f, 1.0f, 1.0f); }
	~Model()	{ Destroy(); }

	// Takes in a list of x,y,z vertices and vertex length to create the VBO and VAO's from,
	// as well as the text file locations for the vertex and fragment shaders to load.
	void Initialize(Vertex3 pVertices[], int length, GLenum pFace, std::string vert, std::string frag);

	void Render();

	// Create the getter and setters for the model's position
	vec3 GetPosition() { return position; }
	void SetPosition(vec3 pPosition) { position = pPosition; }

	// Create the getter and setters for the model's rotation
	vec3 GetRotation() { return rotation; }
	void SetRotation(vec3 pRotation) { rotation = pRotation; }

	// Create the getter and setters for the model's scale
	vec3 GetScale() { return scale; }
	void SetScale(vec3 pScale) { scale = pScale; }

	// This gets and sets the camera to get access to the view and projection matrices
	Camera *GetCamera() { return m_Camera; }
	void SetCamera(Camera *pCamera) { m_Camera = pCamera; }

	// This cleans up the VBO and VAO's
	void Destroy();

protected:
	GLenum FaceMode;
	Vertex3 *vertices;									// The list of the vertices
	GLint verticesLength;								// The length of the vertices
	GLuint VBO;											// The VertexBufferObject Id stored by the VAO
	GLuint VAO;											// The VertexArrayObject Id to bind when drawing

	glm::vec3 position;										// The model's position
	glm::vec3 rotation;										// The model's rotation
	glm::vec3 scale;											// The model's scale

	// Our shader object to initialize and run the shaders
	Shader m_Shader;

	// Add our camera object to access its matrices
	Camera *m_Camera;
};


#endif