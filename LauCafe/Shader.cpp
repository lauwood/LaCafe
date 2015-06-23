#include "Shader.h"
#include <assert.h>
#include "Logger.h"

#define MAX_SHADER_LENGTH 262144

GLuint Shader::Initialize(std::string vert_file_name, std::string frag_file_name) {
	GLuint vert_id, frag_id;
	assert(CreateShader(vert_file_name, &vert_id, GL_VERTEX_SHADER));
	assert(CreateShader(frag_file_name, &frag_id, GL_FRAGMENT_SHADER));
	assert(CreateProgram(vert_id, frag_id, &ShaderId));
	return ShaderId;
}

void Shader::Destroy() {
	if (ShaderId) {
		glDeleteShader(ShaderId);
		ShaderId = 0;
	}
}

bool Shader::CreateShader(std::string file_name, GLuint* shader, GLenum type) {
	Log("creating shader from %s...\n", file_name);
	std::string text_data = LoadFile(file_name);
	const char *text_data_cstr = text_data.c_str();
	*shader = glCreateShader(type);
	glShaderSource(*shader, 1, &text_data_cstr, NULL);
	glCompileShader(*shader);

	int params = -1;
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &params);
	if (GL_TRUE != params) {
		fprintf(stderr, "ERROR: GL shader index %i did not compile\n", *shader);
		Print_Program_Info_Log(*shader);
		return false;
	}
	Log("shader compiled. index %i\n", *shader);
	return true;
}

bool Shader::CreateProgram(GLuint vert, GLuint frag, GLuint* program) {
	*program = glCreateProgram();
	Log("created programme %u. attaching shaders %u and %u...\n", *program, vert, frag);
	glAttachShader(*program, vert);
	glAttachShader(*program, frag);

	glLinkProgram(*program);

	GLint params = -1;
	glGetProgramiv(*program, GL_LINK_STATUS, &params);
	if (GL_TRUE != params) {
		fprintf(stderr, "ERROR: could not link shader programme GL index %u\n", *program);
		Print_Program_Info_Log(*program);
		return false;
	}

	// delete shaders here to free memory
	glDeleteShader(vert);
	glDeleteShader(frag);
	return true;
}

std::string Shader::LoadFile(std::string filename) {
	std::ifstream fin(filename.c_str());

	if (!fin) {
		Log("ERROR: opening file for reading: %s\n", filename);
		return "";
	}

	std::string line = "";
	std::string text = "";

	while (getline(fin, line)) {
		text = text + "\n" + line;
	}

	fin.close();
	return text;
}


GLint Shader::GetVariable(std::string str) {
	return glGetUniformLocation(ShaderId, str.c_str());
}

