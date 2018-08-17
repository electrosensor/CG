#include "Scene.h"
#include "MeshModel.h"

using namespace std;
using namespace glm;

#define IS_CAMERA true

void Scene::LoadOBJModel(string fileName)
{
    auto* model = new MeshModel(fileName);
    if (fileName != CAMERA_OBJ_FILE)
    {
        m_models.push_back(model);
        m_activeModel++;
    }
}

void Scene::Draw()
{
    // 1. Send the renderer the current camera transform and the projection
    // 2. Tell all models to draw themselves
    Camera* activeCamera = nullptr;

    vector<vec4> vPositions;//TODO: RESERVE
    vector<vec4> vVertices;
    vector<vec4> vVerticesNormals;


    if (m_activeCamera != DISABLED)
    {
        activeCamera = m_cameras[m_activeCamera];
    }
    else
    {
        m_cameras.push_back(new Camera());
        activeCamera = m_cameras.front();
        m_activeCamera++;
    }

    renderer->SetCameraTransform(activeCamera->GetTransformation());
    renderer->SetProjection(activeCamera->GetProjection());

    renderer->setProjectionParams(activeCamera->getProjectionParams());

    renderer->SetBgColor(m_bgColor);
    renderer->SetPolygonColor(m_polygonColor);
    renderer->SetWireframeColor(m_wireframeColor);
    renderer->setSolidColor(m_bShowSolidColor);
    //renderer->drawAxis();

    for each (Model* model in m_models)
    {
        tuple modelData(vPositions, vVertices, vVerticesNormals);
        model->Draw(modelData);
        tie(vPositions, vVertices, vVerticesNormals) = modelData;

        renderer->SetObjectMatrices(model->GetModelTransformation(), model->GetNormalTransformation());
        renderer->setWorldTransformation(m_worldTransformation); 
        renderer->DrawTriangles(vPositions, m_bDrawFaceNormal, &model->getCentroid(), m_fnScaleFactor);

        if (m_bDrawVecNormal && !vVerticesNormals.empty())
        {
            renderer->drawVerticesNormals(vVertices , vVerticesNormals, m_vnScaleFactor);
        }
        
        if (m_bShowBorderCube)
        {
            renderer->drawBordersCube(model->getBordersCube());
        }
    }

    for each(Camera* camera in m_cameras)
    {
        auto camModel = (CamMeshModel*) camera->getCameraModel();
        if (camModel->isModelRenderingActive() && camera != activeCamera)
        {
            vVerticesNormals.reserve(0);
            tuple camModelData(vPositions, vVertices, vVerticesNormals);

            renderer->setWorldTransformation(m_worldTransformation);

            mat4x4 cameraModelTransformation = camModel->GetModelTransformation();

            renderer->SetObjectMatrices(cameraModelTransformation, mat4x4(I_MATRIX));

            camModel->Draw(camModelData);

            renderer->DrawTriangles(get<TUPLE_POSITIONS>(camModelData), FALSE, NULL, 1, IS_CAMERA);
        }
    }
    
    renderer->SwapBuffers();

}

mat4x4 Scene::GetWorldTransformation()
{
    return m_worldTransformation;
}

void Scene::SetWorldTransformation(const mat4x4 world)
{
    m_worldTransformation = world;
}

//////////////////// Camera ////////////////////////
/*
* Adds camera to vector of scene cameras.
* eye – The position of the camera
* at – The position the camera looks at
* up – The upside(y) direction of the camera
* returns: current index of added camera.
*/
unsigned int Scene::AddCamera(const vec4& eye, const vec4& at, const vec4& up)
{
    auto* newCamera = new Camera(eye, at, up);
    m_cameras.push_back(newCamera);
    return (unsigned)m_cameras.size() - 1;
}

void Scene::SetPerspectiveProjection(PERSPECTIVE_PARAMS projParams)
{
    if (m_activeCamera != DISABLED)
    {
        Camera* activeCamera = m_cameras[m_activeCamera];
        activeCamera->Perspective(projParams);
    }
}

