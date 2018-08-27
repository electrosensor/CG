#include "Scene.h"
#include "MeshModel.h"

using namespace std;
using namespace glm;

#define IS_CAMERA true

void Scene::LoadOBJModel(std::string fileName, const Surface& material)
{
    auto* model = new MeshModel(fileName, material);
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

    vector<Face> vPolygons;//TODO: RESERVE
    vector<vec3> vVertices;
    vector<vec3> vVerticesNormals;

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
    renderer->SetWorldTransformation(m_worldTransformation);

    renderer->SetCameraTransform(activeCamera->GetTransformation());
    renderer->setProjectionParams(activeCamera->getProjectionParams());
    renderer->SetProjection(activeCamera->GetProjection());

    renderer->SetBgColor(m_bgColor);
    renderer->SetPolygonColor(m_polygonColor);
    renderer->SetWireframeColor(m_wireframeColor);
    renderer->SetShadingType(m_shading);
    renderer->DrawWireframe(m_bDrawWireframe);
    renderer->DrawFaceNormal(m_bDrawFaceNormal);
    renderer->SetFaceNormScaleFactor(m_fnScaleFactor);
    renderer->DrawAxis();


    for each(Light* light in m_lights)
    {
        LightMeshModel& lightModel = light->GetLightModel();
        if (lightModel.isModelRenderingActive())
        {
            tuple lightModelData(vPolygons, vVertices, vVerticesNormals);
            mat4x4 lightModelTransformation = lightModel.GetModelTransformation();
            renderer->SetObjectMatrices(lightModelTransformation, mat4x4(I_MATRIX));
            lightModel.Draw(lightModelData);
            renderer->DrawTriangles(get<TUPLE_POLYGONS>(lightModelData));
        }
    }

    for each (Model* model in m_models)
    {
        tuple modelData(vPolygons, vVertices, vVerticesNormals);
        model->Draw(modelData);

        vector<Face>& polygonsToLight = get<TUPLE_POLYGONS>(modelData);
        for each(Light* light in m_lights)
        {
            if (/*light->isOn()*/ true)
            {
                for(Face& faceIt : polygonsToLight)
                {
                    light->Illuminate(faceIt, light->GetLightModel().GetModelTransformation());
                }
            }
        }
        tie(vPolygons, vVertices, vVerticesNormals) = modelData;

        renderer->SetObjectMatrices(model->GetModelTransformation(), model->GetNormalTransformation());
        renderer->DrawTriangles(vPolygons, &model->getCentroid(), activeCamera->getCameraModel()->getCentroid());

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
            tuple camModelData(vPolygons, vVertices, vVerticesNormals);
            mat4x4 cameraModelTransformation = camModel->GetModelTransformation();
            renderer->SetObjectMatrices(cameraModelTransformation, mat4x4(I_MATRIX));
            camModel->Draw(camModelData);
            renderer->DrawTriangles(get<TUPLE_POLYGONS>(camModelData));
        }
    }

    renderer->applyPostEffect(m_bBlurX, m_bBlurY, m_sigma, m_ePostEffect);


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







void Scene::TranslateModel(Model* activeModel, AXES axis, float value)
{
    mat4x4 currTransf = activeModel->GetModelTransformation();
    mat4x4 translateTransform(TRANSLATION_MATRIX(axis == AXIS_X ? value : 0, axis == AXIS_Y ? value : 0, axis == AXIS_Z ? value : 0));
    activeModel->SetModelTransformation(translateTransform * currTransf);
}

void Scene::ScaleModel(Model* activeModel, float value)
{
    mat4x4 currTransf = activeModel->GetModelTransformation();
    mat4x4 scaleTransform(SCALING_MATRIX4(value));
    activeModel->SetModelTransformation(scaleTransform * currTransf);
}


void Scene::RotateModel(Model* activeModel, AXES axis, float angle)
{
    mat4x4 currTransf = activeModel->GetModelTransformation(); /* activeModel->GetWorldTransformation();*/
    mat4x4 toOrigin(TRANSLATION_MATRIX(-currTransf[3][0], -currTransf[3][1], -currTransf[3][2]));
    mat4x4 toPlace(TRANSLATION_MATRIX(currTransf[3][0], currTransf[3][1], currTransf[3][2]));

    mat4x4 rotateTransform;

    switch (axis)
    {
    case AXIS_X: rotateTransform = ROTATING_MATRIX_X_AXIS(angle); break;
    case AXIS_Y: rotateTransform = ROTATING_MATRIX_Y_AXIS(angle); break;
    case AXIS_Z: rotateTransform = ROTATING_MATRIX_Z_AXIS(angle); break;
    default:; break;
    }

    activeModel->SetModelTransformation(toPlace * rotateTransform * toOrigin * currTransf);
}

