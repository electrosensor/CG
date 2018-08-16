#pragma once


#include "Model.h"
#include "Defs.h"
#include "Util.h"

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
		MeshModel(const std::string& fileName);
		~MeshModel();

        const glm::mat4x4& GetModelTransformation() override;
		const glm::mat4x4& GetWorldTransformation() override;
		const glm::mat4x4& GetNormalTransformation() override;

        void SetModelTransformation(glm::mat4x4& transformation) override;
		void SetWorldTransformation(glm::mat4x4& transformation) override;
		void SetNormalTransformation(glm::mat4x4& transformation) override;

		void LoadFile(const std::string& fileName);
		void Draw(std::tuple<std::vector<glm::vec4>, std::vector<glm::vec4>, std::vector<glm::vec4> >& modelData) override;
        glm::vec4 getCentroid() override { return  m_modelCentroid; }



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
    std::string* setPrimModelFilePath(PRIM_MODEL primModel);

    std::string* m_pPrimModelString;
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

    void Draw(std::tuple<std::vector<glm::vec4>, std::vector<glm::vec4>, std::vector<glm::vec4> >& modelData) override;


};

