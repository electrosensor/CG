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

class Scene {
private:
    std::vector<Model*> m_models;
    std::vector<Light*> m_lights;
    std::vector<Camera*> m_cameras;
    Renderer *renderer;

    int m_activeModel;
    int m_activeLight;
    int m_activeCamera;
    glm::mat4x4 m_worldTransformation;

    glm::vec4 m_polygonColor;
    glm::vec4 m_wireframeColor;
    glm::vec4 m_bgColor;

    bool m_bDrawWireframe;
    bool m_bDrawVecNormal;
    float m_vnScaleFactor;
    bool m_bDrawFaceNormal;
    float m_fnScaleFactor;
    bool m_bShowBorderCube;

    SHADING_TYPE m_shading;

public:
    Scene() = delete;
    Scene(Renderer *renderer) : renderer(renderer), m_activeModel(DISABLED), m_activeLight(DISABLED), m_activeCamera(DISABLED), m_bDrawVecNormal(false), m_vnScaleFactor(2.f), m_fnScaleFactor(2.f), m_bgColor(Util::getColor(YURI_BG)), m_polygonColor(Util::getColor(YURI_POLYGON)), m_wireframeColor(Util::getColor(YURI_WIRE)), m_bDrawWireframe(true)
    {
        m_worldTransformation = I_MATRIX ;
        m_worldTransformation[3].w = 1;
    }

    // Loads an obj file into the scene.
    void LoadOBJModel(std::string fileName, const Surface& material);

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

    unsigned int  AddCamera(const glm::vec3 & eye, const glm::vec3 & at, const glm::vec3 & up);
    void NextCamera();

    void DeleteActiveCamera();
    void SetPerspectiveProjection(PERSPECTIVE_PARAMS projParams);
    void SetOrthoProjection(PROJ_PARAMS projParams);
    void SetFrustum(PROJ_PARAMS projParams);

    glm::mat4x4 GetActiveCameraTransformation();
    glm::mat4x4 GetActiveCameraProjection();

    void ScaleActiveCamera(float value);

    void TranslateActiveCameraAxis(float value, AXES axis);

    void RotateActiveCamera(float angle, AXES axis);
    void RotateActiveCameraRelativeToWorld(float angle, AXES axis);
    

    //Models API:

    int  GetActiveModelIdx();
    void SetActiveModelIdx(unsigned int modelIdx);

    float GetvnScale();
    void SetvnScale(float scale);

    float  GetfnScale();
    void SetfnScale(float scale);

    unsigned int AddPrimitiveModel(PRIM_MODEL primitiveModel, const Surface& material); //TO_DO develop good API
    void NextModel();
    void DeleteActiveModel();

    glm::mat4x4 GetActiveModelTransformation();
    
    void TranslateActiveModel(float value, AXES axis);

    void TranslateModel(Model* activeModel, AXES axis, float value);


    void ScaleActiveModel(float value);

    void ScaleModel(Model* activeModel, float value);

    void RotateActiveModel(float angle, AXES axis);

    void RotateModel(Model* activeModel, AXES axis, float angle);

    void RotateActiveModelRelativeToWorld(float angle, AXES axis);

    void RotateModelRelativeToWorld(Model* activeModel, AXES axis, float angle);

    void showFacesNormals(bool bDrawFaceNormal) { m_bDrawFaceNormal = bDrawFaceNormal; }
    void showVerticesNormals(bool bDrawVecNormal) { m_bDrawVecNormal = bDrawVecNormal; }
    void showBorderCube(bool bShowBorderCube) { m_bShowBorderCube = bShowBorderCube; }

    bool shouldRenderCamera(int cameraIndex);
    glm::vec4 GetBgColor();
    void SetBgColor(const glm::vec4& bgColor);
    glm::vec4 GetPolygonColor();
    void SetPolygonColor(const glm::vec4& polygonColor);
    glm::vec4 GetWireframeColor();
    void SetWireframeColor(const glm::vec4& wireframeColor);

    //Light API:

    int  GetActiveLightIdx();
    void SetActiveLightIdx(unsigned int lightIdx);

    void NextLight();
    void DeleteActiveLight();

    glm::mat4x4 GetActiveLightModelTransformation();

    void TranslateActiveLight(float value, AXES axis);

    void ScaleActiveLightModel(float value);

    void RotateActiveLightModel(float angle, AXES axis);
    void RotateActiveLightModelRelativeToWorld(float angle, AXES axis);

    bool shouldRenderLight();


    int AddLight(LIGHT_SOURCE_TYPE type, const glm::vec3& lightCoord, 
        const glm::vec4& ambiantC, float ambiantI, 
        const glm::vec4& diffusiveC, float diffusiveI, 
        const glm::vec4& specularC, float specularI);

    Light* GetActiveLight();

    void SetShadingType(SHADING_TYPE shading);
    void DrawWireframe(bool bDrawn);
};