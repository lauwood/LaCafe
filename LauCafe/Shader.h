#ifndef _SHADER_H_
#define _SHADER_H_
#include "Main.h"
#include <fstream>
#include <string>

class Shader
{
public:
	Shader()	{}
	~Shader()	{ Destroy(); }
	void Destroy();

	GLuint Initialize(std::string vert, std::string frag);
	bool CreateShader(std::string file_name, GLuint* shader, GLenum type);
	bool CreateProgram(GLuint vert, GLuint frag, GLuint* program);
	std::string LoadFile(std::string file_name);
	GLint GetVariable(std::string str);

	void SetInt(GLint id, int newValue)											{ glUniform1i(id, newValue); }
	void SetFloat(GLint id, GLfloat newValue)									{ glUniform1f(id, newValue); }
	void SetFloat2(GLint id, GLfloat v0, GLfloat v1)							{ glUniform2f(id, v0, v1); }
	void SetFloat3(GLint id, GLfloat v0, GLfloat v1, GLfloat v2)				{ glUniform3f(id, v0, v1, v2); }
	void SetFloat4(GLint id, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)	{ glUniform4f(id, v0, v1, v2, v3); }
	
	void SetMatrix4(GLint id, GLsizei count, GLboolean transpose, const GLfloat *value)
	{
		glUniformMatrix4fv(id, count, transpose, value);
	}

	void Bind()			{ glUseProgram(ShaderId); }
	void Unbind()		{ glUseProgram(0); }
private:
	GLuint ShaderId;
};

#endif
