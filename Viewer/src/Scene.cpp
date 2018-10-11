#include "Scene.h"
#include "MeshModel.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "InitShader.h"
using namespace std;
using namespace glm;

#define IS_CAMERA true


Scene::Scene() : m_activeModel(DISABLED), m_activeLight(DISABLED), m_activeCamera(DISABLED), m_bDrawVecNormal(false), m_vnScaleFactor(2.f), m_fnScaleFactor(2.f), m_bgColor(COLOR(YURI_BG)), m_polygonColor(COLOR(YURI_POLYGON)), m_wireframeColor(COLOR(YURI_WIRE)), m_bDrawWireframe(true), m_bBlurX(1), m_bBlurY(1), m_sigma(1.f), m_ePostEffect(NONE)
{
    m_program = InitShader("vshader.glsl", "fshader.glsl");
    // Make this program the current one.
    glUseProgram(m_program);

    m_worldTransformation = I_MATRIX;
    m_worldTransformation[3].w = 1;
}

void Scene::LoadOBJModel(std::string fileName, const Surface& material)
{
    auto* model = new MeshModel(fileName, material, m_program);
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
    vector<vec3> vVerticesPositions;


    glClearColor(m_bgColor.x, m_bgColor.y, m_bgColor.z, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_activeCamera != DISABLED)
    {
        activeCamera = m_cameras[m_activeCamera];
    }
    else
    {
        m_cameras.push_back(new Camera(m_program));
        activeCamera = m_cameras.front();
        m_activeCamera++;
    }

    mat4x4 View = activeCamera->GetTransformation();
    mat4x4 Projection = activeCamera->GetProjection();

//     for each(Light* light in m_lights)
//     {
//         LightMeshModel& lightModel = light->GetLightModel();
//         if (lightModel.isModelRenderingActive())
//         {
//             tuple lightModelData(vPolygons, vVertices, vVerticesNormals, vVerticesPositions);
//             mat4x4 lightModelTransformation = lightModel.GetModelTransformation();
//             renderer->SetObjectMatrices(lightModelTransformation, mat4x4(I_MATRIX));
//             lightModel.Draw(lightModelData);
//             renderer->DrawTriangles(get<TUPLE_POLYGONS>(lightModelData));
//         }
//     }

    for each (Model* model in m_models)
    {

        tuple modelData(vPolygons, vVertices, vVerticesNormals, vVerticesPositions);
// 
//         vector<Face>& polygonsToLight = get<TUPLE_POLYGONS>(modelData);
//         for each(Light* light in m_lights)
//         {
//             if (/*light->isOn()*/ true)
//             {
//                 for(Face& faceIt : polygonsToLight)
//                 {
//                     light->Illuminate(faceIt, light->GetLightModel().GetModelTransformation());
//                 }
//             }
//         }
        tie(vPolygons, vVertices, vVerticesNormals, vVerticesPositions) = modelData;

        mat4x4 objTransformation = model->GetTranslateTransformation() * model->GetRotateTransformation() * model->GetScaleTransformation();


        GLuint ModelMatrixID = glGetUniformLocation(m_program, "Model");
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &objTransformation[0][0]);

        GLuint ViewMatrixID = glGetUniformLocation(m_program, "View");
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);

        GLuint ProjectionMatrixID = glGetUniformLocation(m_program, "Projection");
        glUniformMatrix4fv(ProjectionMatrixID, 1, GL_FALSE, &Projection[0][0]);

        model->Draw(modelData);

     }

    for each(Camera* camera in m_cameras)
    {
        auto camModel = (CamMeshModel*) camera->getCameraModel();
        if (camModel->isModelRenderingActive() && camera != activeCamera)
        {
            vVerticesNormals.reserve(0);
            tuple camModelData(vPolygons, vVertices, vVerticesNormals, vVerticesPositions);
            mat4x4 cameraModelTransformation = camModel->GetModelTransformation();
            GLuint ModelMatrixID = glGetUniformLocation(m_program, "Model");

            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &cameraModelTransformation[0][0]);

            GLuint ViewMatrixID = glGetUniformLocation(m_program, "View");
            glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);

            GLuint ProjectionMatrixID = glGetUniformLocation(m_program, "Projection");
            glUniformMatrix4fv(ProjectionMatrixID, 1, GL_FALSE, &Projection[0][0]);


            camModel->Draw(camModelData);

        }
    }

//     renderer->applyPostEffect(m_bBlurX, m_bBlurY, m_sigma, m_ePostEffect);

