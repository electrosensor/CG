#include "Scene.h"
#include "MeshModel.h"
#include <string>

using namespace std;
void Scene::LoadOBJModel(string fileName)
{
	MeshModel *model = new MeshModel(fileName);
	models.push_back(model);
}

void Scene::Draw()
{
	// 1. Send the renderer the current camera transform and the projection
	// 2. Tell all models to draw themselves

	//renderer->SetCameraTransform(cameras[0]->GetTransformation());
	//renderer->SetProjection(cameras[0]->GetProjection);

	for each (Model* model in models)
	{
		//renderer->SetObjectMatrices(model->GetWorldTransform(), model->GetNormalTransform());
		renderer->DrawTriangles(model->Draw());
		renderer->SwapBuffers();
	}
}

void Scene::DrawDemo()
{
	renderer->SetDemoBuffer();
	renderer->SwapBuffers();
}
