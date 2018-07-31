#include "Camera.h"

using namespace std;

Camera::Camera() : m_cameraTransform(I_MATRIX), m_cameraProjection(I_MATRIX)
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

void Camera::LookAt(const glm::vec3 & eye, const glm::vec3 & at, const glm::vec3 & up)
{
    glm::vec3   eyeAtDirection   /*forward*/               = glm::normalize(eye - at);
    glm::vec3   fromUpAlongCameraTop            = glm::cross(up, eyeAtDirection);
    glm::vec3   fromUpAlongCameraTopDirection /*left*/  = glm::normalize(fromUpAlongCameraTop);
    glm::vec3   cameraView            /*up*/          = glm::cross(eyeAtDirection, fromUpAlongCameraTopDirection);
    glm::vec3   cameraViewDirection             = glm::normalize(cameraView);
    glm::vec4   homogenousComponent             = glm::vec4(HOMOGENIC_VECTOR4);

    glm::mat4x4 cameraViewTransformation        = glm::mat4x4(Util::expandToVec4(fromUpAlongCameraTopDirection),
                                                              Util::expandToVec4(cameraViewDirection),
                                                              Util::expandToVec4(eyeAtDirection),
                                                              homogenousComponent);
    cameraViewTransformation[0][3] = -fromUpAlongCameraTopDirection.x * eye.x
                                     -fromUpAlongCameraTopDirection.y * eye.y
                                     -fromUpAlongCameraTopDirection.z * eye.z;
    cameraViewTransformation[1][3] = -cameraViewDirection.x * eye.x
                                     -cameraViewDirection.y * eye.y
                                     -cameraViewDirection.z * eye.z;
    cameraViewTransformation[2][3] = -eyeAtDirection.x * eye.x
                                     -eyeAtDirection.y * eye.y
                                     -eyeAtDirection.z * eye.z;

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

        {      2.0f       / (right - left),                 0               ,                0               , 0 },
        {                 0               ,      2.0f       / (top - bottom),                0               , 0 },
        {                 0               ,                 0               ,       2.0f     / (zNear - zFar), 0 },
        { -(right + left) / (right - left), -(bottom + top) / (top - bottom), (zFar + zNear) / (zFar - zNear), 1 }

    });
}

void Camera::Frustum(const PROJ_PARAMS projParams)
{
    validateProjParams(projParams);

    SET_PROJ_PARAMS(projParams);
    
    m_cameraProjection = glm::mat4x4(
    {
        {   2.0f*zNear / (right - left)    ,             0                    ,                  0                  ,                  0              },
        {                0                 ,   2.0f*zNear / (top - bottom)    ,                  0                  ,                  0              },
        { (right + left) / (right - left)  , (top + bottom) / (top - bottom)  ,  -(zFar + zNear) / (zFar - zNear)   ,                 -1              },
        {                0                 ,              0                   , -2.0f*zFar*zNear / (zFar - zNear)   ,                  0              }
    });

    m_frustumParams = projParams;



//     glm::mat4x4 H = glm::mat4x4(
//         {
//             {                1              ,                0              ,              0              ,           0           },
//             {                0              ,                1              ,              0              ,           0           },
//             { (right + left) / -2.0f*zNear  , (top + bottom) / -2.0f*zNear  ,              1              ,           0           },
//             {                0              ,                0              ,              0              ,           1           }
//         });
// 
//     glm::mat4x4 S = glm::mat4x4(
//         {
//             { -2.0f*zNear / (right - left),             0               ,                0              ,           0           },
//             {               0             , -2.0f*zNear / (top - bottom),                0              ,           0           },
//             {               0             ,             0               ,                1              ,           0           },
//             {               0             ,             0               ,                0              ,           1           }
//         });
// 
//     zNear = -1.0f;
//     zFar = 1.0f;
//     float alpha = (zNear + zFar) / (zNear - zFar);
//     float beta = (2.0f * zNear * zFar) / (zNear - zFar);
// 
//     glm::mat4x4 N = glm::mat4x4(
//         {
//             {                   1             ,             0               ,                0              ,           0       },
//             {                   0             ,             1               ,                0              ,           0       },
//             {                   0             ,             0               ,              alpha            ,          -1       },
//             {                   0             ,             0               ,               beta            ,           0       }
//         });
// 
//     m_cameraProjection = N * S * H;
}

void Camera::Perspective(const PERSPECTIVE_PARAMS perspectiveParams)
{
   // SET_PERSP_PARAMS(perspectiveParams);

    float height = perspectiveParams.zNear * tan(TO_RADIAN(perspectiveParams.fovy / 2.0f));
    float width = height * perspectiveParams.aspect;

    PROJ_PARAMS projParams = { 0 };
    projParams.left = -width;
    projParams.right = width;
    projParams.bottom = -height;
    projParams.top = height;
    projParams.zNear = perspectiveParams.zNear;
    projParams.zFar = perspectiveParams.zFar;

    Frustum(projParams);

  SET_PROJ_PARAMS(m_frustumParams);



    glm::mat4x4 H = glm::mat4x4(
    {
        {           1           ,           0           , (right + left) / -2.0f*zNear  ,           0           },
        {           0           ,           1           , (top + bottom) / -2.0f*zNear  ,           0           },
        {           0           ,           0           ,                1              ,           0           },
        {           0           ,           0           ,                0              ,           1           }
    });

    glm::mat4x4 S = glm::mat4x4(
    {
        { -2.0f*zNear / (right - left),             0               ,                0              ,           0           },
        {               0             , -2.0f*zNear / (top - bottom),                0              ,           0           },
        {               0             ,             0               ,                1              ,           0           },
        {               0             ,             0               ,                0              ,           1           }
    });

    zNear = -1;
    zFar = 1;
    float alpha = (zNear + zFar) / (zNear - zFar);
    float beta  = (2.0f * zNear * zFar) / (zNear - zFar);

    glm::mat4x4 N = glm::mat4x4(
    {
        {                   1             ,             0               ,                0              ,           0       },
        {                   0             ,             1               ,                0              ,           0       },
        {                   0             ,             0               ,              alpha            ,          beta     },
        {                   0             ,             0               ,               -1              ,           0       }
    });
    
   m_cameraProjection = N * S * H;
}

void Camera::validateProjParams(PROJ_PARAMS projParams)
{
    SET_PROJ_PARAMS(projParams);

    if (right - left == 0 || top - bottom == 0 || zFar - zNear == 0)
    {
//         throw string("Illegal Params");
    }


}