void Scene::RotateModelRelativeToWorld(Model* activeModel, AXES axis, float angle)
{
    mat4x4 currTransf = activeModel->GetModelTransformation();

    mat4x4 rotateTransform;

    switch (axis)
    {
    case AXIS_X: rotateTransform = ROTATING_MATRIX_X_AXIS(angle); break;
    case AXIS_Y: rotateTransform = ROTATING_MATRIX_Y_AXIS(angle); break;
    case AXIS_Z: rotateTransform = ROTATING_MATRIX_Z_AXIS(angle); break;
    default:; break;
    }

    activeModel->SetModelTransformation(rotateTransform * currTransf);
}









//////////////////// Camera ////////////////////////
/*
* Adds camera to vector of scene cameras.
* eye – The position of the camera
* at – The position the camera looks at
* up – The upside(y) direction of the camera
* returns: current index of added camera.
*/
unsigned int Scene::AddCamera(const vec3& eye, const vec3& at, const vec3& up)
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
/*        projParams.right *= ((float)renderer->getWidth()/(float)renderer->getHeight());*/
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
        mat4x4 currTransf      = p_activeCamera->GetTransformation();
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
        mat4x4 translateTransform(TRANSLATION_MATRIX(axis == AXIS_X ? value : 0, axis == AXIS_Y ? value : 0, axis == AXIS_Z ? value : 0));
        p_activeCamera->SetTransformation(inverse(translateTransform * inverse(currTransf)));

        //model update
        PModel p_cameraModel = p_activeCamera->getCameraModel();
        TranslateModel(p_cameraModel, axis, value);
    }
}

void Scene::RotateActiveCameraRelativeToWorld(float angle, AXES axis)
{
    if (m_activeCamera != DISABLED)
    {
        Camera* p_activeCamera = m_cameras[m_activeCamera];
        mat4x4 currTransf = p_activeCamera->GetTransformation();

        mat4x4 rotateTransform;
        switch (axis)
        {
        case AXIS_X: rotateTransform = ROTATING_MATRIX_X_AXIS(angle); break;
        case AXIS_Y: rotateTransform = ROTATING_MATRIX_Y_AXIS(angle); break;
        case AXIS_Z: rotateTransform = ROTATING_MATRIX_Z_AXIS(angle); break;
        default:                                               ; break;
        }

        p_activeCamera->SetTransformation(inverse(rotateTransform * inverse(currTransf)));

        PModel p_cameraModel = p_activeCamera->getCameraModel();
        RotateModelRelativeToWorld(p_cameraModel, axis, angle);
    }
}

void Scene::RotateActiveCamera(float angle, AXES axis)
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
        case AXIS_X: rotateTransform = ROTATING_MATRIX_X_AXIS(angle); break;
        case AXIS_Y: rotateTransform = ROTATING_MATRIX_Y_AXIS(angle); break;
        case AXIS_Z: rotateTransform = ROTATING_MATRIX_Z_AXIS(angle); break;
        default:                                               ; break;
        }

        p_activeCamera->SetTransformation(inverse(toPlace * rotateTransform * toOrigin * inverse(currTransf)));

        PModel p_cameraModel = p_activeCamera->getCameraModel();
        RotateModel(p_cameraModel, axis, angle);
    }
}


//////////////////// Model ////////////////////////
void Scene::TranslateActiveModel(float value, AXES axis)
{
    if (m_activeModel != DISABLED)
    {
        Model* activeModel = m_models[m_activeModel];
        TranslateModel(activeModel, axis, value);
    }
}

void Scene::ScaleActiveModel(float value)
{
    if (m_activeModel != DISABLED)
    {
        Model* activeModel = m_models[m_activeModel];
        ScaleModel(activeModel, value);
    }
}

void Scene::RotateActiveModel(float angle, AXES axis)
{
    if (m_activeModel != DISABLED)
    {
        Model* activeModel = m_models[m_activeModel];
        RotateModel(activeModel, axis, angle);
    }
}

