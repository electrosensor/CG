#pragma once
#include <glm/glm.hpp>

/*
 * Camera class. This class takes care of all the camera transformations and manipulations.
 */
class Camera
{
private:
	glm::mat4x4 m_cTransform;
	glm::mat4x4 m_projection;

public:
	Camera();
	~Camera();

	const glm::mat4x4 & GetTransformation();

	const glm::mat4x4 & GetProjection();

	// Sets the camera transformations with relation to world coordinates
	void SetTransformation(const glm::mat4x4& transformation);

	void SetProjection(const glm::mat4x4 & projection);

	// Creates lookAt transformation.
	void LookAt(const glm::vec4& eye, const glm::vec4& at, const glm::vec4& up );

	// Sets orthographic projection matrix.
	void Ortho( const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar );

	// Sets frustum view volume for clipping stage.
	void Frustum( const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar );

	// Sets perspective projection matrix.
	glm::mat4x4 Perspective( const float fovy, const float aspect,
		const float zNear, const float zFar);

};
