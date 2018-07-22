#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Renderer.h"
#include "Model.h"
#include "Light.h"
#include "Camera.h"

using namespace std;

//YURI: move after creating a global definitions h file
// Global project definitions:

#define	NOT_ACTIVE -1


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
	Scene(Renderer *renderer) : renderer(renderer), m_activeModel(NOT_ACTIVE), m_activeLight(NOT_ACTIVE), m_activeCamera(NOT_ACTIVE) { ; }

	// Loads an obj file into the scene.
	void LoadOBJModel(string fileName);

	// Draws the current scene.
	void Draw();

	void ScaleActiveModel(float modifier);

	void SetActiveCamera(unsigned int cameraIdx);
	void SetActiveModel(unsigned int modelIdx);
//	void SetActiveLight(size_t lightIdx); // to remember add this in the next homework

	// Draws an example.
	void DrawDemo();
	
};