void Scene::RotateActiveModelRelativeToWorld(float angle, AXES axis)
{
    if (m_activeModel != DISABLED)
    {
        Model* activeModel = m_models[m_activeModel];
        RotateModelRelativeToWorld(activeModel, axis, angle);
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





GENERATED_TEXTURE Scene::GetGeneratedTexture()
{
    return renderer->GetGeneratedTexture();
}

void Scene::SetGeneratedTexture(GENERATED_TEXTURE texture)
{
    renderer->SetGeneratedTexture(texture);
}

int Scene::GetActiveLightIdx()
{
    return m_activeLight;
}

void Scene::SetActiveLightIdx(unsigned int lightIdx)
{
    if (m_lights.size() > lightIdx)
    {
        m_activeLight = lightIdx;
    }
}

void Scene::NextLight()
{
    if (m_activeLight != DISABLED)
    {
        m_activeLight = (m_activeLight + 1) % m_lights.size();
    }
}

void Scene::DeleteActiveLight()
{
    if (m_activeLight != DISABLED)
    {
        m_lights.erase(m_lights.begin() + m_activeLight);
        m_activeLight = (unsigned)m_lights.size() - 1;
    }
}

glm::mat4x4 Scene::GetActiveLightModelTransformation()
{
    if (m_activeLight != DISABLED)
    {
        Model* activeLightModel = &m_lights[m_activeLight]->GetLightModel();
        return activeLightModel->GetModelTransformation();
    }
    else
    {
        return ZERO_MATRIX;
    }
}

void Scene::TranslateActiveLight(float value, AXES axis)
{
    if (m_activeLight != DISABLED)
    {
        Model* activeLightModel = &m_lights[m_activeLight]->GetLightModel();
        TranslateModel(activeLightModel, axis, value);
    }
}

void Scene::ScaleActiveLightModel(float value)
{
    if (m_activeLight != DISABLED)
    {
        Model* activeLightModel = &m_lights[m_activeLight]->GetLightModel();
        ScaleModel(activeLightModel, value);
    }
}

void Scene::RotateActiveLightModel(float angle, AXES axis)
{
    if (m_activeLight != DISABLED)
    {
        Model* activeLightModel = &m_lights[m_activeLight]->GetLightModel();
        RotateModel(activeLightModel, axis, angle);
    }
}

void Scene::RotateActiveLightModelRelativeToWorld(float angle, AXES axis)
{
    if (m_activeLight != DISABLED)
    {
        Model* activeLightModel = &m_lights[m_activeLight]->GetLightModel();
        RotateModelRelativeToWorld(activeLightModel, axis, angle);
    }
}

bool Scene::shouldRenderLight()
{
    if (m_activeLight != DISABLED)
    {
        return m_lights[m_activeLight]->GetLightModel().isModelRenderingActive();
    }
    else return false;
}

int Scene::AddLight(LIGHT_SOURCE_TYPE type, const glm::vec3& lightCoord, const glm::vec4& ambiantC, float ambiantI, const glm::vec4& diffusiveC, float diffusiveI, const glm::vec4& specularC, float specularI)
{
    Light* p_newLight = nullptr;

    switch (type)
    {
    case LST_POINT:
        p_newLight = new PointSourceLight(lightCoord, ambiantC, ambiantI, diffusiveC, diffusiveI, specularC, specularI);
        break;
    case LST_PARALLEL:
        p_newLight = new ParallelSourceLight(lightCoord, ambiantC, ambiantI, diffusiveC, diffusiveI, specularC, specularI);
        break;
    case LST_AREA:
        p_newLight = new DistributedSourceLight(lightCoord, ambiantC, ambiantI, diffusiveC, diffusiveI, specularC, specularI);
        break;

    default:
        break;
    }

    m_lights.push_back(p_newLight);
    return (unsigned)m_lights.size() - 1;
}


Light* Scene::GetActiveLight()
{
    if (m_activeLight != DISABLED) {
        return m_lights[m_activeLight];
    }
    else return nullptr;
}


void Scene::SetShadingType(SHADING_TYPE shading)
{
    m_shading = shading;
}

void Scene::DrawWireframe(bool bDrawn)
{
    m_bDrawWireframe = bDrawn;
}

void Scene::configPostEffect(POST_EFFECT postEffect, int blurX, int blurY, float sigma, float bloomIntensity, float bloomThreshold)
{
    if (postEffect != m_ePostEffect || blurX != m_bBlurX || blurY != m_bBlurY || sigma != m_sigma || bloomIntensity != m_bloomIntensity)
    {
        m_bBlurX = blurX;
        m_bBlurY = blurY;
        m_sigma = sigma;
        m_ePostEffect = postEffect;
        m_bloomIntensity = bloomIntensity;
        renderer->configPostEffect(m_ePostEffect, m_bBlurX, m_bBlurY, m_sigma, m_bloomIntensity, bloomThreshold);
    }
    
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

unsigned int Scene::AddPrimitiveModel(PRIM_MODEL primitiveModel, const Surface& material)
{
    Model* newModel = new PrimMeshModel(primitiveModel, material);
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