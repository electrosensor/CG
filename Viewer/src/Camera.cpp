#include "Camera.h"



Camera::Camera()
{
}


Camera::~Camera()
{
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

	//projection = ST:
	projection = glm::mat4x4( { { 2.0f / (right - left), 0, 0, 0},
								{ 0, 2.0f / (top - bottom), 0, 0},
								{ 0, 0, 2.0f / (zNear - zFar), 0 },
								{ -(right + left) / (right - left), -(bottom + top) / (top - bottom), (zFar + zNear) / (zFar - zNear), 1 } } );
}
