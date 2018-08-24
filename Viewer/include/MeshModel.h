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
        glm::vec3 *m_vertices;
        size_t m_verticesNormSize;
        Face *m_polygons;
		size_t m_polygonsSize;
        glm::vec3 *m_vertexNormals;

		// Add more attributes.
        glm::mat4x4 m_modelTransformation;
		glm::mat4x4 m_worldTransformation;
		glm::mat4x4 m_normalTransformation;
        glm::vec3   m_modelCentroid;
        Surface m_surface;

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
		void Draw(std::tuple<std::vector<Face>, std::vector<glm::vec3>, std::vector<glm::vec3> >& modelData) override;
        glm::vec3 getCentroid() override { return  m_modelCentroid; }



};

class PrimMeshModel : public MeshModel
{
public:
    PrimMeshModel() = delete;
	PrimMeshModel(PRIM_MODEL primModel) : MeshModel(*setPrimModelFilePath(primModel))
	{
		if (m_pPrimModelString)
		{
			delete m_pPrimModelString;
			m_pPrimModelString = nullptr;
		}
	}
    ~PrimMeshModel() = default;
private:
    std::string* setPrimModelFilePath(PRIM_MODEL primModel);

    std::string* m_pPrimModelString;
};


class CamMeshModel : public MeshModel
{
public:
    CamMeshModel() = delete;
    CamMeshModel(glm::vec3 camCoords) : MeshModel(CAMERA_OBJ_FILE)
    {
        m_camCoords = camCoords;
//         glm::mat4x4 eyeTranslation = glm::mat4x4(TRANSLATION_MATRIX(camCoords.x, camCoords.y, camCoords.x))*GetModelTransformation();
//         SetModelTransformation(eyeTranslation);
        m_bShouldRender = false;
    }

    glm::vec3 m_camCoords;

    void setCamCoords(glm::vec3 camCoords)
    {
        m_camCoords = camCoords;
    }

    glm::vec3 getCamCoords()
    {
        return m_camCoords;
    }

    void Draw(std::tuple<std::vector<Face>, std::vector<glm::vec3>, std::vector<glm::vec3> >& modelData) override;

    ~CamMeshModel() = default;
};

class LightMeshModel : public MeshModel
{
public:

    LightMeshModel() : MeshModel(LIGHT_OBJ_FILE) {} // change to light
   ~LightMeshModel() = default;
   
};