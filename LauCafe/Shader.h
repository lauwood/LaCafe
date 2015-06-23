#ifndef _SHADER_H_
#define _SHADER_H_
#include "Main.h"
#include <fstream>
#include <string>

class Shader
{
public:
	Shader()	{};
	Shader(std::string vert, std::string frag);
	~Shader();
	void Destroy();

	std::string LoadShaderFile(std::string filename);
	int Init(std::string vert, std::string frag);
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
	GLuint VertexId;
	GLuint FragmentId;
	GLuint ShaderId;
};

#endif
