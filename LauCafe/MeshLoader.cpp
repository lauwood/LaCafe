////////////
// lauwood
//

#include "MeshLoader.h"
#include "Globals.h"
#include <time.h>
#include <string>
#include <GL/glew.h>

#define INVALID_MATERIAL 0xFFFFFFFF

#define POSITION_LOCATION 0
#define NORMAL_LOCATION 1
#define TEX_COORD_LOCATION 2

MeshEntry::MeshEntry() {
	NumIndices = 0;
	BaseVertex = 0;
	BaseIndex = 0;
	MaterialIndex = INVALID_MATERIAL;
}

MeshEntry::~MeshEntry() {
	NumIndices = 0;
}

void MeshEntry::Init(const std::vector<aiVector3D>& CachedPositions, const std::vector<aiVector3D>& CachedNormals, const std::vector<unsigned int>& Indices) {
	NumIndices = Indices.size();
}

Mesh::Mesh(const Mesh& mesh, std::string vert, std::string frag) {
	m_Entries = mesh.m_Entries;
	CachedPositions = mesh.CachedPositions;
	CachedNormals = mesh.CachedNormals;
	TexCoords = mesh.TexCoords;
	Indices = mesh.Indices;

	m_Shader = mesh.m_Shader;
	m_ForceNoBones = false;
	m_Scene = NULL;
	m_VAO = 0;

	CurrentTime = 0;
	LastPlaying = 0;

	memset(m_Buffers, 0, sizeof(m_Buffers)); 
	
	LoadVBO();
}

Mesh::Mesh(const char* filename, std::string vert, std::string frag) {
	m_Shader.Initialize(vert, frag);
	m_ForceNoBones = false;
	m_Scene = NULL;
	m_VAO = 0;

	CurrentTime = 0;
	LastPlaying = 0;

	memset(m_Buffers, 0, sizeof(m_Buffers));

	std::cout << "Mesh:: loading " << filename << std::endl;
	if (!LoadAsset(filename)) {
		std::cout << "Error loading file" << std::endl;
	};
	std::cout << "Mesh:: finished " << std::endl;

	LoadVBO();
}

Mesh::~Mesh() {
	/*if (m_Buffers[0] != 0) {
		glDeleteBuffers(sizeof(m_Buffers) / sizeof(m_Buffers[0]), m_Buffers);
	}
	if (m_VAO != 0) {
		glDeleteVertexArrays(1, &m_VAO);
	}
	delete(m_Scene);*/
}

bool Mesh::LoadAsset(const char* filename) {
	m_Scene = aiImportFile(filename, aiProcessPreset_TargetRealtime_MaxQuality);

	if (!m_Scene) {
		std::cout << "Model was not loaded correctly." << std::endl;
		exit(-1);
	}

#if DEBUG
	for (unsigned int i = 0; i < m_Scene->mNumMeshes; i++) {
		std::cout << "\t\t" << i << " mesh name: " << m_Scene->mMeshes[i]->mName.C_Str() << " (" << m_Scene->mMeshes[i]->mNumFaces << ")" << std::endl;
		std::cout << "\t\t bones: " << m_Scene->mMeshes[i]->mNumBones << std::endl;
	}
#endif

	if (m_Scene->HasAnimations()) {
		std::cout << filename << " has animations:" << std::endl;
		for (unsigned int i = 0; i < m_Scene->mNumAnimations; i++) {
			std::cout << "\t" << m_Scene->mAnimations[i]->mName.C_Str() << std::endl;
		}
		m_IsAnimated = true;
		mAnimator = new SceneAnimator(m_Scene);
	}

	LoadScene();

	return true;
}

void Mesh::LoadScene() {
	m_Entries.resize(m_Scene->mNumMeshes);

	unsigned int NumVertices = 0;
	unsigned int NumIndices = 0;

	for (unsigned int i = 0; i < m_Entries.size(); i++) {
		m_Entries[i].MaterialIndex = m_Scene->mMeshes[i]->mMaterialIndex;
		m_Entries[i].NumIndices = m_Scene->mMeshes[i]->mNumFaces * 3;
		m_Entries[i].BaseVertex = NumVertices;
		m_Entries[i].BaseIndex = NumIndices;

		NumVertices += m_Scene->mMeshes[i]->mNumVertices;
		NumIndices += m_Entries[i].NumIndices;
	}

	CachedPositions.reserve(NumVertices);
	CachedNormals.reserve(NumVertices);
	TexCoords.reserve(NumVertices);
	Indices.reserve(NumIndices);

	for (unsigned int i = 0; i < m_Entries.size(); i++) {
		const aiMesh* mesh = m_Scene->mMeshes[i];
		LoadMesh(i, mesh, CachedPositions, CachedNormals, TexCoords, Indices);
	}
}

