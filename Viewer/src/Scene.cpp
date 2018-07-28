#include "Scene.h"
#include "MeshModel.h"
#include <string>

using namespace std;
void Scene::LoadOBJModel(string fileName)
{
	MeshModel* model = new MeshModel(fileName);
	m_models.push_back(model);
	m_activeModel++;
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

//////////////////// Camera ////////////////////////
/*
* Adds camera to vector of scene cameras.
* eye – The position of the camera
* at – The position the camera looks at
* up – The upside(y) direction of the camera
* returns: current index of added camera.
*/
unsigned int Scene::AddCamera(const glm::vec4& eye, const glm::vec4& at, const glm::vec4& up)
{
	Camera* newCamera = new Camera(eye, at, up);
	m_cameras.push_back(newCamera);
	return m_cameras.size() - 1;
}

glm::mat4x4 Scene::GetActiveCameraTransformation()
{
	if (m_activeCamera != DISABLED)
	{
		Camera* activeCamera = m_cameras[m_activeCamera];
		return  activeCamera->GetTransformation();
	}
	else
	{
		return ZERO_MATRIX;
	}
}

glm::mat4x4 Scene::GetActiveCameraProjection()
{
	if (m_activeCamera != DISABLED)
	{
		Camera* activeCamera = m_cameras[m_activeCamera];
		return  activeCamera->GetProjection();
	}
	else
	{
		return ZERO_MATRIX;
	}
}

void Scene::NextCamera()
{
	if (m_activeCamera != DISABLED)
	{
		m_activeCamera = (m_activeCamera + 1) % m_cameras.size();
	}
}

int Scene::GetActiveCameraIdx()
{
	return m_activeCamera;
}

void Scene::SetActiveCameraIdx(unsigned int cameraIdx)
{
	if (m_cameras.size() > cameraIdx)
	{
		m_activeCamera = cameraIdx;
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

void Scene::TranslateActiveCameraXAxis(float value)
{
	if (m_activeCamera != DISABLED)
	{
		Camera* activeCamera = m_cameras[m_activeCamera];
		glm::mat4x4 currTransf = activeCamera->GetTransformation();
		glm::mat4x4 translateTransform(TRANSLATION_MATRIX(value, 0, 0));
		activeCamera->SetTransformation(translateTransform * currTransf);
	}
}

void Scene::TranslateActiveCameraYAxis(float value)
{
	if (m_activeCamera != DISABLED)
	{
		Camera* activeCamera = m_cameras[m_activeCamera];
		glm::mat4x4 currTransf = activeCamera->GetTransformation();
		glm::mat4x4 translateTransform(TRANSLATION_MATRIX(0, value, 0));
		activeCamera->SetTransformation(translateTransform * currTransf);
	}
}

void Scene::TranslateActiveCameraZAxis(float value)
{
	if (m_activeCamera != DISABLED)
	{
		Camera* activeCamera = m_cameras[m_activeCamera];
		glm::mat4x4 currTransf = activeCamera->GetTransformation();
		glm::mat4x4 translateTransform(TRANSLATION_MATRIX(0, 0, value));
		activeCamera->SetTransformation(translateTransform * currTransf);
	}
}

void Scene::RotateActiveCameraXAxis(float angle)
{
	if (m_activeCamera != DISABLED)
	{
		Camera* activeCamera = m_cameras[m_activeCamera];
		glm::mat4x4 currTransf = activeCamera->GetTransformation();
		glm::mat4x4 toOrigin(TRANSLATION_MATRIX(-currTransf[3][0], -currTransf[3][1], -currTransf[3][2]));
		glm::mat4x4 toPlace(TRANSLATION_MATRIX(currTransf[3][0], currTransf[3][1], currTransf[3][2]));
		glm::mat4x4 rotateTransform(ROTATING_MATRIX_X_AXIS(angle));
		activeCamera->SetTransformation(toPlace * rotateTransform * toOrigin * currTransf);
	}
}

void Scene::RotateActiveCameraYAxis(float angle)
{
	if (m_activeCamera != DISABLED)
	{
		Camera* activeCamera = m_cameras[m_activeCamera];
		glm::mat4x4 currTransf = activeCamera->GetTransformation();
		glm::mat4x4 toOrigin(TRANSLATION_MATRIX(-currTransf[3][0], -currTransf[3][1], -currTransf[3][2]));
		glm::mat4x4 toPlace(TRANSLATION_MATRIX(currTransf[3][0], currTransf[3][1], currTransf[3][2]));
		glm::mat4x4 rotateTransform(ROTATING_MATRIX_Y_AXIS(angle));
		activeCamera->SetTransformation(toPlace * rotateTransform * toOrigin * currTransf);
	}
}

void Scene::RotateActiveCameraZAxis(float angle)
{
	if (m_activeCamera != DISABLED)
	{
		Camera* activeCamera = m_cameras[m_activeCamera];
		glm::mat4x4 currTransf = activeCamera->GetTransformation();
		glm::mat4x4 toOrigin(TRANSLATION_MATRIX(-currTransf[3][0], -currTransf[3][1], -currTransf[3][2]));
		glm::mat4x4 toPlace(TRANSLATION_MATRIX(currTransf[3][0], currTransf[3][1], currTransf[3][2]));
		glm::mat4x4 rotateTransform(ROTATING_MATRIX_Z_AXIS(angle));
		activeCamera->SetTransformation(toPlace * rotateTransform * toOrigin * currTransf);
	}
}
//////////////////// Model ////////////////////////
void Scene::TranslateActiveModelXAxis(float value)
{
	if (m_activeModel != DISABLED)
	{
		Model* activeModel = m_models[m_activeModel];
		glm::mat4x4 currTransf = activeModel->GetWorldTransformation();
		glm::mat4x4 translateTransform(TRANSLATION_MATRIX(value, 0, 0));
		activeModel->SetWorldTransformation(translateTransform * currTransf);
	}
}

void Scene::TranslateActiveModelYAxis(float value)
{
	if (m_activeModel != DISABLED)
	{
		Model* activeModel = m_models[m_activeModel];
		glm::mat4x4 currTransf = activeModel->GetWorldTransformation();
		glm::mat4x4 translateTransform(TRANSLATION_MATRIX(0, value, 0));
		activeModel->SetWorldTransformation(translateTransform * currTransf);
	}
}

void Scene::TranslateActiveModelZAxis(float value)
{
	if (m_activeModel != DISABLED)
	{
		Model* activeModel = m_models[m_activeModel];
		glm::mat4x4 currTransf = activeModel->GetWorldTransformation();
		glm::mat4x4 translateTransform(TRANSLATION_MATRIX(0, 0, value));
		activeModel->SetWorldTransformation(translateTransform * currTransf);
	}
}

void Scene::ScaleActiveModel(float value)
{
	if (m_activeModel != DISABLED)
	{
		Model* activeCamera = m_models[m_activeModel];
		glm::mat4x4 currTransf = activeCamera->GetWorldTransformation();
		glm::mat4x4 scaleTransform(SCALING_MATRIX(value));
		activeCamera->SetWorldTransformation(currTransf * scaleTransform);
	}
}

void Scene::RotateActiveModelXAxis(float angle)
{
	if (m_activeModel != DISABLED)
	{
		Model* activeModel = m_models[m_activeModel];
		glm::mat4x4 currTransf = activeModel->GetWorldTransformation();
		glm::mat4x4 toOrigin (TRANSLATION_MATRIX(-currTransf[3][0], -currTransf[3][1], -currTransf[3][2]));
		glm::mat4x4 toPlace(TRANSLATION_MATRIX(currTransf[3][0], currTransf[3][1], currTransf[3][2]));
		glm::mat4x4 rotateTransform(ROTATING_MATRIX_X_AXIS(angle));
		activeModel->SetWorldTransformation(toPlace * rotateTransform * toOrigin * currTransf);
	}
}

void Scene::RotateActiveModelYAxis(float angle)
{
	if (m_activeModel != DISABLED)
	{
		Model* activeModel = m_models[m_activeModel];
		glm::mat4x4 currTransf = activeModel->GetWorldTransformation();
		glm::mat4x4 toOrigin(TRANSLATION_MATRIX(-currTransf[3][0], -currTransf[3][1], -currTransf[3][2]));
		glm::mat4x4 toPlace(TRANSLATION_MATRIX(currTransf[3][0], currTransf[3][1], currTransf[3][2]));
		glm::mat4x4 rotateTransform(ROTATING_MATRIX_Y_AXIS(angle));
		activeModel->SetWorldTransformation(toPlace * rotateTransform * toOrigin * currTransf);
	}
}

void Scene::RotateActiveModelZAxis(float angle)
{
	if (m_activeModel != DISABLED)
	{
		Model* activeModel = m_models[m_activeModel];
		glm::mat4x4 currTransf = activeModel->GetWorldTransformation();
		glm::mat4x4 toOrigin(TRANSLATION_MATRIX(-currTransf[3][0], -currTransf[3][1], -currTransf[3][2]));
		glm::mat4x4 toPlace(TRANSLATION_MATRIX(currTransf[3][0], currTransf[3][1], currTransf[3][2]));
		glm::mat4x4 rotateTransform(ROTATING_MATRIX_Z_AXIS(angle));
		activeModel->SetWorldTransformation(toPlace * rotateTransform * toOrigin * currTransf);
	}
}

unsigned int Scene::AddPrimitiveModel(PRIM_MODEL primitiveModel)
{
	Model* newModel = new PrimMeshModel(primitiveModel);
	m_models.push_back(newModel);
	return m_models.size() - 1;
}

void Scene::NextModel()
{
	if (m_activeModel != DISABLED)
	{
		m_activeModel = (m_activeModel + 1) % m_models.size();
	}
}

glm::mat4x4 Scene::GetActiveModelWorldTransformation()
{
	if (m_activeCamera != DISABLED)
	{
		Model* activeModel = m_models[m_activeModel];
		return  activeModel->GetWorldTransformation();
	}
	else
	{
		return ZERO_MATRIX;
	}
}

int Scene::GetActiveModelIdx()
{
	return m_activeModel;
}

void Scene::SetActiveModelIdx(unsigned int modelIdx)
{
	if (m_models.size() > modelIdx)
	{
		m_activeModel = modelIdx;
	}
}