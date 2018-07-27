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




	int  GetActiveCameraIdx();
	void SetActiveCameraIdx(unsigned int cameraIdx);
	int  GetActiveModelIdx();
	void SetActiveModelIdx(unsigned int modelIdx);
//	void SetActiveLight(size_t lightIdx); // to remember add this in the next homework


	// Draws an example.
	void DrawDemo();
	
	//GUI API:
	unsigned int  AddCamera(const glm::vec4 & eye, const glm::vec4 & at, const glm::vec4 & up);
	glm::mat4x4 GetActiveCameraTransformation();
	glm::mat4x4 GetActiveCameraProjection();
	void NextCamera();
	void TranslateActiveCameraLeft(float value);
	void TranslateActiveCameraRight(float value);
	void TranslateActiveCameraUp(float value);
	void TranslateActiveCameraDown(float value);

	void RotateActiveCameraXAxis(float angle);
	void RotateActiveCameraYAxis(float angle);
	void RotateActiveCameraZAxis(float angle);
	
	
	void ScaleActiveCamera(float value);
};