#include "KanaParticleApp.h"
#include <iostream>

#include "GLFW/glfw3.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"

#include "Color.h"
#include "ParticleAffectorSystem.h"
#include "EmitterShape.h"
#include "CpuCacheInfoHelper.h"
#include "ImGuiHelper.h"

KanaParticleApp::KanaParticleApp(GLFWwindow* window)
{
	m_Window = window;
	CpuCacheInfoHelper::PrintLogicalProcessorInformation();
}

void KanaParticleApp::Update(float deltaTime)
{
	
}

void MouseInput()
{
	/*
	static double xpos, ypos;
	int leftMousebutton = glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_1);
	glfwGetCursorPos(window, &xpos, &ypos);
	Vector3 target = Vector3(xpos, WINDOW_HEIGHT - ypos, 0.0);

	static bool targetReached = false;

	if (leftMousebutton == GLFW_PRESS)
	{
		if (targetReached == false)
		{
			if (xpos > WINDOW_WIDTH * 0.3f)
			{
				curPS = NULL;
			}

			std::list<ParticleSystem*>::iterator psIter = m_particleSystemList.begin();
			while (psIter != m_particleSystemList.end())
			{
				ParticleSystem* ps = *psIter;
				if (ps->GetEnable() == true)
				{
					targetReached = ps->CheckPosition(target);

					if (targetReached == true)
					{
						curPS = ps;
						break;
					}
				}
				psIter++;
			}

			

			if (curPS == NULL) {}
		}
	}

	if (targetReached == true && curPS != NULL)
	{
		glfwGetCursorPos(window, &xpos, &ypos);
		Vector3 pos = Vector3(xpos, WINDOW_HEIGHT - ypos, 0.0);
		curPS->SetPos(pos);
	}

	if (leftMousebutton == GLFW_RELEASE)
	{
		targetReached = false;
	}
	*/
}

void KanaParticleApp::Render()
{
	ScopedImGuiColorAndStyle appScopedStyle;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	const ImGuiViewport* mainViewport = ImGui::GetMainViewport();
	
	if (!mainViewport)
	{
		std::cout << "Error: Main Viewport is not available";
		return;
	}

	const float propertyTabWidth = mainViewport->Size.x * 0.3f;
	const float propertyTabHeight = mainViewport->Size.y;

	RenderPropertyTab(mainViewport->Pos, ImVec2{ propertyTabWidth, propertyTabHeight });
	RenderSceneTab(*mainViewport, propertyTabWidth);
}

void KanaParticleApp::RenderPropertyTab(ImVec2 propertyTabPos, ImVec2 propertyTabSize)
{
	ImGuiWindowFlags propertiesTab = 0;
	propertiesTab |= ImGuiWindowFlags_NoDocking;
	propertiesTab |= ImGuiWindowFlags_NoResize;
	propertiesTab |= ImGuiWindowFlags_NoMove;
	propertiesTab |= ImGuiWindowFlags_AlwaysAutoResize;
	propertiesTab |= ImGuiWindowFlags_NoCollapse;
	propertiesTab |= ImGuiWindowFlags_NoBringToFrontOnFocus;

	ImGui::SetNextWindowPos(propertyTabPos);
	ImGui::SetNextWindowSize(propertyTabSize);

	ScopedImGuiColorAndStyle propertyTabScopedStyle;

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ColorHelper::Black15);
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ColorHelper::Purple);
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ColorHelper::Purple);
	ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ColorHelper::Purple);
	ImGui::PushStyleColor(ImGuiCol_Tab, ColorHelper::Purple);
	ImGui::PushStyleColor(ImGuiCol_TabActive, ColorHelper::Purple);
	ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, ColorHelper::Purple);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
	//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 20));

	if (ImGui::Begin("Properties", nullptr, propertiesTab))
	{
		RenderWorld::PropertyPanelSystem gg;
		gg.Update();
		ImGui::End();
	}

}

//We want the scene view to be rendered beside the property panel
void KanaParticleApp::RenderSceneTab(const ImGuiViewport& mainViewport, float propertyTabWidth)
{
	ImGuiWindowFlags sceneTab = 0;
	sceneTab |= ImGuiWindowFlags_NoTitleBar;
	sceneTab |= ImGuiWindowFlags_NoDocking;
	sceneTab |= ImGuiWindowFlags_NoResize;
	sceneTab |= ImGuiWindowFlags_NoMove;
	sceneTab |= ImGuiWindowFlags_AlwaysAutoResize;
	sceneTab |= ImGuiWindowFlags_NoCollapse;
	sceneTab |= ImGuiWindowFlags_NoBringToFrontOnFocus;

	ImVec2 sceneTabPos = { mainViewport.Pos.x + propertyTabWidth, mainViewport.Pos.y };

	float sceneTabWidth = mainViewport.Size.x - propertyTabWidth;
	float sceneTabHeight = mainViewport.Size.y;

	ImVec2 sceneTabSize = { sceneTabWidth, sceneTabHeight };


	ImGui::SetNextWindowPos(sceneTabPos);
	ImGui::SetNextWindowSize(sceneTabSize);

	if (ImGui::Begin("Scene", nullptr, sceneTab))
	{
		RenderParticleData();
		ImGui::End();
	}
}



