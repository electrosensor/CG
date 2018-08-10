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

    glm::vec3 m_polygonColor;
    glm::vec3 m_bgColor;

    bool m_bDrawVecNormal;
    float m_vnScaleFactor;
    bool m_bDrawFaceNormal;
    float m_fnScaleFactor;
    bool m_bShowBorderCube;

public:
    Scene() : m_activeModel(DISABLED), m_activeLight(DISABLED), m_activeCamera(DISABLED), m_worldTransformation(I_MATRIX), m_bDrawVecNormal(false) { ; }
    Scene(Renderer *renderer) : renderer(renderer), m_activeModel(DISABLED), m_activeLight(DISABLED), m_activeCamera(DISABLED), m_worldTransformation(I_MATRIX), m_bDrawVecNormal(false), m_vnScaleFactor(2.f), m_fnScaleFactor(2.f), m_bgColor(Util::getColor(CLEAR)), m_polygonColor(Util::getColor(WHITE)){ ; }

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

    void TranslateActiveCameraXAxis(float value);
    void TranslateActiveCameraYAxis(float value);
    void TranslateActiveCameraZAxis(float value);

    void RotateActiveCameraWorldXAxis(float angle);
    void RotateActiveCameraWorldYAxis(float angle);
    void RotateActiveCameraWorldZAxis(float angle);
   
    void RotateActiveCameraXAxis(float angle);
    void RotateActiveCameraYAxis(float angle);
    void RotateActiveCameraZAxis(float angle);
    

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
    
    void TranslateActiveModelXAxis(float value);
    void TranslateActiveModelYAxis(float value);
    void TranslateActiveModelZAxis(float value);

    void ScaleActiveModel(float value);

    void RotateActiveModelXAxis(float angle);
    void RotateActiveModelYAxis(float angle);
    void RotateActiveModelZAxis(float angle);
    
    void showFacesNormals(bool bDrawFaceNormal) { m_bDrawFaceNormal = bDrawFaceNormal; }
    void showVerticesNormals(bool bDrawVecNormal) { m_bDrawVecNormal = bDrawVecNormal; }
    void showBorderCube(bool bShowBorderCube) { m_bShowBorderCube = bShowBorderCube; }

    bool shouldRenderCamera(int cameraIndex);
    glm::vec3 GetBgColor();
    void SetBgColor(glm::vec3 newBgColor);
    glm::vec3 GetPolygonColor();
    void SetPolygonColor(glm::vec3 newMeshColor);
};