//     void Renderer::applyPostEffect(int kernelSizeX, int kernelSizeY, float sigma, POST_EFFECT postEffect /*= NONE*/)
//     {
//         static int kerSizeX = -1;
//         static int kerSizeY = -1;
// 
//         float* source;
//         float* destination;
// 
//         static float kernel[29][29] = { 0 };
// 
//         if (kernelSizeX <= 2 && kernelSizeY <= 2 || postEffect == NONE)
//         {
//             pDispBuffer = colorBuffer;
//             return;
//         }
// 
// 
//         if (kerSizeX != kernelSizeX || kerSizeY != kernelSizeY || sigma != m_sigma)
//         {
//             kerSizeX = kernelSizeX;
//             kerSizeY = kernelSizeY;
//             m_sigma = sigma;
// 
//             Renderer::makeKernel(kernel, kernelSizeX, kernelSizeY, sigma);
// 
//             static int counter = 0;
//             printf("%d: kernel size is x,y:%d,%d with sigma %f\n", ++counter, kernelSizeX, kernelSizeY, sigma);
//             for (int kX = 0; kX < kernelSizeX; ++kX)
//             {
//                 for (int kY = 0; kY < kernelSizeY; ++kY)
//                 {
//                     printf("%f\t\t", kernel[kX][kY]);
//                 }
//                 printf("\n\n");
//             }
//         }
// 
//         switch (postEffect)
//         {
//         case NONE:
//             return;
//             break;
//         case BLUR_SCENE:
//             source = colorBuffer;
//             destination = blurredBuffer;
//             break;
//         case BLOOM:
//             source = bloomBuffer;
//             destination = bloomDestBuff;
//             break;
//         default:
//             return;
//             break;
//         }
// 
//         int halfX = kernelSizeX / 2;
//         int halfY = kernelSizeY / 2;
// 
//         for (int x = halfX; x < m_width - halfX; x++)
//         {
//             for (int y = halfY; y < m_height - halfY; y++)
//             {
//                 for (int color = 0; color < 3; color++)
//                 {
//                     float sum = 0.0f;
//                     float sumBloom = 0.0f;
//                     for (int kX = -halfX; kX <= halfX; kX++)
//                     {
//                         for (int kY = -halfY; kY <= halfY; kY++)
//                         {
//                             sum += kernel[kX + halfX][kY + halfY] * colorTruncate(source[COLOR_BUF_INDEX(m_width, x + kX, y + kY, color)]);
//                             if (postEffect == BLOOM)
//                             {
//                                 sumBloom += kernel[kX + halfX][kY + halfY] * colorTruncate(colorBuffer[COLOR_BUF_INDEX(m_width, x + kX, y + kY, color)]);
//                             }
//                         }
//                     }
//                     destination[COLOR_BUF_INDEX(m_width, x, y, color)] = sum;
//                     if (postEffect == BLOOM)
//                     {
//                         blurredBuffer[COLOR_BUF_INDEX(m_width, x, y, color)] = sumBloom;
//                     }
//                 }
//             }
//         }
// 
// 
//         if (postEffect == BLOOM)
//         {
//             for (int x = 0; x < m_width; x++)
//             {
//                 for (int y = 0; y < m_height; y++)
//                 {
//                     for (int color = 0; color < 3; color++)
//                         blurredBuffer[COLOR_BUF_INDEX(m_width, x, y, color)] += m_bloomIntensity * bloomDestBuff[COLOR_BUF_INDEX(m_width, x, y, color)];
//                 }
//             }
//         }
//     }

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

    mat4x4 currTransf = activeModel->GetTranslateTransformation();
    mat4x4 translateTransform(translate(currTransf, vec3(axis == AXIS_X ? value : 0, axis == AXIS_Y ? value : 0, axis == AXIS_Z ? value : 0)));
    activeModel->SetTranslateTransformation(translateTransform);
}

void Scene::ScaleModel(Model* activeModel, float value)
{
    mat4x4 currTransf = activeModel->GetScaleTransformation();
    mat4x4 scaleTransform(scale(currTransf, vec3(value, value, value)));
    activeModel->SetScaleTransformation(scaleTransform);
}


void Scene::RotateModel(Model* activeModel, AXES axis, float angle)
{
    mat4x4 currTransl = activeModel->GetTranslateTransformation(); 
    mat4x4 currRot = activeModel->GetRotateTransformation();
    vec3 toOrigin(-currTransl[3][0], -currTransl[3][1], -currTransl[3][2]);
    vec3 toPlace = -toOrigin;

    mat4x4 rotateTransform = translate(currRot, toOrigin);

    switch (axis)
    {
    case AXIS_X: rotateTransform = rotate(rotateTransform, angle, vec3(1, 0, 0)); break;
    case AXIS_Y: rotateTransform = rotate(rotateTransform, angle, vec3(0, 1, 0)); break;
    case AXIS_Z: rotateTransform = rotate(rotateTransform, angle, vec3(0, 0, 1)); break;
    default:; break;
    }

    rotateTransform = translate(rotateTransform, toPlace);

    activeModel->SetRotateTransformation(rotateTransform);
}

