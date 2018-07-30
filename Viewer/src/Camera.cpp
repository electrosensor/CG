#include "Camera.h"
#include "Defs.h"
#include "Util.h"

Camera::Camera() : m_cameraTransform(I_MATRIX), m_cameraProjection(I_MATRIX), m_frustum(I_MATRIX)
{
    glm::mat4x4 toCenter(TRANSLATION_MATRIX(DEFAULT_WIDTH / 2, DEFAULT_HEIGHT / 2, 0));

    m_cameraTransform =  m_cameraTransform * toCenter;
}

Camera::Camera(const glm::vec4& eye, const glm::vec4& at, const glm::vec4& up)
{
    LookAt(eye, at, {0, 1, 0});
}

Camera::~Camera()
{
}

const glm::mat4x4& Camera::GetTransformation()
{
    return m_cameraTransform;
}

const glm::mat4x4& Camera::GetProjection()
{
    return m_cameraProjection;
}

const glm::mat4x4& Camera::GetFrustum()
{
    return m_frustum;
}

void Camera::LookAt(const glm::vec3 & eye, const glm::vec3 & at, const glm::vec3 & up)
{
    glm::vec3 eyeAtDirection = glm::normalize(eye - at);
    glm::vec3 fromUpAlongCameraTop = glm::cross(up, eyeAtDirection);
    glm::vec3 fromUpAlongCameraTopDirection = glm::normalize(fromUpAlongCameraTop);
    glm::vec3 cameraView = glm::cross(eyeAtDirection, fromUpAlongCameraTopDirection);
    glm::vec3 cameraViewDirection = glm::normalize(cameraView);
    glm::vec4 homogenicComponent = glm::vec4(HOMOGENIC_VECTOR4);
    glm::mat4x4 cameraViewTransformation = glm::mat4x4(	Util::expandToVec4(fromUpAlongCameraTopDirection),
                                                        Util::expandToVec4(cameraViewDirection),
                                                        Util::expandToVec4(eyeAtDirection),
                                                        homogenicComponent);
    glm::mat4x4 translatedEye(TRANSLATION_MATRIX(-eye.x, -eye.y, -eye.z));
    m_cameraTransform = cameraViewTransformation * translatedEye;
}

void Camera::SetTransformation(const glm::mat4x4 & transform)
{
    m_cameraTransform = transform;
}

void Camera::SetProjection(const glm::mat4x4 & projection)
{
    m_cameraProjection = projection;
}

void Camera::Ortho(const PROJ_PARAMS projParams)
{
    SET_PROJ_PARAMS(projParams);

    m_cameraProjection = glm::mat4x4(
    {

        {      2.0f       / (right - left),                 0               ,                0               , 0 },
        {                 0               ,      2.0f       / (top - bottom),                0               , 0 },
        {                 0               ,                 0               ,       2.0f     / (zNear - zFar), 0 },
        { -(right + left) / (right - left), -(bottom + top) / (top - bottom), (zFar + zNear) / (zFar - zNear), 1 }

    });
}

void Camera::Frustum(const PROJ_PARAMS projParams)
{
    SET_PROJ_PARAMS(projParams);

    m_frustum = glm::mat4x4(
    {

        {2.0f / (right - left),           0          , (right + left) / (right - left),             0             },
        {           0         , 2.0f / (top - bottom), (top + bottom) / (top - bottom),             0             },
        {           0         ,           0          ,-(zNear + zFar) / (zNear - zFar), -2.0f*zNear / (zFar-zNear)},
        {           0         ,           0          ,                -1              ,             0             }

    });

}

void Camera::Perspective(const PERSPECTIVE_PARAMS perspectiveParams)
{
    //m_cameraProjection = 
}
