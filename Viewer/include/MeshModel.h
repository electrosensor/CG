#pragma once
#include <glm/glm.hpp>
#include <string>
#include "Model.h"

using namespace std;

/*
 * MeshModel class. Mesh model object represents a triangle mesh (loaded fron an obj file).
 * 
 */
class MeshModel : public Model
{
protected :

	glm::vec3 *vertexPositions;
	size_t vertexPosSize;
	// Add more attributes.
	glm::mat4x4 worldTransform;
	glm::mat4x4 normalTransform;

public:
	MeshModel(const string& fileName);
	~MeshModel();

	const glm::mat4x4& GetWorldTransform();
	const glm::mat4x4& GetNormalTransform();

	void SetWorldTransform(glm::mat4x4& transformation);
	void SetNormalTransform(glm::mat4x4& transformation);

	void LoadFile(const string& fileName);
	const vector<glm::vec3>* Draw();

};
