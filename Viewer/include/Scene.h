#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Renderer.h"
#include "Model.h"
#include "Light.h"
#include "Camera.h"
#include "Defs.h"

using namespace std;


class Scene {
private:
    vector<Model*> m_models;
    vector<Light*> m_lights;
    vector<Camera*> m_cameras;
    Renderer *renderer;

    int m_activeModel;
    int m_activeLight;
    int m_activeCamera;
    glm::mat4x4 m_worldTransformation;

    glm::vec4 m_polygonColor;
    glm::vec4 m_bgColor;

    bool m_bDrawVecNormal;
    float m_vnScaleFactor;
    bool m_bDrawFaceNormal;
    float m_fnScaleFactor;
    bool m_bShowBorderCube;

public:
    Scene() = delete;
    Scene(Renderer *renderer) : renderer(renderer), m_activeModel(DISABLED), m_activeLight(DISABLED), m_activeCamera(DISABLED), m_bDrawVecNormal(false), m_vnScaleFactor(2.f), m_fnScaleFactor(2.f), m_bgColor(Util::getColor(CLEAR)), m_polygonColor(Util::getColor(WHITE))
    {
        m_worldTransformation = I_MATRIX ;
        m_worldTransformation[3].w = 1;
    }

    // Loads an obj file into the scene.
    void LoadOBJModel(string fileName);

    // Draws the current scene.
    void Draw();

    glm::mat4x4 GetWorldTransformation();
    void SetWorldTransformation(const glm::mat4x4 world);


    //Cameras API:

    int  GetActiveCameraIdx();
    Camera* getActiveCamera()
    {
        if (m_activeCamera != DISABLED) {
            return m_cameras[m_activeCamera]; 
        }
        else return NULL;
    }
    Camera* getCameraByIndex(int index)
    {
        return index < m_cameras.size() ? m_cameras[index] : nullptr; 
    }
    void SetActiveCameraIdx(unsigned int cameraIdx);

    unsigned int  AddCamera(const glm::vec4 & eye, const glm::vec4 & at, const glm::vec4 & up);
    void NextCamera();

    void DeleteActiveCamera();
    void SetPerspectiveProjection(PERSPECTIVE_PARAMS projParams);
    void SetOrthoProjection(PROJ_PARAMS projParams);
    void SetFrustum(PROJ_PARAMS projParams);

    glm::mat4x4 GetActiveCameraTransformation();
    glm::mat4x4 GetActiveCameraProjection();

    void ScaleActiveCamera(float value);

    void TranslateActiveCameraAxis(float value, AXES axis);

    void RotateActiveCameraWorldAxis(float angle, AXES axis);
   
    void RotateActiveCameraAxis(float angle, AXES axis);
    

    //Models API:

    int  GetActiveModelIdx();
    void SetActiveModelIdx(unsigned int modelIdx);

    float GetvnScale();
    void SetvnScale(float scale);

    float  GetfnScale();
    void SetfnScale(float scale);

    unsigned int  AddPrimitiveModel(PRIM_MODEL primitiveModel); //TO_DO develop good API
    void NextModel();
    void DeleteActiveModel();

    glm::mat4x4 GetActiveModelTransformation();
    
    void TranslateActiveModelAxis(float value, AXES axis);

    void ScaleActiveModel(float value);

    void RotateActiveModelAxis(float angle, AXES axis);
    
    void showFacesNormals(bool bDrawFaceNormal) { m_bDrawFaceNormal = bDrawFaceNormal; }
    void showVerticesNormals(bool bDrawVecNormal) { m_bDrawVecNormal = bDrawVecNormal; }
    void showBorderCube(bool bShowBorderCube) { m_bShowBorderCube = bShowBorderCube; }

    bool shouldRenderCamera(int cameraIndex);
    glm::vec4 GetBgColor();
    void SetBgColor(glm::vec4 newBgColor);
    glm::vec4 GetPolygonColor();
    void SetPolygonColor(glm::vec4 newMeshColor);
};