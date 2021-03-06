#pragma once


#include "Model.h"


/*
 * MeshModel class. Mesh model object represents a triangle mesh (loaded fron an obj file).
 * 
 */

class MeshModel : public Model
{
	protected :
        GLuint VAO;
        GLuint VBO;
        GLuint TEX;

        size_t m_verticesSize;
        glm::vec3 *m_vertices;
        size_t m_verticesNormSize;
        Face *m_polygons;
		size_t m_polygonsSize;
        glm::vec3 *m_vertexNormals;
        size_t m_vPositionsSize;
        glm::vec3* m_vertexPositions;

		// Add more attributes.
        glm::mat4x4 m_scaleTransformation;
        glm::mat4x4 m_translateTransformation;
        glm::mat4x4 m_rotateTransformation;

        glm::mat4x4 m_modelTransformation;
		glm::mat4x4 m_worldTransformation;
		glm::mat4x4 m_normalTransformation;
        glm::vec3   m_modelCentroid;

	public:
        Surface m_surface;
		MeshModel(const std::string& fileName, const Surface& material, GLuint program);
		~MeshModel();

        const glm::mat4x4& GetModelTransformation() override;
        const glm::mat4x4& GetScaleTransformation() override;
        const glm::mat4x4& GetTranslateTransformation() override;
        const glm::mat4x4& GetRotateTransformation() override;
		const glm::mat4x4& GetWorldTransformation() override;
		const glm::mat4x4& GetNormalTransformation() override;

        void SetModelTransformation(glm::mat4x4& transformation) override;
        void SetScaleTransformation(glm::mat4x4& transformation) override;
        void SetTranslateTransformation(glm::mat4x4& transformation) override;
        void SetRotateTransformation(glm::mat4x4& transformation) override;
		void SetWorldTransformation(glm::mat4x4& transformation) override;
		void SetNormalTransformation(glm::mat4x4& transformation) override;

		void LoadFile(const std::string& fileName, GLuint program);
		void Draw(std::tuple<std::vector<Face>, std::vector<glm::vec3>, std::vector<glm::vec3>, std::vector<glm::vec3> >& modelData) override;
        glm::vec3 getCentroid() override { return  m_modelCentroid; }

        void ApplyTexture(std::string path) override;
private:
    GLuint m_cur_prog;
};

class PrimMeshModel : public MeshModel
{
public:
    PrimMeshModel() = delete;
	PrimMeshModel(PRIM_MODEL primModel, const Surface& material, GLuint prog) : MeshModel(*setPrimModelFilePath(primModel), material, prog)
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
    CamMeshModel(glm::vec3 camCoords, GLuint prog) : MeshModel(CAMERA_OBJ_FILE, Surface(), prog)
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

    void Draw(std::tuple<std::vector<Face>, std::vector<glm::vec3>, std::vector<glm::vec3>, std::vector<glm::vec3> >& modelData) override;

    ~CamMeshModel() = default;
};

class LightMeshModel : public MeshModel
{
public:

    LightMeshModel(LIGHT_SOURCE_TYPE type, const glm::vec3& location, GLuint prog) : MeshModel(LIGHT_OBJ_FILE, Surface(), prog)
    {
        SetModelTransformation(glm::mat4x4(TRANSLATION_MATRIX(location.x, location.y, location.z)) * glm::mat4x4(SCALING_MATRIX4(0.1)));
    } 
   ~LightMeshModel() = default;
   
};