#pragma once

#include "Defs.h"
#include "Util.h"
#include "MeshModel.h"


/*
 * Camera class. This class takes care of all the camera transformations and manipulations.
 */
class Camera
{
private:
    glm::mat4x4 m_cameraTransform;
    glm::mat4x4 m_cameraProjection;
    PROJ_PARAMS m_projParams;
    int m_cameraIdx;
    PModel m_cameraModel;
    

public:
    Camera(GLuint prog);
    Camera(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up, GLuint prog);
    ~Camera();

    const glm::mat4x4 & GetTransformation();

    const glm::mat4x4 & GetProjection();

    // Sets the camera transformations with relation to world coordinates
    void SetTransformation(const glm::mat4x4& transformation);

    void SetProjection(const glm::mat4x4 & projection);

    // Creates lookAt transformation.
    void LookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up );

    // Sets orthographic projection matrix.
    void Ortho(const PROJ_PARAMS projParams );

    // Sets frustum view volume for clipping stage.
    void Frustum(const PROJ_PARAMS projParams);

    // Sets perspective projection matrix.
    void Perspective(const PERSPECTIVE_PARAMS perspectiveParams);

    void setCameraModel(Model* camModel) { m_cameraModel = camModel; }
    
    PModel getCameraModel() { return m_cameraModel; }

    PROJ_PARAMS getProjectionParams()
    {
        return m_projParams;
    }

private:
    void validateProjParams(PROJ_PARAMS projParams);
};
