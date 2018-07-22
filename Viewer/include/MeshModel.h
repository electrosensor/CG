#pragma once
#include <glm/glm.hpp>
#include <string>
#include "Model.h"

using namespace std;

//TODO_AVIAD: move after creating a global definitions h file
// Global project definitions:
typedef enum _RETURN_CODE
{
	RC_SUCCESS = 0,
	RC_FAILURE,
	RC_UNDEFINED,
	RC_IO_ERROR

}RETURN_CODE;


/*
 * MeshModel class. Mesh model object represents a triangle mesh (loaded fron an obj file).
 * 
 */
class MeshModel : public Model
{
protected :

	glm::vec3 *m_vertexPositions;
	size_t m_vertexPosSize;
	// Add more attributes.
	glm::mat4x4 m_worldTransform;
	glm::mat4x4 m_normalTransform;

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
