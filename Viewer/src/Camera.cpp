#include "Camera.h"
#include "Defs.h"
#include "Util.h"

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
	
	float right		= projParams.right;
	float left		= projParams.left;
	float top		= projParams.top;
	float bottom		= projParams.bottom;
	float zNear		= projParams.zNear;
	float zFar		= projParams.zFar;
	// Moves center to the origin:
	/*glm::vec3 T( -(right + left) / 2.0f,
				-(bottom + top) / 2.0f,
				(zNear + zFar) / 2.0f );

	// Scale to make sides of length 2:
	glm::vec3 S( 2.0f / (right - left),
				2.0f / (top - bottom),
				2.0f / (zNear - zFar));*/

	//m_cameraProjection = ST:
	m_cameraProjection = glm::mat4x4( { { 2.0f / (right - left), 0, 0, 0 },
										{ 0, 2.0f / (top - bottom), 0, 0 },
										{ 0, 0, 2.0f / (zNear - zFar), 0 },
										{ -(right + left) / (right - left), -(bottom + top) / (top - bottom), (zFar + zNear) / (zFar - zNear), 1 } } );
}

void Camera::Frustum(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar)
{
	//
}

void Camera::Perspective(const float fovy, const float aspect, const float zNear, const float zFar)
{
	//m_cameraProjection = 
}