void Mesh::LoadMesh(unsigned int index, const aiMesh* mesh, std::vector<aiVector3D>& CachedPositions, std::vector<aiVector3D>& CachedNormals, std::vector<aiVector2D>& TexCoords, std::vector<unsigned int>& Indices) {
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		const aiVector3D vertices(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		const aiVector3D normals(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		const aiVector2D tex(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		CachedPositions.push_back(vertices);
		CachedNormals.push_back(normals);
		TexCoords.push_back(tex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		const aiFace* face = &mesh->mFaces[i];
		if (face->mNumIndices == 2) {
			Indices.push_back(face->mIndices[0]);
			Indices.push_back(face->mIndices[1]);
			Indices.push_back(0);
		} else if (face->mNumIndices == 3) {
			Indices.push_back(face->mIndices[0]);
			Indices.push_back(face->mIndices[1]);
			Indices.push_back(face->mIndices[2]);
		}
	}
}

void Mesh::SetAnimation(unsigned int index) {
	if (m_IsAnimated) {
		m_AnimIndex = index;
		mAnimator->SetAnimIndex(index);
	}
}

void Mesh::LoadVBO() {
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(sizeof(m_Buffers) / sizeof(m_Buffers[0]), m_Buffers);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CachedPositions[0]) * CachedPositions.size(), &CachedPositions[0], GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CachedNormals[0]) * CachedNormals.size(), &CachedNormals[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(NORMAL_LOCATION);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(TEX_COORD_LOCATION);
	glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_DYNAMIC_DRAW);
}

void Mesh::RenderMesh() {
	m_Shader.Bind();

	mat4 projectionMatrix = Globals::Camera.GetProjectionMatrix();
	mat4 viewMatrix = Globals::Camera.GetViewMatrix();
	mat4 modelMatrix = translate(mat4(1.0f), Position);

	// Similar to the Position above, we do the same for the Scale
	modelMatrix = scale(modelMatrix, Scale);

	// Add any rotation to the Axis Matrix for each axis
	modelMatrix = rotate(modelMatrix, Rotation.x, vec3(1, 0, 0));	// x-axis
	modelMatrix = rotate(modelMatrix, Rotation.y, vec3(0, 1, 0));	// y-axis
	modelMatrix = rotate(modelMatrix, Rotation.z, vec3(0, 0, 1));	// z-axis

	// Get the variables from the shader for each matrix
	GLint modelMatrixId = m_Shader.GetVariable("modelMatrix");
	GLint viewMatrixId = m_Shader.GetVariable("viewMatrix");
	GLint projectionMatrixId = m_Shader.GetVariable("projectionMatrix");

	// Set the model, view and projection matrix for the uniform matrices in the Vertex Shader.
	// To get at a GLM mat4 value, we use the address of the first item [0][0].
	m_Shader.SetMatrix4(modelMatrixId, 1, false, &modelMatrix[0][0]);
	m_Shader.SetMatrix4(viewMatrixId, 1, false, &viewMatrix[0][0]);
	m_Shader.SetMatrix4(projectionMatrixId, 1, false, &projectionMatrix[0][0]);
	glBindVertexArray(m_VAO);
	glEnable(GL_NORMALIZE);
	for (unsigned int i = 0; i < m_Entries.size(); i++) {
		//ApplyMaterial(m_Scene->mMaterials[m_Entries[i].MaterialIndex]);
		glDrawElementsBaseVertex(GL_TRIANGLES, m_Entries[i].NumIndices, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * m_Entries[i].BaseIndex), m_Entries[i].BaseVertex);
	}

	glBindVertexArray(0);
	m_Shader.Unbind();
}

void Mesh::UpdateAnimation() {
	const float animRate = 1.0f;
	if (m_Scene->HasAnimations()) {
		CurrentTime += clock() *animRate / double(CLOCKS_PER_SEC) - LastPlaying;
		mAnimator->Calculate(CurrentTime);
		LastPlaying = CurrentTime;
	}
}

void Mesh::CalculateBones(const aiNode* node) {
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		const aiMesh* mesh = m_Scene->mMeshes[node->mMeshes[i]];
		if (mesh->HasBones() && !m_ForceNoBones) {
			const std::vector<aiMatrix4x4>& boneMatrices = mAnimator->GetBoneMatrices(node, i);

			for (unsigned int a = 0; a < mesh->mNumBones; a++) {
				const aiBone *bone = mesh->mBones[a];
				const aiMatrix4x4& posTrafo = boneMatrices[a];
				aiMatrix3x3 normTrafo = aiMatrix3x3(posTrafo);
				for (unsigned int b = 0; b < bone->mNumWeights; b++)
				{
					const aiVertexWeight& weight = bone->mWeights[b];
					unsigned int vertexId = weight.mVertexId;
					const aiVector3D& srcPos = mesh->mVertices[vertexId];
					const aiVector3D& srcNorm = mesh->mNormals[vertexId];
					CachedPositions[vertexId] += weight.mWeight * (posTrafo * srcPos);
					CachedNormals[vertexId] += weight.mWeight * (normTrafo * srcNorm);
				}
			}
		}


		/*for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
			const aiFace* face = &mesh->mFaces[j];
			for (unsigned int k = 0; k < face->mNumIndices; k++) {
				int v_index = face->mIndices[k];
				glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
				glBufferSubData(GL_ARRAY_BUFFER, v_index, sizeof(CachedPositions[0]) * face->mNumIndices, &CachedPositions[v_index]);

				glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
				glBufferSubData(GL_ARRAY_BUFFER, v_index, sizeof(CachedNormals[0]) * face->mNumIndices, &CachedNormals[v_index]);
			}
		}*/
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		CalculateBones(node->mChildren[i]);
	}
}

void Mesh::Render() {
//	CalculateBones(m_Scene->mRootNode);
	RenderMesh();
}
