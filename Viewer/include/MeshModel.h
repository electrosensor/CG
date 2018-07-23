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

		glm::vec3 *m_vertexPositions;
		size_t m_vertexPosSize;
		// Add more attributes.
		glm::mat4x4 m_worldTransformation;
		glm::mat4x4 m_normalTransformation;

	public:
		MeshModel(const string& fileName);
		~MeshModel();

		const glm::mat4x4& GetWorldTransformation();
		const glm::mat4x4& GetNormalTransformation();

		void SetWorldTransformation(glm::mat4x4& transformation);
		void SetNormalTransformation(glm::mat4x4& transformation);

		void LoadFile(const string& fileName);
		const vector<glm::vec3>* Draw();

};
