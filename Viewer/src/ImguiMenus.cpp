#pragma once
#include "ImguiMenus.h"
#include "Defs.h"
#include <stdio.h>
#include <stdlib.h>
// open file dialog cross platform https://github.com/mlabbe/nativefiledialog
#include <nfd.h>

bool showDemoWindow = false;
bool modelControlWindow = false;
bool colorMenu = false;
bool showFile = false;
glm::vec4 clearColor = glm::vec4(0.4f, 0.55f, 0.60f, 1.00f);

const glm::vec4& GetClearColor()
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
        static int world[3] = { 1,1,1 };
        ImGui::InputInt3("World transformation: (x,y,z)", world);
        ImGui::Text("World transformation: (%d, %d, %d)", world[0], world[1], world[2]);
        if (ImGui::Button("Set world transformation"))
        {
            scene->SetWorldTransformation(HOMOGENEOUS_MATRIX4(world[0], world[1], world[2], 1.0f));
        }
        static glm::mat4x4 worldTransformation = glm::mat4x4(0);
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
        ImGui::Text("-------------- Primitives Models: --------------");

        if (ImGui::Button("Add Cube model"))
        {
            int idx = scene->AddPrimitiveModel(PM_CUBE);
            scene->SetActiveModelIdx(idx);
            modelControlWindow = true;
        }

        if (ImGui::Button("Add Sphere model"))
        {
            int idx = scene->AddPrimitiveModel(PM_SPHERE);
            scene->SetActiveModelIdx(idx);
            modelControlWindow = true;
        }


        ImGui::Text("------------------- Cameras: -------------------");

        static float eye[3] = { 2,2,2 };
        static float at[3] = { 0,0,0 };
        ImGui::SliderFloat3("Look from: (x,y,z)", eye, -10, 10);
        ImGui::SliderFloat3("Look at: (x,y,z)", at, -10, 10);
        ImGui::Text("Look from: (%d, %d, %d)", eye[0], eye[1], eye[2]);
        ImGui::Text("Look at: (%d, %d, %d)", at[0], at[1], at[2]);
        if (ImGui::Button("Add new camera"))
        {
            int idx = scene->AddCamera({ eye[0], eye[1], eye[2], 1.0f }, { at[0], at[1], at[2], 1.0f }, { 0, 1, 0, 1.0f });
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
            if (scene->getActiveCamera() != NULL) {
                scene->getActiveCamera()->getCameraModel()->setModelRenderingState(bShowCamera);
            }
        }


        static glm::mat4x4 activeCameraTransformation = glm::mat4x4(0);
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

        ImGui::Text("Rotate:");
        static float rotAngle = PI / 36.0f;
        ImGui::SliderAngle("rotation angle", &rotAngle, 1.0f, 180.0f);

        static int currentFrame = FRAME_TYPE::FT_CAMERA;
        static int currentAxis = AXIS::Y;
        static int currentRel = ROTATION_REL::RR_WORLD;
        static const char AxisList[6] = { 'X','\0','Y','\0','Z','\0' };
        static const char Relations[11] = { 'W','o','r','l','d','\0',
                                            'S','e','l','f','\0'
                                           };
        static const char Frames[19] = { 'C','a','m','e','r','a','\0' ,
                                        'M','o','d','e','l','\0',
                                        'W','o','r','l','d','\0'
                                      };
        ImGui::Combo("Frame", &currentFrame, Frames);
        ImGui::Combo("Relation", &currentRel, Relations);
        ImGui::Combo("Axis", &currentAxis, AxisList);
        if (!ImGui::IsMouseHoveringAnyWindow() && io.MouseDown[0] && io.MouseDelta.x)
        {
            int direction = io.MouseDelta.x / abs(io.MouseDelta.x);

            switch (currentFrame)
            {
                case FRAME_TYPE::FT_CAMERA:
                {
                    switch (currentRel)
                    {
                        case ROTATION_REL::RR_WORLD:
                        {
                            switch (currentAxis)
                            {
                            case AXIS::X: scene->RotateActiveCameraWorldXAxis(rotAngle * direction); break;
                            case AXIS::Y: scene->RotateActiveCameraWorldYAxis(rotAngle * direction); break;
                            case AXIS::Z: scene->RotateActiveCameraWorldZAxis(rotAngle * direction); break;
                            default: break;
                            }
                        } break;
                        case ROTATION_REL::RR_SELF:
                        {
                            switch (currentAxis)
                            {
                            case AXIS::X: scene->RotateActiveCameraXAxis(rotAngle * direction); break;
                            case AXIS::Y: scene->RotateActiveCameraYAxis(rotAngle * direction); break;
                            case AXIS::Z: scene->RotateActiveCameraZAxis(rotAngle * direction); break;
                            default: break;
                            }
                        } break;
                        default: break;
                    }
                } break;
                case FRAME_TYPE::FT_MODEL:
                {
                    switch (currentRel)
                    {
                        case ROTATION_REL::RR_SELF:
                        {
                            switch (currentAxis)
                            {
                            case AXIS::X: scene->RotateActiveModelXAxis(rotAngle * direction); break;
                            case AXIS::Y: scene->RotateActiveModelYAxis(rotAngle * direction); break;
                            case AXIS::Z: scene->RotateActiveModelZAxis(rotAngle * direction); break;
                            default: break;
                            }
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
                    scene->TranslateActiveCameraXAxis(-moveFactor);
                } break;
                case FRAME_TYPE::FT_MODEL:
                {
                    scene->TranslateActiveModelXAxis(-moveFactor);
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
                scene->TranslateActiveCameraXAxis(moveFactor);
            } break;
            case FRAME_TYPE::FT_MODEL:
            {
                scene->TranslateActiveModelXAxis(moveFactor);
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
                scene->TranslateActiveCameraYAxis(moveFactor);
            } break;
            case FRAME_TYPE::FT_MODEL:
            {
                scene->TranslateActiveModelYAxis(moveFactor);
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
                scene->TranslateActiveCameraYAxis(-moveFactor);
            } break;
            case FRAME_TYPE::FT_MODEL:
            {
                scene->TranslateActiveModelYAxis(-moveFactor);
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
                scene->TranslateActiveCameraZAxis(moveFactor);
            } break;
            case FRAME_TYPE::FT_MODEL:
            {
                scene->TranslateActiveModelZAxis(moveFactor);
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
                scene->TranslateActiveCameraZAxis(-moveFactor);
            } break;
            case FRAME_TYPE::FT_MODEL:
            {
                scene->TranslateActiveModelZAxis(-moveFactor);
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
            projParams.zNear = 0.1,
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
            perspParam.fovy = TO_RADIAN(325),
            perspParam.aspect = 1/*ImGui::GetWindowWidth() / ImGui::GetWindowHeight()*/,
            perspParam.zNear = 0.5f,
            perspParam.zFar = 2.f
        };
        ImGui::SliderAngle("Fovy", &perspParam.fovy, 1);
        ImGui::SliderFloat("Near", &perspParam.zNear, 0.1, 10);
        ImGui::SliderFloat("Far", &perspParam.zFar, 0.2, 20);

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

        static glm::mat4x4 activeCameraProjection;
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

    /////////////////////////////////////////////////////////////////////

        //static float f = 1.0f;
        //static int counter = 0;
        //ImGui::Text("Hello, world!");                           // Display some text (you can use a format string too)
        //ImGui::SliderFloat("float", &f, -100.0f, 100.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
        //{
        //}
        //	ImGui::Checkbox("Demo Window", &showDemoWindow);      // Edit bools storing our windows open/close state
        //ImGui::Checkbox("Another Window", &modelControlWindow);

        //ImGui::ColorEdit3("clear color", (float*)&clearColor); // Edit 3 floats representing a color

        //ImGui::SameLine();
        //ImGui::Text("counter = %d", counter);

        //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name your windows.
    if (modelControlWindow)
    {
        ImGui::Begin("Models");
        ImGui::Text("-------------- Models Control: --------------");

        if (ImGui::Button("Next model"))
        {
            scene->NextModel();
        }
        if (ImGui::Button("Delete model"))
        {
            scene->DeleteActiveModel();
        }
        ImGui::Text("Active model: %d", scene->GetActiveModelIdx());

        static glm::mat4x4 activeModelTransformation;
        activeModelTransformation = scene->GetActiveModelTransformation();
        string sModelTransformation = "";
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
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

        bool bShowVertNorms = normals[0] > 0.1f ? true : false;
        bool bShowFaceNorms = normals[1] > 0.1f ? true : false;
        bool bShowBorderCube = false;
        
        scene->showVerticesNormals(bShowVertNorms);
        scene->showFacesNormals(bShowFaceNorms);
        scene->showBorderCube(bShowBorderCube);

        ImGui::Checkbox("Show vertices normals", &bShowVertNorms);
        ImGui::Checkbox("Show face normals", &bShowFaceNorms);
        ImGui::Checkbox("Show Border Cube", &bShowBorderCube);
       


        //int val[2]; val[0] = io.MousePos.x; val[1] = io.MousePos.y;
        //ImGui::Begin("Another Window", &modelControlWindow);
        //ImGui::InputInt2("(x,y)", val, 3);
        //ImGui::Text("Hello from another window!");
        //if (ImGui::Button("Close Me"))
        //	modelControlWindow = false;
        ImGui::End();
    }

    // 3. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
    if (showDemoWindow)
    {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
        ImGui::ShowDemoWindow(&showDemoWindow);
    }

    if (colorMenu)
    {
        ImGui::Begin("Color menu");

        ImGui::Text("-------------- Background: --------------");
        glm::vec3 currentBgCol = scene->GetBgColor();
        static float bg[3] = { currentBgCol.x ,currentBgCol.y ,currentBgCol.z };
        ImGui::ColorEdit3("BG", bg);
        scene->SetBgColor({ bg[0], bg[1], bg[2] });

        ImGui::Text("-------------- Polygons: --------------");
        glm::vec3 currentPolygonCol = scene->GetPolygonColor();
        static float polygon[3] = { currentPolygonCol.x ,currentPolygonCol.y ,currentPolygonCol.z };
        ImGui::ColorEdit3("Polygon", polygon);
        scene->SetPolygonColor({ polygon[0], polygon[1], polygon[2]});
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
                    nfdchar_t *outPath = NULL;
                    nfdresult_t result = NFD_OpenDialog("obj;png,jpg", NULL, &outPath);
                    if (result == NFD_OKAY) {
                        ImGui::Text("Hello from another window!");
                        scene->LoadOBJModel(outPath);
                        free(outPath);
                        modelControlWindow = true;
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
