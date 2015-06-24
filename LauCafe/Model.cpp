#include "Model.h"

Vertex3 Model::axis[6] = { vec3(1.0, 0.0, 0.0), vec4(1, 0, 0, 1),
						   vec3(0.0, 0.0, 0.0), vec4(1, 0, 0, 1),
						   vec3(0.0, 1.0, 0.0), vec4(0, 1, 0, 1),
						   vec3(0.0, 0.0, 0.0), vec4(0, 1, 0, 1),
						   vec3(0.0, 0.0, 1.0), vec4(0, 0, 1, 1),
						   vec3(0.0, 0.0, 0.0), vec4(0, 0, 1, 1) };

Vertex3 Model::square[6] = { vec3(-0.5,  0.5,  0.0), vec4(1, 1, 0, 1),
							 vec3(-0.5, -0.5,  0.0), vec4(1, 1, 0, 1),
							 vec3( 0.5, -0.5,  0.0), vec4(0, 0, 1, 1),
							 vec3( 0.5, -0.5,  0.0), vec4(0, 0, 1, 1),
							 vec3( 0.5,  0.5,  0.0), vec4(1, 1, 0, 1),
							 vec3(-0.5,  0.5,  0.0), vec4(1, 1, 0, 1) };

Vertex3 Model::square2[6] = { vec3(-0.5,  0.0,  0.5), vec4(1, 1, 0, 1),
							  vec3(-0.5,  0.0, -0.5), vec4(1, 1, 0, 1),
							  vec3( 0.5,  0.0, -0.5), vec4(0, 0, 1, 1),
							  vec3( 0.5,  0.0, -0.5), vec4(0, 0, 1, 1),
							  vec3( 0.5,  0.0,  0.5), vec4(1, 1, 0, 1),
							  vec3(-0.5,  0.0,  0.5), vec4(1, 1, 0, 1) };

void Model::Initialize(Vertex3 pVertices[], int length, GLenum pFace, std::string vert, std::string frag) {
	FaceMode = pFace;
	m_Shader.Initialize(vert, frag);
	Selected = false;

	vertices = pVertices;
	verticesLength = length;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * length, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VertexIndex, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), 0);
	glVertexAttribPointer(ColorIndex, 4, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (GLvoid*)sizeof(vertices[0].rgba));
}

void Model::Select() {
	Selected = true;
}

void Model::Unselect() {
	Selected = false;
}

void Model::Render() {
	m_Shader.Bind();

	mat4 projectionMatrix = m_Camera->GetProjectionMatrix();
	mat4 viewMatrix = m_Camera->GetViewMatrix();
	mat4 modelMatrix = glm::translate(mat4(1.0f), position);
	modelMatrix = glm::scale(modelMatrix, scale);

	modelMatrix = glm::rotate(modelMatrix, rotation.x, vec3(1, 0, 0));	// x-axis
	modelMatrix = glm::rotate(modelMatrix, rotation.y, vec3(0, 1, 0));	// y-axis
	modelMatrix = glm::rotate(modelMatrix, rotation.z, vec3(0, 0, 1));	// z-axis

	GLint modelMatrixId = m_Shader.GetVariable("modelMatrix");
	GLint viewMatrixId = m_Shader.GetVariable("viewMatrix");
	GLint projectionMatrixId = m_Shader.GetVariable("projectionMatrix");

	m_Shader.SetMatrix4(modelMatrixId, 1, GL_FALSE, &modelMatrix[0][0]);
	m_Shader.SetMatrix4(viewMatrixId, 1, GL_FALSE, &viewMatrix[0][0]);
	m_Shader.SetMatrix4(projectionMatrixId, 1, GL_FALSE, &projectionMatrix[0][0]);

	GLint blueId = m_Shader.GetVariable("blue");
	if (Selected) {
		m_Shader.SetFloat(blueId, 1.0f);
	}
	else {
		m_Shader.SetFloat(blueId, 0.0f);
	}

	glBindVertexArray(VAO);
	glEnableVertexAttribArray(VertexIndex);
	glEnableVertexAttribArray(ColorIndex);
	glDrawArrays(FaceMode, 0, verticesLength);
	glDisableVertexAttribArray(ColorIndex);
	glDisableVertexAttribArray(VertexIndex);
	glBindVertexArray(0);

	m_Shader.Unbind();
}

void Model::Destroy() {
	if (VBO)
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}

	if (VAO)
	{
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}
	m_Camera = nullptr;
	m_Shader.Destroy();
}