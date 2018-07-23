#include "Scene.h"
#include "MeshModel.h"
#include <string>

using namespace std;
void Scene::LoadOBJModel(string fileName)
{
	MeshModel *model = new MeshModel(fileName);
//TODO: check for OUT_OF_MEMORY
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
	if (m_activeCamera != DISABLED)
	{
//		renderer->SetCameraTransform(m_cameras[m_activeCamera]->GetTransformation());
//		renderer->SetProjection(m_cameras[m_activeCamera]->GetProjection());
	}

	for each (Model* model in m_models)
	{
		const vector<glm::vec3>* modelVertices = model->Draw();
		renderer->DrawTriangles(modelVertices);
		renderer->SwapBuffers();
		delete modelVertices;
	}
}

void Scene::ScaleActiveModel(float modifier)
{
	if (m_activeModel != DISABLED)
	{
		Model* activeModel = m_models[m_activeModel];
		glm::mat4x4 currTransf = activeModel->GetNormalTransform();
		glm::mat4x4 scaleTransform({ { modifier,0,0,0 },{ 0,modifier,0,0 },{ 0,0,modifier,0 },{ 0,0,0,1 } });
		activeModel->SetNormalTransform(scaleTransform * currTransf);
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
