#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Renderer.h"
#include "Model.h"
#include "Light.h"
#include "Camera.h"
#include "Defs.h"

using namespace std;


class Scene {
private:
	vector<Model*> m_models;
	vector<Light*> m_lights;
	vector<Camera*> m_cameras;
	Renderer *renderer;

	int m_activeModel;
	int m_activeLight;
	int m_activeCamera;

public:
	Scene() {};
	Scene(Renderer *renderer) : renderer(renderer), m_activeModel(DISABLED), m_activeLight(DISABLED), m_activeCamera(DISABLED) { ; }

	// Loads an obj file into the scene.
	void LoadOBJModel(string fileName);

	// Draws the current scene.
	void Draw();

//	void SetActiveLight(size_t lightIdx); // to remember add this in the next homework


	//Cameras API:

	int  GetActiveCameraIdx();
	void SetActiveCameraIdx(unsigned int cameraIdx);

	unsigned int  AddCamera(const glm::vec4 & eye, const glm::vec4 & at, const glm::vec4 & up);
    void setPerspectiveProjection(PERSPECTIVE_PARAMS projParams);
    void NextCamera();

	glm::mat4x4 GetActiveCameraTransformation();
	glm::mat4x4 GetActiveCameraProjection();

	void ScaleActiveCamera(float value);

	void TranslateActiveCameraXAxis(float value);
	void TranslateActiveCameraYAxis(float value);
	void TranslateActiveCameraZAxis(float value);

	void RotateActiveCameraXAxis(float angle);
	void RotateActiveCameraYAxis(float angle);
	void RotateActiveCameraZAxis(float angle);
	

	//Models API:

	int  GetActiveModelIdx();
	void SetActiveModelIdx(unsigned int modelIdx);

	unsigned int  AddPrimitiveModel(PRIM_MODEL primitiveModel); //TO_DO develop good API
	void NextModel();

	glm::mat4x4 GetActiveModelWorldTransformation();
	
	void TranslateActiveModelXAxis(float value);
	void TranslateActiveModelYAxis(float value);
	void TranslateActiveModelZAxis(float value);

	void ScaleActiveModel(float value);

	void RotateActiveModelXAxis(float angle);
	void RotateActiveModelYAxis(float angle);
	void RotateActiveModelZAxis(float angle);
	
	void setOrthoProjection(PROJ_PARAMS projParams);
    void setFrustum(PROJ_PARAMS projParams);
};