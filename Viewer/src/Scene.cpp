#include "Scene.h"
#include "MeshModel.h"
#include "Defs.h"
#include <string>

using namespace std;
void Scene::LoadOBJModel(string fileName)
{
	MeshModel* model = new MeshModel(fileName);
	m_models.push_back(model);
	if (m_activeModel == DISABLED)
	{
		m_activeModel = 0;
	}
}

void Scene::Draw()
{
	// 1. Send the renderer the current camera transform and the projection
	// 2. Tell all models to draw themselves
	if (m_activeCamera == DISABLED)
	{
		Camera* camera = new Camera();
		m_cameras.push_back(camera);
		m_activeCamera = 0;
	}

	//BUG: Sets right transformation only one (suspected), something owerrwrite it!

	Camera* activeCamera = m_cameras[m_activeCamera];
	renderer->SetCameraTransform(activeCamera->GetTransformation());
	renderer->SetProjection(m_cameras[m_activeCamera]->GetProjection());

	for each (Model* model in m_models)
	{
		const vector<glm::vec3>* modelVertices = model->Draw();
		//renderer->SetObjectMatrices();
		renderer->DrawTriangles(modelVertices);
		renderer->SwapBuffers();
		delete modelVertices;
	}
}

void Scene::ScaleActiveCamera(float value)
{
	if (m_activeCamera != DISABLED)
	{
		Camera* activeCamera = m_cameras[m_activeCamera];
		glm::mat4x4 currTransf = activeCamera->GetTransformation();
		glm::mat4x4 scaleTransform(SCALING_MATRIX(value));
		activeCamera->SetTransformation(scaleTransform * currTransf);
	}
}

void Scene::TranslateActiveCameraLeft(float value)
{
	if (m_activeCamera != DISABLED)
	{
		Camera* activeCamera = m_cameras[m_activeCamera];
		glm::mat4x4 currTransf = activeCamera->GetTransformation();
		glm::mat4x4 scaleTransform(TRANSLATION_MATRIX(-value, 0, 0));
		activeCamera->SetTransformation(scaleTransform * currTransf);
	}
}

void Scene::TranslateActiveCameraRight(float value)
{
	if (m_activeCamera != DISABLED)
	{
		Camera* activeCamera = m_cameras[m_activeCamera];
		glm::mat4x4 currTransf = activeCamera->GetTransformation();
		glm::mat4x4 scaleTransform(TRANSLATION_MATRIX(value, 0, 0));
		activeCamera->SetTransformation(scaleTransform * currTransf);
	}
}

void Scene::TranslateActiveCameraUp(float value)
{
	if (m_activeCamera != DISABLED)
	{
		Camera* activeCamera = m_cameras[m_activeCamera];
		glm::mat4x4 currTransf = activeCamera->GetTransformation();
		glm::mat4x4 scaleTransform(TRANSLATION_MATRIX(0, value, 0));
		activeCamera->SetTransformation(scaleTransform * currTransf);
	}
}

void Scene::TranslateActiveCameraDown(float value)
{
	if (m_activeCamera != DISABLED)
	{
		Camera* activeCamera = m_cameras[m_activeCamera];
		glm::mat4x4 currTransf = activeCamera->GetTransformation();
		glm::mat4x4 scaleTransform(TRANSLATION_MATRIX(0, -value, 0));
		activeCamera->SetTransformation(scaleTransform * currTransf);
	}
}
/*
* eye – The position of the camera
* at – The position the camera looks at
* up – The upside(y) direction of the camera
*/
void Scene::AddCamera(const glm::vec4& eye, const glm::vec4& at, const glm::vec4& up)
{
	Camera* newCamera = new Camera(eye, at, up);
	m_cameras.push_back(newCamera);
	if (m_activeCamera != DISABLED)
	{
		m_activeCamera = 0;
	}
}

void Scene::NextCamera()
{
	if (m_activeCamera != DISABLED)
	{
		m_activeCamera = (m_activeCamera + 1) % m_cameras.size();
	}
}

void Scene::PreviousCamera()
{
	if (m_activeCamera != DISABLED)
	{
		m_activeCamera = (m_activeCamera - 1) % m_cameras.size();
	}
}

void Scene::SetActiveCamera(unsigned int cameraIdx)
{
	if (m_cameras.size() > cameraIdx)
	{
		m_activeCamera = cameraIdx;
	}
}

void Scene::SetActiveModel(unsigned int modelIdx)
{
	if (m_models.size() > modelIdx)
	{
		m_activeModel = modelIdx;
	}
}

void Scene::DrawDemo()
{
	renderer->SetDemoBuffer();
	renderer->SwapBuffers();
}