void Scene::SetOrthoProjection(PROJ_PARAMS projParams)
{
    if (m_activeCamera != DISABLED)
    {
        Camera* activeCamera = m_cameras[m_activeCamera];
        activeCamera->Ortho(projParams);
    }
}

void Scene::SetFrustum(PROJ_PARAMS projParams)
{
    if (m_activeCamera != DISABLED)
    {
        Camera* activeCamera = m_cameras[m_activeCamera];
        activeCamera->Frustum(projParams);
    }
}


mat4x4 Scene::GetActiveCameraTransformation()
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

mat4x4 Scene::GetActiveCameraProjection()
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

void Scene::DeleteActiveCamera()
{
    if (m_activeCamera != DISABLED)
    {
        m_cameras.erase(m_cameras.begin() + m_activeCamera);
        m_activeCamera = (unsigned) m_cameras.size() - 1;
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
        Camera* p_activeCamera = m_cameras[m_activeCamera];
        mat4x4 currTransf = p_activeCamera->GetTransformation();
        mat4x4 scaleTransform(SCALING_MATRIX4(value));
        p_activeCamera->SetTransformation(inverse(scaleTransform * inverse(currTransf)));
    }
}


// void ApplyTransformation(point, )


void Scene::TranslateActiveCameraAxis(float value, AXES axis)
{
    if (m_activeCamera != DISABLED)
    {
        Camera* p_activeCamera = m_cameras[m_activeCamera];
        mat4x4 currTransf = p_activeCamera->GetTransformation();
        mat4x4 translateTransform(TRANSLATION_MATRIX(axis == X ? value : 0, axis == Y ? value : 0, axis == Z ? value : 0));
        p_activeCamera->SetTransformation(inverse(translateTransform * inverse(currTransf)));

        //model update
        PModel p_cameraModel = p_activeCamera->getCameraModel();
        p_cameraModel->SetModelTransformation(translateTransform * p_cameraModel->GetModelTransformation());
    }
}

void Scene::RotateActiveCameraWorldAxis(float angle, AXES axis)
{
    if (m_activeCamera != DISABLED)
    {
        Camera* p_activeCamera = m_cameras[m_activeCamera];
        mat4x4 currTransf = p_activeCamera->GetTransformation();

        mat4x4 rotateTransform;
        switch (axis)
        {
        case X: rotateTransform = ROTATING_MATRIX_X_AXIS(angle); break;
        case Y: rotateTransform = ROTATING_MATRIX_Y_AXIS(angle); break;
        case Z: rotateTransform = ROTATING_MATRIX_Z_AXIS(angle); break;
        default:                                               ; break;
        }

        p_activeCamera->SetTransformation(inverse(rotateTransform * inverse(currTransf)));

        PModel p_cameraModel = p_activeCamera->getCameraModel();
        p_cameraModel->SetModelTransformation(rotateTransform * p_cameraModel->GetModelTransformation());
    }
}

void Scene::RotateActiveCameraAxis(float angle, AXES axis)
{
    if (m_activeCamera != DISABLED)
    {
        Camera* p_activeCamera = m_cameras[m_activeCamera];
        mat4x4 currTransf = p_activeCamera->GetTransformation();

        mat4x4 toOrigin(TRANSLATION_MATRIX(-currTransf[3][0], -currTransf[3][1], -currTransf[3][2]));
        mat4x4 toPlace(TRANSLATION_MATRIX(currTransf[3][0], currTransf[3][1], currTransf[3][2]));

        mat4x4 rotateTransform;

        switch (axis)
        {
        case X: rotateTransform = ROTATING_MATRIX_X_AXIS(angle); break;
        case Y: rotateTransform = ROTATING_MATRIX_Y_AXIS(angle); break;
        case Z: rotateTransform = ROTATING_MATRIX_Z_AXIS(angle); break;
        default:                                               ; break;
        }

        p_activeCamera->SetTransformation(inverse(toPlace * rotateTransform * toOrigin * inverse(currTransf)));

        PModel p_cameraModel = p_activeCamera->getCameraModel();
        p_cameraModel->SetModelTransformation(rotateTransform * p_cameraModel->GetModelTransformation());
    }
}
//////////////////// Model ////////////////////////
void Scene::TranslateActiveModelAxis(float value, AXES axis)
{
    if (m_activeModel != DISABLED)
    {
        Model* activeModel = m_models[m_activeModel];
        mat4x4 currTransf = activeModel->GetModelTransformation();
        mat4x4 translateTransform(TRANSLATION_MATRIX(axis == X ? value : 0, axis == Y ? value : 0, axis == Z ? value : 0));
        activeModel->SetModelTransformation(translateTransform * currTransf);
    }
}