void Scene::RotateModelRelativeToWorld(Model* activeModel, AXES axis, float angle)
{
    mat4x4 currTransf = activeModel->GetRotateTransformation();

    switch (axis)
    {
    case AXIS_X: currTransf = rotate(currTransf, angle, vec3(1, 0, 0)); break;
    case AXIS_Y: currTransf = rotate(currTransf, angle, vec3(0, 1, 0)); break;
    case AXIS_Z: currTransf = rotate(currTransf, angle, vec3(0, 0, 1)); break;
    default:; break;
    }

    activeModel->SetRotateTransformation(currTransf);
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
    auto* newCamera = new Camera(eye, at, up, m_program);
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
        //projParams.right *= ((float)renderer->getWidth()/(float)renderer->getHeight());

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
        p_newLight = new PointSourceLight(lightCoord, ambiantC, ambiantI, diffusiveC, diffusiveI, specularC, specularI, m_program);
        break;
    case LST_PARALLEL:
        p_newLight = new ParallelSourceLight(lightCoord, ambiantC, ambiantI, diffusiveC, diffusiveI, specularC, specularI, m_program);
        break;
    case LST_AREA:
        p_newLight = new DistributedSourceLight(lightCoord, ambiantC, ambiantI, diffusiveC, diffusiveI, specularC, specularI, m_program);
        break;

    default:
        break;
    }

    m_lights.push_back(p_newLight);
    return (unsigned)m_lights.size() - 1;
}

int Scene::AddLight(LIGHTS_INFO lightsInfo)
{
    Light* p_newLight = nullptr;

    switch (lightsInfo.lightSourceType)
    {
    case LST_POINT:
        p_newLight = new PointSourceLight(   lightsInfo.location, 
                                             lightsInfo.ambient.color,
                                             lightsInfo.ambient.intensity,
                                             lightsInfo.diffusive.color,
                                             lightsInfo.diffusive.intensity,
                                             lightsInfo.specular.color,
                                             lightsInfo.specular.intensity, m_program);
        break;
    case LST_PARALLEL:
        p_newLight = new ParallelSourceLight(lightsInfo.location,
                                             lightsInfo.ambient.color,
                                             lightsInfo.ambient.intensity,
                                             lightsInfo.diffusive.color,
                                             lightsInfo.diffusive.intensity,
                                             lightsInfo.specular.color,
                                             lightsInfo.specular.intensity, m_program);
        break;
    case LST_AREA:
        p_newLight = new DistributedSourceLight(lightsInfo.location,
                                                lightsInfo.ambient.color,
                                                lightsInfo.ambient.intensity,
                                                lightsInfo.diffusive.color,
                                                lightsInfo.diffusive.intensity,
                                                lightsInfo.specular.color,
                                                lightsInfo.specular.intensity, m_program);
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

void Scene::configPostEffect(POST_EFFECT postEffect, int blurX, int blurY, float sigma, float bloomIntensity, glm::vec4 bloomThreshold, float bloomThresh)
{
    if (postEffect != m_ePostEffect || blurX != m_bBlurX || blurY != m_bBlurY || sigma != m_sigma || bloomIntensity != m_bloomIntensity || bloomThresh != m_bloomThresh || !memcmp(&bloomThreshold,&m_bloomThreshold,sizeof(glm::vec4)))
    {
        m_bBlurX         = blurX;
        m_bBlurY         = blurY;
        m_sigma          = sigma;
        m_ePostEffect    = postEffect;
        m_bloomIntensity = bloomIntensity;
        m_bloomThreshold = bloomThreshold;
        m_bloomThresh    = bloomThresh;
//         renderer->configPostEffect(m_ePostEffect, m_bBlurX, m_bBlurY, m_sigma, m_bloomIntensity, m_bloomThreshold, m_bloomThresh);
    }
    
}

void Scene::ApplyTextureToActiveModel(std::string texPath)
{
    if (m_activeModel != DISABLED)
    {
        Model* activeModel = m_models[m_activeModel];
        return  activeModel->ApplyTexture(texPath);
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
    Model* newModel = new PrimMeshModel(primitiveModel, material, m_program);
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