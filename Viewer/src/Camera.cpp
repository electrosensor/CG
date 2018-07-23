#include "Camera.h"
#include "Defs.h"


Camera::Camera() : m_cTransform(I_MATRIX), m_projection(I_MATRIX)
{
}


Camera::~Camera()
{
}

const glm::mat4x4& Camera::GetTransformation()
{
	return m_cTransform;
}

const glm::mat4x4& Camera::GetProjection()
{
	return m_projection;
}

void Camera::SetTransformation(const glm::mat4x4 & transform)
{
	m_cTransform = transform;
}

void Camera::SetProjection(const glm::mat4x4 & projection)
{
	m_projection = projection;
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

	//m_projection = ST:
	m_projection = glm::mat4x4( { { 2.0f / (right - left), 0, 0, 0},
								{ 0, 2.0f / (top - bottom), 0, 0},
								{ 0, 0, 2.0f / (zNear - zFar), 0 },
								{ -(right + left) / (right - left), -(bottom + top) / (top - bottom), (zFar + zNear) / (zFar - zNear), 1 } } );
}
