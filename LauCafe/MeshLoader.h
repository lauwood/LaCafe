////////////
// lauwood
//

#ifndef MESHLOADER_H_
#define MESHLOADER_H_

#include <assimp/ai_assert.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "AnimEvaluator.h"
#include "SceneAnimator.h"
#include "Shader.h"
#include "Camera.h"

#include <map>
#include <iostream>
#include <vector>

#include "Main.h"

using namespace glm;

class Shader;

struct MeshEntry {
	MeshEntry();
	~MeshEntry();
	void Init(const std::vector<aiVector3D>& CachedPositions, 
			  const std::vector<aiVector3D>& CachedNormals, 
			  const std::vector<unsigned int>& Indices);

	unsigned int NumIndices;
	unsigned int BaseVertex;
	unsigned int BaseIndex;
	unsigned int MaterialIndex;
};

class Mesh {
public:

	Mesh() {};
	~Mesh();
	Mesh(const char* filename, std::string vert, std::string frag);

	void LoadScene();
	void LoadMesh(unsigned int index, const aiMesh* mesh, 
				  std::vector<aiVector3D>& CachedPositions, 
				  std::vector<aiVector3D>& CachedNormals, 
				  std::vector<aiVector2D>& TexCoords, 
				  std::vector<unsigned int>& Indices);
	
	// Renders the mesh using VBOs
	void Render();

	// Force bones to be disabled when there are no bones in the model
	void DisableBones()		{ m_ForceNoBones = true; }

	// Updates the current animation frame and channel by calling upon the scene animator's calculate method
	void UpdateAnimation();

	void SetAnimation(unsigned int index);
	unsigned int GetAnimation() { return m_AnimIndex; }
	bool HasAnimations() { return m_IsAnimated; };

	// Get and Set for the model's position
	vec3 GetPosition() { return Position; }
	void SetPosition(vec3 position) { Position = position; }

	// Get and Set for the model's rotation
	vec3 GetRotation() { return Rotation; }
	void SetRotation(vec3 rotation) { Rotation = rotation; }

	// Get and Set for the model's scale
	vec3 GetScale() { return Scale; }
	void SetScale(vec3 scale) { Scale = scale; }

	// Get and Set for the model's camera
	Camera *GetCamera() { return m_Camera; }
	void SetCamera(Camera *pCamera) { m_Camera = pCamera; }

private:
	bool m_ForceNoBones;
	bool m_IsAnimated;
	bool m_AnimIndex;

	double CurrentTime;
	double LastPlaying;

	bool LoadAsset(const char* filename);
	void CalculateBones(const aiNode* node);

	void RenderMesh(const aiNode* node);

	const aiScene* m_Scene;
	SceneAnimator* mAnimator;

	std::vector<MeshEntry> m_Entries;
	std::vector<aiVector3D> CachedPositions;
	std::vector<aiVector3D> CachedNormals;
	std::vector<aiVector2D> TexCoords;
	std::vector<unsigned int> Indices;

	enum VB_TYPES {
		INDEX_BUFFER,
		POS_VB,
		NORMAL_VB,
		TEXCOORD_VB,
		NUM_VBs
	};
	GLuint m_VAO;
	GLuint m_Buffers[NUM_VBs];

	vec3 Position;										// The model's position
	vec3 Rotation;										// The model's rotation
	vec3 Scale;											// The model's scale

	Camera* m_Camera;
	Shader m_Shader;
};

#endif