void Scene::ScaleActiveModel(float value)
{
    if (m_activeModel != DISABLED)
    {
        Model* activeModel = m_models[m_activeModel];
        mat4x4 currTransf = activeModel->GetModelTransformation();
        mat4x4 scaleTransform(SCALING_MATRIX4(value));
        activeModel->SetModelTransformation(scaleTransform * currTransf);
    }
}

void Scene::RotateActiveModelAxis(float angle, AXES axis)
{
    if (m_activeModel != DISABLED)
    {
        Model* activeModel = m_models[m_activeModel];
        mat4x4 currTransf = activeModel->GetModelTransformation(); /* activeModel->GetWorldTransformation();*/
        mat4x4 toOrigin (TRANSLATION_MATRIX(-currTransf[3][0], -currTransf[3][1], -currTransf[3][2]));
        mat4x4 toPlace(TRANSLATION_MATRIX(currTransf[3][0], currTransf[3][1], currTransf[3][2]));
        
        mat4x4 rotateTransform;

        switch (axis)
        {
        case X: rotateTransform = ROTATING_MATRIX_X_AXIS(angle); break;
        case Y: rotateTransform = ROTATING_MATRIX_Y_AXIS(angle); break;
        case Z: rotateTransform = ROTATING_MATRIX_Z_AXIS(angle); break;
        default:; break;
        }

        activeModel->SetModelTransformation(toPlace * rotateTransform * toOrigin * currTransf);
    }
}

bool Scene::shouldRenderCamera(int cameraIndex)
{
    if (m_activeCamera != DISABLED)
    {
        return m_cameras[cameraIndex]->getCameraModel()->isModelRenderingActive();
    }
    else return false;
}


vec4 Scene::GetBgColor()
{
    return m_bgColor;
}

void Scene::SetBgColor(const glm::vec4& newBgColor)
{
    m_bgColor = newBgColor;
}

vec4 Scene::GetPolygonColor()
{
    return m_polygonColor;
}

void Scene::SetPolygonColor(const vec4& newMeshColor)
{
    m_polygonColor = newMeshColor;
}

vec4 Scene::GetWireframeColor()
{
    return m_wireframeColor;
}

void Scene::SetWireframeColor(const vec4& newWireframeColor)
{
    m_wireframeColor = newWireframeColor;
}

float Scene::GetvnScale()
{
    return m_vnScaleFactor;
}

void Scene::SetvnScale(float scale)
{
    m_vnScaleFactor = scale;
}

float Scene::GetfnScale()
{
    return m_fnScaleFactor;
}

void Scene::SetfnScale(float scale)
{
    m_fnScaleFactor = scale;
}

unsigned int Scene::AddPrimitiveModel(PRIM_MODEL primitiveModel)
{
    Model* newModel = new PrimMeshModel(primitiveModel);
    m_models.push_back(newModel);
    return (unsigned)m_models.size() - 1;
}

void Scene::NextModel()
{
    if (m_activeModel != DISABLED)
    {
        m_activeModel = (m_activeModel + 1) % m_models.size();
    }
}

void Scene::DeleteActiveModel()
{
    if (m_activeModel != DISABLED)
    {
        m_models.erase(m_models.begin() + m_activeModel);
        m_activeModel = (unsigned)m_models.size() - 1;
    }
}

mat4x4 Scene::GetActiveModelTransformation()
{
    if (m_activeModel != DISABLED)
    {
        Model* activeModel = m_models[m_activeModel];
        return  activeModel->GetModelTransformation();
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