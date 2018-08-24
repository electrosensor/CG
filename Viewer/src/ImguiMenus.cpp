#pragma once
#include "ImguiMenus.h"
#include "Defs.h"
#include <stdio.h>
#include <stdlib.h>
// open file dialog cross platform https://github.com/mlabbe/nativefiledialog
#include <nfd.h>

using namespace std;
using namespace glm;

bool showDemoWindow = false;
bool modelControlWindow = true;
static bool bModelControlFrame = false;

bool lightingMenu = true;
static bool bLightControlFrame = false;

bool colorMenu = false;
bool showFile = false;
vec4 clearColor = vec4(0.4f, 0.55f, 0.60f, 1.00f);

const vec4& GetClearColor()
{
    return clearColor;
}

void DrawImguiMenus(ImGuiIO& io, Scene* scene)
{
    ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Once);
    // 1. Show a simple window.
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
    {
        ImGui::Begin("Main menu");
        static int world[4] = { 1,1,1 };
        ImGui::InputInt3("World transformation: (x,y,z)", world);
        ImGui::Text("World transformation: (%d, %d, %d)", world[0], world[1], world[2]);
        if (ImGui::Button("Set world transformation"))
        {
            scene->SetWorldTransformation(HOMOGENEOUS_MATRIX4(world[0], world[1], world[2], 1.0f));
        }
        static mat4x4 worldTransformation = mat4x4(0);
        worldTransformation = scene->GetWorldTransformation();
        string sWorldTransform = "";
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                sWorldTransform.append(std::to_string(worldTransformation[i][j]) + " ");
            }
            sWorldTransform.append("\n");
        }
        ImGui::Text("World transformation:\n");
        ImGui::Text(sWorldTransform.c_str());

        //cene->updateCurrentDims((int) ImGui::GetWindowHeight(), (int) ImGui::GetWindowWidth());



        ImGui::Text("------------------- Cameras: -------------------");

        static float eye[3] = { 2,2,2 };
        static float at[3] = { 0,0,0 };
        ImGui::SliderFloat3("Look from: (x,y,z)", eye, -10, 10);
        ImGui::SliderFloat3("Look at: (x,y,z)", at, -10, 10);
        ImGui::Text("Look from: (%d, %d, %d)", eye[0], eye[1], eye[2]);
        ImGui::Text("Look at: (%d, %d, %d)", at[0], at[1], at[2]);
        if (ImGui::Button("Add new camera"))
        {
            int idx = scene->AddCamera({ eye[0], eye[1], eye[2]}, { at[0], at[1], at[2] }, { 0, 1, 0 });
            scene->SetActiveCameraIdx(idx);
        }
        ImGui::SameLine();

        if (ImGui::Button("Next camera"))
        {
            scene->NextCamera();
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete camera"))
        {
            scene->DeleteActiveCamera();
        }
        //         ImGui::SameLine();
        //         if (ImGui::Button("Apply to camera (todo)"))
        //         {
        //             
        //         }

        static bool bShowCamera = false;
        int cameraIndex = scene->GetActiveCameraIdx();
        ImGui::Text("Active camera: %d", cameraIndex);
        bShowCamera = scene->shouldRenderCamera(cameraIndex);
        ImGui::SameLine();
        if (ImGui::Checkbox("Show Camera", &bShowCamera))
        {
            if (scene->getActiveCamera() != nullptr) {
                scene->getActiveCamera()->getCameraModel()->setModelRenderingState(bShowCamera);
            }
        }


        static mat4x4 activeCameraTransformation = mat4x4(0);
        activeCameraTransformation = scene->GetActiveCameraTransformation();
        string sCameraTransform = "";
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                sCameraTransform.append(std::to_string(activeCameraTransformation[i][j]) + " ");
            }
            sCameraTransform.append("\n");
        }

        ImGui::Text("Camera transformation:\n");
        ImGui::Text(sCameraTransform.c_str());


        ImGui::Text("---------------- Camera Control: ----------------");


        static int currentFrame = FRAME_TYPE::FT_CAMERA;
        static AXES currentAxis = AXES::AXIS_Y;
        static int currentRel = ROTATION_REL::RR_WORLD;

        static string sCurrentFrame = "Camera";
        static string sCurrentAxis = "Y-axis";
        static string sCurrentRel = "World";
        
        ImGui::TextColored( { 1.f, 0.4f, 0.4f, 1.f }, (sCurrentFrame + " relative to " + sCurrentAxis + " of " + sCurrentRel).c_str());

        ImGui::Text("Frame: ");
        ImGui::SameLine();
        if (ImGui::SmallButton("World"))
        {
            sCurrentFrame = "World";
            currentFrame = FT_WORLD;
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Camera"))
        {
            sCurrentFrame = "Camera";
            currentFrame = FT_CAMERA;
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Model"))
        {
            sCurrentFrame = "Model";
            currentFrame = FT_MODEL;
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Light"))
        {
            sCurrentFrame = "Light";
            currentFrame = FT_LIGHT;
        }

        ImGui::Text("Relative to: ");
        ImGui::SameLine();
        if (ImGui::SmallButton("itself"))
        {
            sCurrentRel = "itself";
            currentRel = RR_SELF;
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("world"))
        {
            sCurrentRel = "World";
            currentRel = RR_WORLD;
        }

        ImGui::Text("Axis: ");
        ImGui::SameLine();
        if (ImGui::SmallButton("X-axis"))
        {
            sCurrentAxis = "X-axis";
            currentAxis = AXIS_X;
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Y-axis"))
        {
            sCurrentAxis = "Y-axis";
            currentAxis = AXIS_Y;
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Z-axis"))
        {
            sCurrentAxis = "Z-axis";
            currentAxis = AXIS_Z;
        }


        ImGui::Text("Rotate:");
        static auto rotAngle = static_cast<float>( PI / 36.0f );
        ImGui::SliderAngle("rotation angle", &rotAngle, 1.0f, 180.0f);

        if (!ImGui::IsMouseHoveringAnyWindow() && io.MouseDown[0] && io.MouseDelta.x)
        {
            auto direction = static_cast<int>(io.MouseDelta.x / abs(io.MouseDelta.x));

            switch (currentFrame)
            {
                case FRAME_TYPE::FT_CAMERA:
                {
                    switch (currentRel)
                    {
                        case ROTATION_REL::RR_WORLD:
                        {
                            scene->RotateActiveCameraRelativeToWorld(rotAngle * direction, currentAxis);
                        } break;
                        case ROTATION_REL::RR_SELF:
                        {
                            scene->RotateActiveCamera(rotAngle * direction, currentAxis);
                        } break;
                        default: break;
                    }
                } break;
                case FRAME_TYPE::FT_MODEL:
                {
                    switch (currentRel)
                    {
                        case ROTATION_REL::RR_WORLD:
                        {
                            scene->RotateActiveModelRelativeToWorld(rotAngle * direction, currentAxis);
                        } break;
                        case ROTATION_REL::RR_SELF:
                        {
                            scene->RotateActiveModel(rotAngle * direction, currentAxis);
                        } break;
                        default: break;
                    }
                } break;
                case FRAME_TYPE::FT_LIGHT:
                {
                    switch (currentRel)
                    {
                    case ROTATION_REL::RR_WORLD:
                    {
                        scene->RotateActiveLightModelRelativeToWorld(rotAngle * direction, currentAxis);
                    } break;
                    case ROTATION_REL::RR_SELF:
                    {
                        scene->RotateActiveLightModel(rotAngle * direction, currentAxis);
                    } break;
                    default: break;
                    }
                } break;
                default: break;
            }
        }

        ImGui::Text("Zoom:");
        //Camera scaling:            
        static float camScaleFactor = 1.5f;
        ImGui::SliderFloat("scaling factor", &camScaleFactor, 1.0f, 10.0f);

        if (ImGui::IsKeyPressed('=') || io.MouseWheel > 0 || ImGui::Button("   Zoom in   "))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
        {
            switch (currentFrame)
            {
            case FRAME_TYPE::FT_CAMERA:
            {
                scene->ScaleActiveCamera(1.f / camScaleFactor);
            } break;
            case FRAME_TYPE::FT_MODEL:
            {
                scene->ScaleActiveModel(camScaleFactor);
            } break;
            case FRAME_TYPE::FT_LIGHT:
            {
                scene->ScaleActiveLightModel(camScaleFactor);
            } break;
            default: break;
            }
            
        }
        ImGui::SameLine();
        if (ImGui::IsKeyPressed('-') || io.MouseWheel < 0 || ImGui::Button("   Zoom out   "))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
        {
            switch (currentFrame)
            {
            case FRAME_TYPE::FT_CAMERA:
            {
                scene->ScaleActiveCamera(camScaleFactor);
            } break;
            case FRAME_TYPE::FT_MODEL:
            {
                scene->ScaleActiveModel(1.0f / camScaleFactor);
            } break;
            case FRAME_TYPE::FT_LIGHT:
            {
                scene->ScaleActiveLightModel(1.0f / camScaleFactor);
            } break;
            default: break;
            }

        }

        //Camera moves:
        ImGui::Text("Move:");
        static float moveFactor = 0.5f;
        ImGui::SliderFloat("move factor", &moveFactor, 0.1f, 1.f);

        if (ImGui::IsKeyPressed(GLFW_KEY_LEFT) || ImGui::Button("  Left  "))
        {
            switch (currentFrame)
            {
                case FRAME_TYPE::FT_CAMERA:
                {
                    scene->TranslateActiveCameraAxis(-moveFactor, AXES::AXIS_X);
                } break;
                case FRAME_TYPE::FT_MODEL:
                {
                    scene->TranslateActiveModel(-moveFactor, AXES::AXIS_X);
                } break;
                case FRAME_TYPE::FT_LIGHT:
                {
                    scene->TranslateActiveLight(-moveFactor, AXES::AXIS_X);
                } break;
                default: break;
            }
        }
        ImGui::SameLine();
        if (ImGui::IsKeyPressed(GLFW_KEY_RIGHT) || ImGui::Button("  Right "))
        {
            switch (currentFrame)
            {
            case FRAME_TYPE::FT_CAMERA:
            {
                scene->TranslateActiveCameraAxis(moveFactor, AXES::AXIS_X);
            } break;
            case FRAME_TYPE::FT_MODEL:
            {
                scene->TranslateActiveModel(moveFactor, AXES::AXIS_X);
            } break;
            case FRAME_TYPE::FT_LIGHT:
            {
                scene->TranslateActiveLight(moveFactor, AXES::AXIS_X);
            } break;
            default: break;
            }
        }
        if (ImGui::IsKeyPressed(GLFW_KEY_UP) || ImGui::Button("   Up   "))
        {
            switch (currentFrame)
            {
            case FRAME_TYPE::FT_CAMERA:
            {
                scene->TranslateActiveCameraAxis(moveFactor, AXES::AXIS_Y);
            } break;
            case FRAME_TYPE::FT_MODEL:
            {
                scene->TranslateActiveModel(moveFactor, AXES::AXIS_Y);
            } break;
            case FRAME_TYPE::FT_LIGHT:
            {
                scene->TranslateActiveLight(moveFactor, AXES::AXIS_Y);
            } break;
            default: break;
            }
        }
        ImGui::SameLine();
        if (ImGui::IsKeyPressed(GLFW_KEY_DOWN) || ImGui::Button("  Down  "))
        {
            switch (currentFrame)
            {
            case FRAME_TYPE::FT_CAMERA:
            {
                scene->TranslateActiveCameraAxis(-moveFactor, AXES::AXIS_Y);
            } break;
            case FRAME_TYPE::FT_MODEL:
            {
                scene->TranslateActiveModel(-moveFactor, AXES::AXIS_Y);
            } break;
            case FRAME_TYPE::FT_LIGHT:
            {
                scene->TranslateActiveLight(-moveFactor, AXES::AXIS_Y);
            } break;
            default: break;
            }
        }
        if (ImGui::IsKeyPressed(GLFW_KEY_PAGE_UP) || ImGui::Button(" Forward"))
        {
            switch (currentFrame)
            {
            case FRAME_TYPE::FT_CAMERA:
            {
                scene->TranslateActiveCameraAxis(moveFactor, AXES::AXIS_Z);
            } break;
            case FRAME_TYPE::FT_MODEL:
            {
                scene->TranslateActiveModel(moveFactor, AXES::AXIS_Z);
            } break;
            case FRAME_TYPE::FT_LIGHT:
            {
                scene->TranslateActiveLight(moveFactor, AXES::AXIS_Z);
            } break;
            default: break;
            }
        }
        ImGui::SameLine();
        if (ImGui::IsKeyPressed(GLFW_KEY_PAGE_DOWN) || ImGui::Button("  Back  "))
        {
            switch (currentFrame)
            {
            case FRAME_TYPE::FT_CAMERA:
            {
                scene->TranslateActiveCameraAxis(-moveFactor, AXES::AXIS_Z);
            } break;
            case FRAME_TYPE::FT_MODEL:
            {
                scene->TranslateActiveModel(-moveFactor, AXES::AXIS_Z);
            } break;
            case FRAME_TYPE::FT_LIGHT:
            {
                scene->TranslateActiveLight(-moveFactor, AXES::AXIS_Z);
            } break;
            default: break;
            }
        }

        ////////////////////// PROJECTION //////////////////////////////
        ImGui::Text("Projection:");

        static PROJ_PARAMS projParams =
        {
            projParams.left = -1,
            projParams.right = 1,
            projParams.bottom = -1,
            projParams.top = 1,
            projParams.zNear = 0.1f,
            projParams.zFar = 2,
        };
        ImGui::SliderFloat("Left", &projParams.left, -5.0, 5.0);
        ImGui::SliderFloat("Right", &projParams.right, -5.0, 5.0);
        ImGui::SliderFloat("Bottom", &projParams.bottom, -5.0, 5.0);
        ImGui::SliderFloat("Top", &projParams.top, -5.0, 5.0);
        ImGui::SliderFloat("zNear", &projParams.zNear, -5.0, 5.0);
        ImGui::SliderFloat("zFar", &projParams.zFar, -5.0, 5.0);

        static bool bIsProjError = false;
        static bool bIsPersError = false;


        try
        {
            if (ImGui::Button("Orthographic Projection"))
            {
                scene->SetOrthoProjection(projParams);
            }
        }
        catch (bool thrownErrorState)
        {
            bIsProjError = thrownErrorState;
        }

        static bool bisFirst = true;

        if (bIsProjError)
        {
            ImGui::Text("Invalid input parameters, can't divide by zero - Try Again!");
            bIsPersError = false;
        }

        static PERSPECTIVE_PARAMS perspParam =
        {
            perspParam.fovy = static_cast<float>(TO_RADIAN(325)),
            perspParam.aspect = 1/*ImGui::GetWindowWidth() / ImGui::GetWindowHeight()*/,
            perspParam.zNear = 0.5f,
            perspParam.zFar = 2.f
        };
        ImGui::SliderAngle("Fovy", &perspParam.fovy, 1);
        ImGui::SliderFloat("Near", &perspParam.zNear, 0.1f, 10.f);
        ImGui::SliderFloat("Far",  &perspParam.zFar, 0.2f, 20.f);

        try
        {
            if (ImGui::Button("Perspective Projection"))
            {
                scene->SetPerspectiveProjection(perspParam);
            }
        }
        catch (bool thrownErrorState)
        {
            bIsPersError = thrownErrorState;
        }

        if (bIsPersError)
        {
            ImGui::Text("Invalid input parameters, can't divide by zero - Try Again!");
            bIsProjError = false;
        }

        static mat4x4 activeCameraProjection;
        activeCameraProjection = scene->GetActiveCameraProjection();
        string sCameraProjection = "";
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                sCameraProjection.append(std::to_string(activeCameraProjection[i][j]) + " ");
            }
            sCameraProjection.append("\n");
        }

        ImGui::Text("Camera Projection:\n");
        ImGui::Text(sCameraProjection.c_str());

        ImGui::End();
    }

    // 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name your windows.
    if (modelControlWindow)
    {
        ImGui::Begin("Models");
        ImGui::Text("-------------- Primitives Models: --------------");

        if (ImGui::Button("Add Cube model"))
        {
            int idx = scene->AddPrimitiveModel(PM_CUBE);
            scene->SetActiveModelIdx(idx);
            bModelControlFrame = true;
        }

        if (ImGui::Button("Add Sphere model"))
        {
            int idx = scene->AddPrimitiveModel(PM_SPHERE);
            scene->SetActiveModelIdx(idx);
            bModelControlFrame = true;
        }
        
        
        if(bModelControlFrame)
        {
            ImGui::Text("-------------- Models Control: --------------");

            if (ImGui::Button("Next model"))
            {
                scene->NextModel();
            }
            if (ImGui::Button("Delete model"))
            {
                scene->DeleteActiveModel();
                if (scene->GetActiveModelIdx() == DISABLED)
                {
                    bModelControlFrame = false;
                }
            }
            ImGui::Text("Active model: %d", scene->GetActiveModelIdx());

            static mat4x4 activeModelTransformation;
            activeModelTransformation = scene->GetActiveModelTransformation();
            string sModelTransformation = "";
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    sModelTransformation.append(std::to_string(activeModelTransformation[i][j]) + " ");
                }
                sModelTransformation.append("\n");
            }

            ImGui::Text("Model transformation:\n");
            ImGui::Text(sModelTransformation.c_str());

            static float normals[2] = { 0.1f, 0.1f };
            ImGui::SliderFloat2("vn/fn", normals, 0.1f, 2.0f);

            scene->SetvnScale(normals[0]);
            scene->SetfnScale(normals[1]);

            bool        bShowVertNorms = normals[0] > 0.1f ? true : false;
            bool        bShowFaceNorms = normals[1] > 0.1f ? true : false;
            static bool bShowBorderCube = false;
            static bool bShowSolidColor = true;

            scene->showVerticesNormals(bShowVertNorms);
            scene->showFacesNormals(bShowFaceNorms);
            scene->showBorderCube(bShowBorderCube);
            scene->showSolidColor(bShowSolidColor);

            ImGui::Checkbox("Show vertices normals", &bShowVertNorms);
            ImGui::Checkbox("Show face normals", &bShowFaceNorms);
            ImGui::Checkbox("Show Border Cube", &bShowBorderCube);
            ImGui::Checkbox("Show Solid Color", &bShowSolidColor);
        }

        ImGui::End();
    }

    if (lightingMenu)
    {
        ImGui::Begin("Light");
        ImGui::Text("-------------- Light: --------------");

        static int currentLight = LT_AMBIENT;
    
        static const char LightTypes[28] = { "Ambient\0Specular\0Diffusive\0" };
        ImGui::Combo("Light Source", &currentLight, LightTypes);
        static float direction[3] = { 2,2,2 };
        static float lightCoord[3] = { 0,0,0 };
        static float intencity = 1.f;
        ImGui::SliderFloat3("Light from: (x,y,z)", direction, -10, 10);
        ImGui::SliderFloat3("Light at: (x,y,z)", lightCoord, -10, 10);
        ImGui::SliderFloat("Light Intencity: ", &intencity, -10, 10);

        ImGui::Text("Light from: (%d, %d, %d)", direction[0], direction[1], direction[2]);
        ImGui::SameLine();
        ImGui::Text("at: (%d, %d, %d)", lightCoord[0], lightCoord[1], lightCoord[2]);
        ImGui::SameLine();
        ImGui::Text("with intencity: %d", intencity);

        if (ImGui::Button("Add new light"))
        {
            int idx = scene->AddLight(static_cast<LIGHT_SOURCE_TYPE>(currentLight), intencity, lightCoord, direction);
            scene->SetActiveLightIdx(idx);
            bLightControlFrame = true;
        }

        static bool bShowLight = false;
        bShowLight = (scene->GetActiveLight() == nullptr) ? false : scene->GetActiveLight()->GetLightModel()->isModelRenderingActive();
        ImGui::SameLine();
        if (ImGui::Checkbox("Show Light", &bShowLight))
        {
            if (scene->GetActiveLight() != nullptr) {
                scene->GetActiveLight()->GetLightModel()->setModelRenderingState(bShowLight);
            }
        }

        if (bLightControlFrame)
        {
            ImGui::Text("-------------- Light Control: --------------");

            if (ImGui::Button("Next light model"))
            {
                scene->NextLight();
            }
            if (ImGui::Button("Delete light model"))
            {
                scene->DeleteActiveLight();
                if (scene->GetActiveLightIdx() == DISABLED)
                {
                    bLightControlFrame = false;
                }
            }
            ImGui::Text("Active light: %d", scene->GetActiveLightIdx());

            static mat4x4 activeLightModelTransformation;
            activeLightModelTransformation = scene->GetActiveLightModelTransformation();
            string sLightModelTransformation = "";
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    sLightModelTransformation.append(std::to_string(activeLightModelTransformation[i][j]) + " ");
                }
                sLightModelTransformation.append("\n");
            }

            ImGui::Text("Light Model transformation:\n");
            ImGui::Text(sLightModelTransformation.c_str());

        }

        ImGui::End();
    }

    if (colorMenu)
    {
        ImGui::Begin("Color menu");

        ImGui::Text("-------------- Background: --------------");
        vec4 currentBgCol = scene->GetBgColor();
        static float bg[3] = { currentBgCol.x ,currentBgCol.y ,currentBgCol.z };
        ImGui::ColorEdit3("BG", bg);
        scene->SetBgColor({ bg[0], bg[1], bg[2], 1 });

        ImGui::Text("-------------- Polygons: --------------");
        vec4 currentPolygonCol = scene->GetPolygonColor();
        static float polygon[3] = { currentPolygonCol.x ,currentPolygonCol.y ,currentPolygonCol.z };
        ImGui::ColorEdit3("Polygon", polygon);
        scene->SetPolygonColor({ polygon[0], polygon[1], polygon[2], 1 });

        ImGui::Text("-------------- Wireframe: --------------");
        vec4 currentWireframeCol = scene->GetWireframeColor();
        static float wireframe[3] = { currentWireframeCol.x ,currentWireframeCol.y ,currentWireframeCol.z };
        ImGui::ColorEdit3("Wireframe", wireframe);
        scene->SetWireframeColor({ wireframe[0], wireframe[1], wireframe[2], 1 });
     
        ImGui::End();
    }

    // Demonstrate creating a fullscreen menu bar and populating it.
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoFocusOnAppearing;
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open", "CTRL+O"))
                {
                    nfdchar_t *outPath = nullptr;
                    nfdresult_t result = NFD_OpenDialog("obj;png,jpg", nullptr, &outPath);
                    if (result == NFD_OKAY) {
                        ImGui::Text("Hello from another window!");
                        scene->LoadOBJModel(outPath);
                        bModelControlFrame = true;
                        free(outPath);

                    }
                    else if (result == NFD_CANCEL) {
                    }
                    else {
                    }

                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
                if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
                ImGui::Separator();
                if (ImGui::MenuItem("Cut", "CTRL+X")) {}
                if (ImGui::MenuItem("Copy", "CTRL+C")) {}
                if (ImGui::MenuItem("Paste", "CTRL+V")) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Settings"))
            {
                if (ImGui::MenuItem("Colors"))
                {
                    colorMenu = true;
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help"))
            {
                if (ImGui::MenuItem("Show Demo Menu")) { showDemoWindow = true; }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }
}
