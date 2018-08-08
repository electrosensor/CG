#include "Camera.h"

using namespace std;

Camera::Camera() : m_cameraTransform(I_MATRIX), m_cameraProjection(I_MATRIX)
{
    m_cameraModel = (PModel) new CamMeshModel(HOMOGENEOUS_VECTOR4);
//     PROJ_PARAMS initProj;
//     initProj.left = -100;
//     initProj.right = 100;
//     initProj.top = 100;
//     initProj.bottom = -100;
//     initProj.zFar = 100;
//     initProj.zNear = 1;
//     Ortho(initProj);

   // glm::mat4x4 toCenter(TRANSLATION_MATRIX(DEFAULT_WIDTH / 2, DEFAULT_HEIGHT / 2, 0));

   // m_cameraTransform =  m_cameraTransform/* * toCenter*/;
}

Camera::Camera(const glm::vec4& eye, const glm::vec4& at, const glm::vec4& up) : m_cameraTransform(I_MATRIX), m_cameraProjection(I_MATRIX)
{
    LookAt(eye, at, up);
    m_cameraModel = (PModel) new CamMeshModel(eye);
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

void Camera::LookAt(const glm::vec3 & eye, const glm::vec3 & at, const glm::vec3 & up)
{
    glm::vec3   eyeAtDirection                /*forward*/  = glm::normalize(eye - at);
    glm::vec3   fromUpAlongCameraTop                       = glm::cross(up, eyeAtDirection);
    glm::vec3   fromUpAlongCameraTopDirection /*left*/     = glm::normalize(fromUpAlongCameraTop);
    glm::vec3   cameraView                    /*up*/       = glm::cross(eyeAtDirection, fromUpAlongCameraTopDirection);
    glm::vec3   cameraViewDirection                        = glm::normalize(cameraView);
    glm::vec4   homogenousComponent                        = glm::vec4(HOMOGENEOUS_VECTOR4);
    glm::mat4x4 cameraViewTransformation                   = glm::mat4x4(Util::expandToVec4(fromUpAlongCameraTopDirection),
                                                                         Util::expandToVec4(cameraViewDirection),
                                                                         Util::expandToVec4(eyeAtDirection),
                                                                         homogenousComponent);

    cameraViewTransformation[0][3] -= eye.x;
    cameraViewTransformation[1][3] -= eye.y;
    cameraViewTransformation[2][3] -= eye.z;

    m_cameraTransform = cameraViewTransformation;
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

        {      2.0f       / (right - left) ,                 0                ,                 0                   ,              0              },
        {                 0                ,      2.0f       / (top - bottom) ,                 0                   ,              0              },
        {                 0                ,                 0                ,        2.0f     / (zNear - zFar)    ,              0              },
        { -(right + left) / (right - left) , -(bottom + top) / (top - bottom) , -(zFar + zNear) / (zFar - zNear)    ,              1              }

    });
}

void Camera::Frustum(const PROJ_PARAMS projParams)
{
    validateProjParams(projParams);

    SET_PROJ_PARAMS(projParams);
    
    m_cameraProjection = glm::mat4x4(
    {
        {   2.0f*zNear / (right - left)    ,             0                    ,                  0                  ,              0              },
        {                0                 ,   2.0f*zNear / (top - bottom)    ,                  0                  ,              0              },
        { (right + left) / (right - left)  , (top + bottom) / (top - bottom)  ,  -(zFar + zNear) / (zFar - zNear)   ,             -1              },
        {                0                 ,              0                   , -2.0f*zFar*zNear / (zFar - zNear)   ,              0              }
    });

    m_frustumParams = projParams;

    throw false;
}

void Camera::Perspective(const PERSPECTIVE_PARAMS perspectiveParams)
{

    PROJ_PARAMS projParams  = { 0 };
    float       height      = perspectiveParams.zNear * tan(TO_RADIAN(perspectiveParams.fovy) / 2.0f);
    float       width       = height * perspectiveParams.aspect;

    projParams.left         = -width;
    projParams.right        = width;
    projParams.bottom       = -height;
    projParams.top          = height;
    projParams.zNear        = perspectiveParams.zNear;
    projParams.zFar         = perspectiveParams.zFar;

    validateProjParams(projParams);
          
    Frustum(projParams);

    throw false;
}

void Camera::validateProjParams(PROJ_PARAMS projParams)
{
    SET_PROJ_PARAMS(projParams);

    if (right - left == 0 || top - bottom == 0 || zFar - zNear == 0)
    {
        throw true;
    }
}
