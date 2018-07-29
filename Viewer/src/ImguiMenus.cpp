#pragma once
#include "ImguiMenus.h"
#include <stdio.h>
#include <stdlib.h>
// open file dialog cross platform https://github.com/mlabbe/nativefiledialog
#include <nfd.h>

bool showDemoWindow = false;
bool modelControlWindow = false;
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

		if (ImGui::Button("Add sphere model"))
		{
			int idx = scene->AddPrimitiveModel(PM_SPHERE);
			scene->SetActiveModelIdx(idx);
			modelControlWindow = true;
		}
		if (ImGui::Button("Add cube model"))
		{
			int idx = scene->AddPrimitiveModel(PM_CUBE);
			scene->SetActiveModelIdx(idx);
			modelControlWindow = true;
		}

		ImGui::Text("-------------- Cameras Control: --------------");
		static int eye[3] = { 0,0,0 };
		static int at[3] = { 0,0,0 };
		ImGui::InputInt3("Look from: (x,y,z)", eye);
		ImGui::InputInt3("Look at: (x,y,z)", at);
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
		
		ImGui::Text("Active camera: %d", scene->GetActiveCameraIdx());


		PROJ_PARAMS projParams = { 0 };
// 		void Camera::Ortho(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar)
		ImGui::InputFloat("Left",  &projParams.left);
		ImGui::InputFloat("Right", &projParams.right);
		ImGui::InputFloat("Bottom",&projParams.bottom);
		ImGui::InputFloat("Top",   &projParams.top);
		ImGui::InputFloat("zNear", &projParams.zNear);
		ImGui::InputFloat("zFar",  &projParams.zFar);
		if (ImGui::Button("Orthographic Projection"))
		{
			scene->setOrthoProjection(projParams);
		}
		ImGui::SameLine();
		if (ImGui::Button("Frustum"))
		{
			scene->setFrustum(projParams);
		}


		static glm::mat4x4 activeCameraTransformation = glm::mat4x4(0);
		activeCameraTransformation = scene->GetActiveCameraTransformation();
		string sCameraTransform = "";
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++) 
			{
				sCameraTransform.append(std::to_string(activeCameraTransformation[j][i]) + " ");
			}
			sCameraTransform.append("\n");
		}

		ImGui::Text("Camera transformation:\n");
		ImGui::Text(sCameraTransform.c_str());


		static glm::mat4x4 activeCameraProjection;
		activeCameraProjection = scene->GetActiveCameraProjection();
		string sCameraProjection = "";
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++) 
			{
				sCameraProjection.append(std::to_string(activeCameraProjection[j][i]) + " ");
			}
			sCameraProjection.append("\n");
		}

		ImGui::Text("Camera Projection:\n");
		ImGui::Text(sCameraProjection.c_str());

		//Camera sacaling:
		static float camScaleFactor = 1.5f;
		ImGui::SliderFloat("scaling factor:", &camScaleFactor, 1.0f, 10.0f);

		if (ImGui::IsKeyPressed(GLFW_KEY_PAGE_UP) || ImGui::Button("Zoom in"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
		{
			scene->ScaleActiveCamera(camScaleFactor);
		}
		ImGui::SameLine();
		if (ImGui::IsKeyPressed(GLFW_KEY_PAGE_DOWN) || ImGui::Button("Zoom out"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
		{
			scene->ScaleActiveCamera(1.0f / camScaleFactor);
		}

		if (io.MouseWheel > 0)
		{
			scene->ScaleActiveCamera(2.0f);
		}
		if (io.MouseWheel < 0)
		{
			scene->ScaleActiveCamera(1.0f/2.0f);
		}

		//Camera moves:
		
		static float camMoveFactor = 10.0f;
		ImGui::SliderFloat("move factor:", &camMoveFactor, 1.0f, 300.0f);

		if (ImGui::IsKeyPressed(GLFW_KEY_LEFT) || ImGui::Button("  Left  "))
		{
			scene->TranslateActiveCameraXAxis(camMoveFactor);
		}
		ImGui::SameLine();
		if (ImGui::IsKeyPressed(GLFW_KEY_RIGHT) || ImGui::Button("  Right "))
		{
			scene->TranslateActiveCameraXAxis(-camMoveFactor);
		}
		if (ImGui::IsKeyPressed(GLFW_KEY_UP) || ImGui::Button("  Up  "))
		{
			scene->TranslateActiveCameraYAxis(-camMoveFactor);
		}
		ImGui::SameLine();
		if (ImGui::IsKeyPressed(GLFW_KEY_DOWN) || ImGui::Button("  Down  "))
		{
			scene->TranslateActiveCameraYAxis(camMoveFactor);
		}
		if (ImGui::IsKeyPressed('F') || ImGui::Button("Forward"))
		{
			scene->TranslateActiveCameraZAxis(-camMoveFactor);
		}
		ImGui::SameLine();
		if (ImGui::IsKeyPressed('B') || ImGui::Button("  Back "))
		{
			scene->TranslateActiveCameraZAxis(camMoveFactor);
		}

		//Camera rotation:

		static float cameraAngle = 5.0f;
		ImGui::SliderFloat("rotation angle:", &cameraAngle, 1.0f, 90.0f);

		if (ImGui::IsKeyPressed('X') || ImGui::Button("+X Axis"))
		{
			scene->RotateActiveCameraXAxis(cameraAngle);
		}
		ImGui::SameLine();
		if (ImGui::IsKeyPressed('D') || ImGui::Button("-X Axis"))
		{
			scene->RotateActiveCameraXAxis(-cameraAngle);
		}
		if (ImGui::IsKeyPressed('W') || ImGui::Button("+Y Axis"))
		{
			scene->RotateActiveCameraYAxis(cameraAngle);
		}
		ImGui::SameLine();
		if (ImGui::IsKeyPressed('S') || ImGui::Button("-Y Axis"))
		{
			scene->RotateActiveCameraYAxis(-cameraAngle);
		}
		if (ImGui::IsKeyPressed('Q') || ImGui::Button("+Z Axis"))
		{
			scene->RotateActiveCameraZAxis(cameraAngle);
		}
		ImGui::SameLine();
		if (ImGui::IsKeyPressed('E') || ImGui::Button("-Z Axis"))
		{
			scene->RotateActiveCameraZAxis(-cameraAngle);
		}


	

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
		ImGui::Text("-------------- Models Control: --------------");

		static int world[3] = { 0,0,0 };
		ImGui::InputInt3("World transformation: (x,y,z)", world);
		ImGui::Text("World transformation:: (%d, %d, %d)", world[0], world[1], world[2]);

		if (ImGui::Button("Next model"))
		{
			scene->NextModel();
		}

		ImGui::Text("Active model: %d", scene->GetActiveModelIdx());

		static glm::mat4x4 activeModelWorldTransformation;
		activeModelWorldTransformation = scene->GetActiveModelWorldTransformation();
		string sModelTransform = "";
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				sModelTransform.append(std::to_string(activeModelWorldTransformation[j][i]) + " ");
			}
			sModelTransform.append("\n");
		}

		ImGui::Text("Model world transformation:\n");
		ImGui::Text(sModelTransform.c_str());

		//Model sacaling:
		static float modelScaleFactor = 1.5f;
		ImGui::SliderFloat("scaling factor:", &modelScaleFactor, 1.0f, 10.0f);

		if (ImGui::Button("Bigger"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
		{
			scene->ScaleActiveModel(modelScaleFactor);
		}
		if (ImGui::Button("Smaller"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
		{
			scene->ScaleActiveModel(1.0f / modelScaleFactor);
		}

		//Model moves:

		static float modelMoveFactor = 10.0f;
		ImGui::SliderFloat("move factor:", &modelMoveFactor, 1.0f, 300.0f);

		if (ImGui::Button("Left"))
		{
			scene->TranslateActiveModelXAxis(-modelMoveFactor);
		}
		if (ImGui::Button("Right"))
		{
			scene->TranslateActiveModelXAxis(modelMoveFactor);
		}
		if (ImGui::Button("Up"))
		{
			scene->TranslateActiveModelYAxis(modelMoveFactor);
		}
		if (ImGui::Button("Down"))
		{
			scene->TranslateActiveModelYAxis(-modelMoveFactor);
		}
		if (ImGui::Button("Forward"))
		{
			scene->TranslateActiveModelZAxis(modelMoveFactor);
		}
		if (ImGui::Button("Back"))
		{
			scene->TranslateActiveModelZAxis(-modelMoveFactor);
		}

		//Model rotation:

		static float modelAngle = 5.0f;
		ImGui::SliderFloat("rotation angle:", &modelAngle, 1.0f, 90.0f);

		if ((io.MouseDown[0] && io.MouseDelta.y > 0) || ImGui::Button("+X Axis"))
		{
			scene->RotateActiveModelXAxis(modelAngle);
		}
		if ((io.MouseDown[0] && io.MouseDelta.y < 0) || ImGui::Button("-X Axis"))
		{
			scene->RotateActiveModelXAxis(-modelAngle);
		}
		if ((io.MouseDown[0] && io.MouseDelta.x > 0) || ImGui::Button("+Y Axis"))
		{
			scene->RotateActiveModelYAxis(modelAngle);
		}
		if ((io.MouseDown[0] && io.MouseDelta.x < 0) || ImGui::Button("-Y Axis"))
		{
			scene->RotateActiveModelYAxis(-modelAngle);
		}
		if ((io.MouseDown[1] && io.MouseDelta.x > 0) || ImGui::Button("+Z Axis"))
		{
			scene->RotateActiveModelZAxis(modelAngle);
		}
		if ((io.MouseDown[1] && io.MouseDelta.x < 0) || ImGui::Button("-Z Axis"))
		{
			scene->RotateActiveModelZAxis(-modelAngle);
		}




		//int val[2]; val[0] = io.MousePos.x; val[1] = io.MousePos.y;
		//ImGui::Begin("Another Window", &modelControlWindow);
		//ImGui::InputInt2("(x,y)", val, 3);
		//ImGui::Text("Hello from another window!");
		//if (ImGui::Button("Close Me"))
		//	modelControlWindow = false;
		//ImGui::End();
	}

	// 3. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
	if (showDemoWindow)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
		ImGui::ShowDemoWindow(&showDemoWindow);
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
			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("Show Demo Menu")) { showDemoWindow = true; }
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}
}
