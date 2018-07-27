#include "Camera.h"
#include "Defs.h"
#include "Util.h"

Camera::Camera() : m_cameraTransform(I_MATRIX), m_cameraProjection(I_MATRIX)
{
}

Camera::Camera(const glm::vec4& eye, const glm::vec4& at, const glm::vec4& up)
{
	LookAt(eye, at, up);
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

void Camera::LookAt(const glm::vec4 & eye, const glm::vec4 & at, const glm::vec4 & up)
{
	glm::vec4 eyeAtDirection = glm::normalize(eye - at);
	glm::vec4 fromUpAlongCameraTop = Util::Cross(up, eyeAtDirection);
	glm::vec4 fromUpAlongCameraTopDirection = glm::normalize(fromUpAlongCameraTop);
	glm::vec4 cameraView = Util::Cross(eyeAtDirection, fromUpAlongCameraTopDirection);	
	glm::vec4 cameraViewDirection = glm::normalize(cameraView);
	glm::vec4 homogenicComponent = glm::vec4(HOMOGENIC_VECTOR4);
	glm::mat4x4 cameraViewTransformation = glm::mat4x4(fromUpAlongCameraTopDirection, cameraViewDirection, eyeAtDirection, homogenicComponent);

	m_cameraTransform = cameraViewTransformation/* * Translate(-eye)*/;
}

void Camera::SetTransformation(const glm::mat4x4 & transform)
{
	m_cameraTransform = transform;
}

void Camera::SetProjection(const glm::mat4x4 & projection)
{
	m_cameraProjection = projection;
}

void Camera::Ortho(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar)
{
	// Moves center to the origin:
	/*glm::vec3 T( -(right + left) / 2.0f,
				-(bottom + top) / 2.0f,
				(zNear + zFar) / 2.0f );

	// Scale to make sides of length 2:
	glm::vec3 S( 2.0f / (right - left),
				2.0f / (top - bottom),
				2.0f / (zNear - zFar));*/

	//m_cameraProjection = ST:
	m_cameraProjection = glm::mat4x4( { { 2.0f / (right - left), 0, 0, 0},
								{ 0, 2.0f / (top - bottom), 0, 0},
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
