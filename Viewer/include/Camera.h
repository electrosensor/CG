#pragma once
#include <glm/glm.hpp>
#include "Defs.h"

/*
 * Camera class. This class takes care of all the camera transformations and manipulations.
 */
class Camera
{
private:
	glm::mat4x4 m_cameraTransform;
	glm::mat4x4 m_cameraProjection;
	int m_cameraIdx;

public:
	Camera();
	Camera(const glm::vec4& eye, const glm::vec4& at, const glm::vec4& up);
	~Camera();

	const glm::mat4x4 & GetTransformation();

	const glm::mat4x4 & GetProjection();

	// Sets the camera transformations with relation to world coordinates
	void SetTransformation(const glm::mat4x4& transformation);

	void SetProjection(const glm::mat4x4 & projection);

	// Creates lookAt transformation.
	void LookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up );

	// Sets orthographic projection matrix.
	void Camera::Ortho( PROJ_PARAMS projParams );

	// Sets frustum view volume for clipping stage.
	void Frustum( const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar );

	// Sets perspective projection matrix.
	void Perspective( const float fovy, const float aspect,
		const float zNear, const float zFar);

};
