#include "Shader.h"

Shader::Shader(std::string vert, std::string frag) {
	Init(vert, frag);
}

Shader::~Shader() {
	Destroy();
}

void Shader::Destroy() {
	if (VertexId) {
		glDetachShader(ShaderId, VertexId);
		glDeleteShader(VertexId);
		VertexId = 0;
	}

	if (FragmentId) {
		glDetachShader(ShaderId, FragmentId);
		glDeleteShader(FragmentId);
		FragmentId = 0;
	}

	if (ShaderId) {
		glDeleteShader(ShaderId);
		ShaderId = 0;
	}
}

std::string Shader::LoadShaderFile(std::string filename) {
	std::ifstream fin(filename.c_str());

	if (!fin)
		return "";

	std::string line = "";
	std::string text = "";

	while (getline(fin, line)) {
		text = text + "\n" + line;
	}

	fin.close();
	return text;
}

int Shader::Init(std::string vert, std::string frag) {
	if (!vert.length() || !frag.length())
		return 0;

	// If any of the shader pointers are already set, free them
	if (VertexId || FragmentId || ShaderId)
		Destroy();

	VertexId = glCreateShader(GL_VERTEX_SHADER);
	FragmentId = glCreateShader(GL_FRAGMENT_SHADER);

	// Holds the shader's text file data
	std::string vertStringData, fragStringData;

	vertStringData = LoadShaderFile(vert.c_str());
	fragStringData = LoadShaderFile(frag.c_str());

	// switch to const char* to read
	const char *vshader = vertStringData.c_str();
	const char *fshader = fragStringData.c_str();

	glShaderSource(VertexId, 1, &vshader, nullptr);
	glShaderSource(FragmentId, 1, &fshader, nullptr);

	glCompileShader(VertexId);
	glCompileShader(FragmentId);

	ShaderId = glCreateProgram();
	glAttachShader(ShaderId, VertexId);
	glAttachShader(ShaderId, FragmentId);

	glLinkProgram(ShaderId);

	return 1;
}

GLint Shader::GetVariable(std::string str) {
	if (!ShaderId)
		return -1;

	return glGetUniformLocation(ShaderId, str.c_str());
}

