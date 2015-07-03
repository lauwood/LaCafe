#ifndef CUBEMAP_H
#define CUBEMAP_H

#include "Main.h"
#include "Shader.h"
#include "Camera.h"

class CubeMap {
public:
	CubeMap();
	~CubeMap();
	void Initialize();
	void CreateCubeMap(const char* front,
					   const char* back,
					   const char* top,
					   const char* bottom,
					   const char* left,
					   const char* right);

	bool LoadCubeMapSide(GLuint texture, GLenum side_target, const char* file_name);
	void Render();
private:
	GLuint vbo;
	GLuint vao;
	GLuint tex_cube;

	Shader m_Shader;
};

#endif