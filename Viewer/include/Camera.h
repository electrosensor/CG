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
	glm::mat4x4 m_frustum;
    PROJ_PARAMS m_frustumParams;
	int m_cameraIdx;

public:
	Camera();
	Camera(const glm::vec4& eye, const glm::vec4& at, const glm::vec4& up);
	~Camera();

	const glm::mat4x4 & GetTransformation();

	const glm::mat4x4 & GetProjection();

    const glm::mat4x4 & GetFrustum();
    // Sets the camera transformations with relation to world coordinates
	void SetTransformation(const glm::mat4x4& transformation);

	void SetProjection(const glm::mat4x4 & projection);

	// Creates lookAt transformation.
	void LookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up );

	// Sets orthographic projection matrix.
	void Ortho(const PROJ_PARAMS projParams );

	// Sets frustum view volume for clipping stage.
	void Frustum(const PROJ_PARAMS projParams);

	// Sets perspective projection matrix.
	void Perspective(const PERSPECTIVE_PARAMS perspectiveParams);

};
