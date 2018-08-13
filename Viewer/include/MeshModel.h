#pragma once
#include <glm/glm.hpp>
#include <string>
#include "Model.h"
#include "Defs.h"
#include "Util.h"


using namespace std;


/*
 * MeshModel class. Mesh model object represents a triangle mesh (loaded fron an obj file).
 * 
 */
class MeshModel : public Model
{
	protected :

        size_t m_verticesSize;
        glm::vec4 *m_vertices;
        size_t m_vertexNormSize;
		glm::vec4 *m_vertexPositions;
		size_t m_vertexPosSize;
        glm::vec4 *m_vertexNormals;

		// Add more attributes.
        glm::mat4x4 m_modelTransformation;
		glm::mat4x4 m_worldTransformation;
		glm::mat4x4 m_normalTransformation;
        glm::vec4   m_modelCentroid;

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
		pair<vector<glm::vec4>, pair<vector<glm::vec4>, vector<glm::vec4> > >* Draw();
        glm::vec4 getCentroid() { return  m_modelCentroid; }



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


class CamMeshModel : public MeshModel
{
public:
    CamMeshModel(glm::vec4 camCoords) : MeshModel(CAMERA_OBJ_FILE)
    {
        m_camCoords = camCoords;
//         glm::mat4x4 eyeTranslation = glm::mat4x4(TRANSLATION_MATRIX(camCoords.x, camCoords.y, camCoords.x))*GetModelTransformation();
//         SetModelTransformation(eyeTranslation);
        m_bShouldRender = false;
    }

    glm::vec4 m_camCoords;

    void setCamCoords(glm::vec4 camCoords)
    {
        m_camCoords = camCoords;
    }

    glm::vec4 getCamCoords()
    {
        return m_camCoords;
    }

    pair<vector<glm::vec4>, pair<vector<glm::vec4>, vector<glm::vec4> > >* Draw()
    {
        pair<vector<glm::vec4>, pair<vector<glm::vec4>, vector<glm::vec4> > >* verticesData = new pair<vector<glm::vec4>, pair<vector<glm::vec4>, vector<glm::vec4> > >();
        vector<glm::vec4> camModelVertices; //AVIAD TODO CHECK
        pair<vector<glm::vec4>, vector<glm::vec4> > dummy;

        for (size_t i = 0; i < m_vertexPosSize; i++)
        {
            glm::vec4 vertex = glm::mat4x4(SCALING_MATRIX4(0.3f))*m_vertexPositions[i];
            camModelVertices.push_back(vertex);
        }

        verticesData->first = camModelVertices;
        verticesData->second = dummy;

        return verticesData;
    }


};

