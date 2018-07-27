#pragma once
#include "ImguiMenus.h"
#include <stdio.h>
#include <stdlib.h>
// open file dialog cross platform https://github.com/mlabbe/nativefiledialog
#include <nfd.h>

bool showDemoWindow = false;
bool showAnotherWindow = false;
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

		if (ImGui::Button("Next camera"))
		{
			scene->NextCamera();
		}
		
		ImGui::Text("Active camera: %d", scene->GetActiveCameraIdx());

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


		static glm::mat4x4 activeCameraProjection = glm::mat4x4(0);
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

		static int scaleFactor;
		ImGui::InputInt("scaling factor: ", &scaleFactor);
		if (ImGui::IsKeyPressed(GLFW_KEY_PAGE_UP) || ImGui::Button("Zoom in"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
		{
			scene->ScaleActiveCamera(scaleFactor);
		}
		if (ImGui::IsKeyPressed(GLFW_KEY_PAGE_DOWN) || ImGui::Button("Zoom out"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
		{
			scene->ScaleActiveCamera(1.0f / scaleFactor);
		}

		if (io.MouseWheel > 0)
		{
			scene->ScaleActiveCamera(2);
		}
		if (io.MouseWheel < 0)
		{
			scene->ScaleActiveCamera(1/2);
		}

		ImGui::Text("-------------- Camera movements --------------");
		static int moveFactor = 1;
		ImGui::InputInt("move factor: ", &moveFactor);
		if (ImGui::IsKeyPressed(GLFW_KEY_LEFT) || ImGui::Button("Left"))
		{
			scene->TranslateActiveCameraLeft(moveFactor);
		}
		if (ImGui::IsKeyPressed(GLFW_KEY_RIGHT) || ImGui::Button("Right"))
		{
			scene->TranslateActiveCameraRight(moveFactor);
		}
		if (ImGui::IsKeyPressed(GLFW_KEY_UP) || ImGui::Button("Up"))
		{
			scene->TranslateActiveCameraUp(moveFactor);
		}
		if (ImGui::IsKeyPressed(GLFW_KEY_DOWN) || ImGui::Button("Down"))
		{
			scene->TranslateActiveCameraDown(moveFactor);
		}
		static int angle;
		ImGui::InputInt("rotation angle: ", &angle);
		if (ImGui::IsKeyPressed('A') || ImGui::Button("+X Axis"))
		{
			scene->RotateActiveCameraXAxis(angle);
		}
		if (ImGui::IsKeyPressed('D') || ImGui::Button("-X Axis"))
		{
			scene->RotateActiveCameraXAxis(-angle);
		}
		if (ImGui::IsKeyPressed('W') || ImGui::Button("+Y Axis"))
		{
			scene->RotateActiveCameraYAxis(angle);
		}
		if (ImGui::IsKeyPressed('S') || ImGui::Button("-Y Axis"))
		{
			scene->RotateActiveCameraYAxis(-angle);
		}
		if (ImGui::IsKeyPressed('Q') || ImGui::Button("+Z Axis"))
		{
			scene->RotateActiveCameraZAxis(angle);
		}
		if (ImGui::IsKeyPressed('E') || ImGui::Button("-Z Axis"))
		{
			scene->RotateActiveCameraZAxis(-angle);
		}

		//static float f = 1.0f;
		//static int counter = 0;
		//ImGui::Text("Hello, world!");                           // Display some text (you can use a format string too)
		//ImGui::SliderFloat("float", &f, -100.0f, 100.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
		//{
		//}
		//	ImGui::Checkbox("Demo Window", &showDemoWindow);      // Edit bools storing our windows open/close state
		//ImGui::Checkbox("Another Window", &showAnotherWindow);

		//ImGui::ColorEdit3("clear color", (float*)&clearColor); // Edit 3 floats representing a color

		//ImGui::SameLine();
		//ImGui::Text("counter = %d", counter);

		//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name your windows.
	if (showAnotherWindow)
	{
		int val[2]; val[0] = io.MousePos.x; val[1] = io.MousePos.y;
		ImGui::Begin("Another Window", &showAnotherWindow);
		ImGui::InputInt2("(x,y)", val, 3);
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			showAnotherWindow = false;
		ImGui::End();
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
