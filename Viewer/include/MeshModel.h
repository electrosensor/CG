#pragma once
#include <glm/glm.hpp>
#include <string>
#include "Model.h"
#include "Defs.h"


using namespace std;


/*
 * MeshModel class. Mesh model object represents a triangle mesh (loaded fron an obj file).
 * 
 */
class MeshModel : public Model
{
	protected :

        glm::vec3 *m_vectorNormals;
        size_t m_vectorNormSize;
		glm::vec3 *m_vertexPositions;
		size_t m_vertexPosSize;
        glm::vec3 *m_vertexNormals;

		// Add more attributes.
        glm::mat4x4 m_modelTransformation;
		glm::mat4x4 m_worldTransformation;
		glm::mat4x4 m_normalTransformation;

	public:
		MeshModel(const string& fileName);
		~MeshModel();

        const glm::mat4x4& GetModelTransformation();
		const glm::mat4x4& GetWorldTransformation();
		const glm::mat4x4& GetNormalTransformation();

        void SetModelTransformation(glm::mat4x4& transformation);
		void SetWorldTransformation(glm::mat4x4& transformation);
		void SetNormalTransformation(glm::mat4x4& transformation);

		void LoadFile(const string& fileName);
		const pair<vector<glm::vec3>, vector<glm::vec3>>* Draw();

};

class PrimMeshModel : public MeshModel
{
public:
	PrimMeshModel(PRIM_MODEL primModel) : MeshModel(*setPrimModelFilePath(primModel))
	{
		if (m_pPrimModelString)
		{
			delete m_pPrimModelString;
			m_pPrimModelString = nullptr;
		}
	}


private:
	string* setPrimModelFilePath(PRIM_MODEL primModel);

	string* m_pPrimModelString;
};

