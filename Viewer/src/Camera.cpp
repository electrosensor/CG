#include "Camera.h"

using namespace std;
using namespace glm;



Camera::Camera(const vec3& eye, const vec3& at, const vec3& up, GLuint prog) : m_cameraTransform(I_MATRIX), m_cameraProjection(I_MATRIX)
{
    m_cameraModel = (PModel) new CamMeshModel(eye, prog);

    LookAt(eye, at, up);
    try 
    {
        Ortho({ -10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f });
    }
    catch (...) {}

}

Camera::Camera(GLuint prog) : Camera(DEFAULT_CAMERA_POSITION, prog) {}

Camera::~Camera()
{
    delete m_cameraModel;
}

const mat4x4& Camera::GetTransformation()
{
    return m_cameraTransform;
}

const mat4x4& Camera::GetProjection()
{
    return m_cameraProjection;
}

void Camera::LookAt(const vec3 & eye, const vec3 & at, const vec3 & up)
{
//     vec3   eyeAtDirection                /*forward*/  = Util::isVecEqual(eye - at, vec3(0, 0, 0)) ? eye - at : -normalize(eye - at);
//     vec3   fromUpAlongCameraTop                       = cross(up, eyeAtDirection);
//     vec3   fromUpAlongCameraTopDirection /*left*/     = Util::isVecEqual(fromUpAlongCameraTop, vec3(0, 0, 0)) ? fromUpAlongCameraTop : normalize(fromUpAlongCameraTop);
//     vec3   cameraView                    /*up*/       = cross(eyeAtDirection, fromUpAlongCameraTopDirection);
//     vec3   cameraViewDirection                        = Util::isVecEqual(cameraView, vec3(0, 0, 0)) ? cameraView : normalize(cameraView);
//     vec4   homogenousComponent                        = vec4(HOMOGENEOUS_VECTOR4);
//     mat4x4 cameraViewTransformation                   = mat4x4(Util::expandToVec4(fromUpAlongCameraTopDirection),
//                                                                          Util::expandToVec4(cameraViewDirection),
//                                                                          Util::expandToVec4(eyeAtDirection),
//                                                                          homogenousComponent);
// 
// 
// 
//     mat4x4 cameraTransformation = transpose(cameraViewTransformation);
// 
//     cameraTransformation[3][0] = -fromUpAlongCameraTopDirection.x * eye.x - fromUpAlongCameraTopDirection.y * eye.y - fromUpAlongCameraTopDirection.z * eye.z;
//     cameraTransformation[3][1] = -cameraView.x * eye.x - cameraView.y * eye.y - cameraView.z * eye.z;
//     cameraTransformation[3][2] = -eyeAtDirection.x * eye.x - eyeAtDirection.y * eye.y - eyeAtDirection.z * eye.z;
// 
// 
//     cameraViewTransformation = inverse(cameraTransformation);

    glm::mat4 View = glm::lookAt(eye, at, up);

    m_cameraTransform = View;
    getCameraModel()->SetModelTransformation(View);
}

void Camera::SetTransformation(const mat4x4 & transform)
{
    m_cameraTransform = transform;
}

void Camera::SetProjection(const mat4x4 & projection)
{
    m_cameraProjection = projection;
}

void Camera::Ortho(const PROJ_PARAMS projParams)
{
    SET_PROJ_PARAMS(projParams);

    m_cameraProjection = glm::ortho(left, right, bottom, top, zNear, zFar);

    m_projParams = projParams;
    
}

void Camera::Frustum(const PROJ_PARAMS projParams)
{
    validateProjParams(projParams);

    SET_PROJ_PARAMS(projParams);
    
    m_cameraProjection = mat4x4(
    {
        {   2.0f*zNear / (right - left)    ,             0                    ,                  0                  ,              0              },
        {                0                 ,   2.0f*zNear / (top - bottom)    ,                  0                  ,              0              },
        { (right + left) / (right - left)  , (top + bottom) / (top - bottom)  ,  -(zFar + zNear) / (zFar - zNear)   ,             -1              },
        {                0                 ,              0                   , -2.0f*zFar*zNear / (zFar - zNear)   ,              0              }
    });

    m_projParams = projParams;

    throw false;
}

void Camera::Perspective(const PERSPECTIVE_PARAMS perspectiveParams)
{
    if(perspectiveParams.fovy == PI / 2.f || perspectiveParams.fovy == 3.f * PI / 2.f) throw true;
    
    PROJ_PARAMS projParams = { 0 };

    float height = perspectiveParams.zNear * tan(perspectiveParams.fovy / 2.0f);
    float width  = height * perspectiveParams.aspect;
   
    projParams.left         = -width;
    projParams.right        = width;
    projParams.top          = height;
    projParams.bottom       = -height;
    projParams.zNear        = perspectiveParams.zNear;
    projParams.zFar         = perspectiveParams.zFar;

    validateProjParams(projParams);
          
    m_cameraProjection = glm::perspective(perspectiveParams.fovy, perspectiveParams.aspect, perspectiveParams.zNear, perspectiveParams.zFar);

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