void KanaParticleApp::RenderParticleData()
{
	ImVec2 winPos = ImGui::GetWindowPos();
	ImVec2 startPos = winPos + ImVec2(100.0f, 100.0f);
	
	ImVec2 winSize = ImGui::GetWindowSize();
	ImVec2 winCenterSize = ImGui::GetWindowSize() / 2;
	ImVec2 endSize = winSize - ImVec2(200.0f, 200.0f);

	uint32_t particleSize = 1;
	ImVec2 radius = endSize / static_cast<float>(particleSize);
	ImVec2 particleRadius = radius / 2;

	//auto mousePos = ImGui::GetMousePos();
	//ImVec2 radius = { 10.0f, 10.0f };
	//ImVec2 minRect = mousePos - radius;
	//ImVec2 maxRect = mousePos + radius;

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	ImVec2 pos = winPos + winCenterSize;

	float centerScale = 250 * 0.5f;

	float rot = 0.001f * ::GetTickCount();

	float cos_a = cosf(rot);
	float sin_a = sinf(rot);

	ImVec2 point1 = pos + ImRotate(ImVec2(-centerScale, -centerScale), cos_a, sin_a); //left top corner
	ImVec2 point2 = pos + ImRotate(ImVec2(+centerScale, -centerScale), cos_a, sin_a); //right top corner
	ImVec2 point3 = pos + ImRotate(ImVec2(+centerScale, +centerScale), cos_a, sin_a); //right bottom corner
	ImVec2 point4 = pos + ImRotate(ImVec2(-centerScale, +centerScale), cos_a, sin_a); //left bottom corner

	drawList->AddQuad(point1, point2, point3, point4, ImGui::ColorConvertFloat4ToU32(ColorHelper::Fuchsia));

	return;

	for (uint32_t i = 0; i < particleSize; i++)
	{
		for (uint32_t j = 0; j < particleSize; j++)
		{
			int rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
			float xPos = startPos.x + (radius.x * i);
			float yPos = startPos.y + (radius.y * j);
			ImVec2 minRect = { xPos, yPos };
			ImVec2 maxRect = minRect + ImVec2{particleRadius.x, particleRadius.x};

			ImVec2 center = (minRect + maxRect) / 2;
			
			float cos_a = cosf(10.0f * ::GetTickCount());
			float sin_a = sinf(10.0f * ::GetTickCount());
			
			/*
			ImVec2 pos[4] = {
				center + ImRotate(ImVec2(-size.x * 0.5f, -size.y * 0.5f), cos_a, sin_a),
				center + ImRotate(ImVec2(+size.x * 0.5f, -size.y * 0.5f), cos_a, sin_a),
				center + ImRotate(ImVec2(+size.x * 0.5f, +size.y * 0.5f), cos_a, sin_a),
				center + ImRotate(ImVec2(-size.x * 0.5f, +size.y * 0.5f), cos_a, sin_a)
			};
			*/



			drawList->AddRectFilled(minRect, maxRect, ImGui::ColorConvertFloat4ToU32(ColorHelper::Fuchsia));
			
			/*
			auto& buf = draw_list->VtxBuffer;
			ImVec2 centerRot = ImRotate(center, s, c) - center;
			for (int i = rotation_start_index; i < buf.Size; i++)
			{
				buf[i].pos = ImRotate(buf[i].pos, c, s) - centerRot;
			}
			*/


			//minRect = ImRotate(minRect, cos_a, sin_a);
			//maxRect = ImRotate(maxRect, cos_a, sin_a);

			
			/*
			//float s = sin(0.0005f * ::GetTickCount()), c = cos(0.0005f * ::GetTickCount());
			float s = sinf(180), c = cosf(180);
			
			ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX); // bounds

			auto& buf = draw_list->VtxBuffer;

			for (int i = rotation_start_index; i < buf.Size; i++)
			{
				l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);
			}

			ImVec2 center = ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2); // or use _ClipRectStack?
			
			ImVec2 centerRot = ImRotate(center, s, c) - center;

			for (int i = rotation_start_index; i < buf.Size; i++)
			{
				buf[i].pos = ImRotate(buf[i].pos, s, c) - centerRot;
			}
			*/
		}
	}


	/*
	for (int i = 0; i < particleSize; i++)
	{
		for (int j = 0; j < particleSize; j++)
		{
			float xPos = startPos.x + ((radius.x + particleRadius.x) * i);
			float yPos = startPos.y + ((radius.y + particleRadius.y) * j);
			ImVec2 minRect = { xPos, yPos };
			ImVec2 maxRect = minRect + particleRadius;

			//pos += particleRadius + radius;

			//ImVec2 minRect = pos - (radius * 2);
			//ImVec2 maxRect = pos + (radius * 2);

			draw_list->AddRectFilled(minRect, maxRect, ImGui::ColorConvertFloat4ToU32(ColorHelper::Fuchsia));

		}
	}
	*/

	/*
	for (int i = 0; i < 1; i++)
	{
		const ImRect check_bb(minRect, maxRect);

		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		const bool is_rect_visible = check_bb.Overlaps(window->ClipRect);
		if (!is_rect_visible) continue;

		// If we can render in terms of painters algorithm with even spacing between particles
		// then we can use cursors based clipping solution
		// But I want the random movement and intersecting so we leave it at that
		//ImGui::ItemSize(check_bb); // - This moves the cursor from left to right


		draw_list->AddRectFilled(minRect, maxRect, ImGui::ColorConvertFloat4ToU32(ColorHelper::Fuchsia));
	}
	*/
}

