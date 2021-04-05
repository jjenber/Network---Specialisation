#include "WorldServer.h"
#include "Window.h"

#include "../Game/Components/Transform.hpp"

#include <cmath>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "Editor.h"
#include "imgui_internal.h"
#include <iostream>
#include <sstream>

void Editor::Init(std::shared_ptr<Window> aWindowPtr)
{
	myWindowPtr = aWindowPtr;
	myScrolling = ImVec2(300, 300);
}

void Editor::Show()
{

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::GetIO().ConfigDockingNoSplit = false;

	ImGuiWindowFlags flags = 
		ImGuiWindowFlags_NoTitleBar | 
		ImGuiWindowFlags_NoResize | 
		ImGuiWindowFlags_NoMove | 
		ImGuiWindowFlags_NoBringToFrontOnFocus;
	
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	bool open = true;
	ImGui::Begin("Editor", &open, flags);
	
	DrawGrid();
	DrawRegions();

	// Area Server view
	{
		flags = 0;
		flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
		ImGui::Begin("World Server", &open, flags);
		DrawAreaServerListControls();
		DrawAreaServerList();
		ImGui::End();
	}

	ImGui::ShowDemoWindow();
	ImGui::End();
	ImGui::PopStyleVar();
}

void Editor::DrawGrid()
{
	ImGuiIO& io = ImGui::GetIO();

	if (!ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
	{
		if (ImGui::IsMouseDragging(2))
			myScrolling += io.MouseDelta;

		if (io.KeyShift && !io.KeyCtrl)
			myScrolling.x += io.MouseWheel * 16.0f;

		if (!io.KeyShift && !io.KeyCtrl)
		{
		   myScrolling.y += io.MouseWheel * 16.0f;
		   myScrolling.x += io.MouseWheelH * 16.0f;
		}

		if (!io.KeyShift && io.KeyCtrl)
		{
			if (io.MouseWheel != 0)
			{
				ImVec2 mouseRel = ImVec2{ ImGui::GetMousePos().x - ImGui::GetWindowPos().x, ImGui::GetMousePos().y - ImGui::GetWindowPos().y };
				float prevZoom = myZoomFactor;
				myZoomFactor = ImClamp(myZoomFactor + io.MouseWheel * myZoomFactor / 16.f, 0.3f, 3.f);
				float zoomFactor = (prevZoom - myZoomFactor) / prevZoom;
				myScrolling += (mouseRel - myScrolling) * zoomFactor;
			}
		}
	}
	ImColor color        = IM_COL32(90, 90, 90, 65);
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	const float gridCell = 32.0f * myZoomFactor;

	ImVec2 pos  = ImGui::GetWindowPos();
	ImVec2 size = ImGui::GetWindowSize();

	for (float x = fmodf(myScrolling.x, gridCell); x < size.x;)
	{
		drawList->AddLine(ImVec2(x, 0) + pos, ImVec2(x, size.y) + pos, color);
		x += gridCell;
	}

	for (float y = fmodf(myScrolling.y, gridCell); y < size.y;)
	{
		drawList->AddLine(ImVec2(0, y) + pos, ImVec2(size.x, y) + pos, color);
		y += gridCell;
	}

	auto origoX = ImVec2(pos.x, pos.y + myScrolling.y);
	auto origoY = ImVec2(pos.x + myScrolling.x, pos.y);
	
	drawList->AddLine(origoX, ImVec2(origoX + ImVec2(size.x, 0)), 0xFFFFFFFF);
	drawList->AddLine(origoY, ImVec2(origoY + ImVec2(0, size.y)), 0xFFFFFFFF);

	drawList->AddText(WorldToScreenPos(ImVec2(-10.f, -10.f)), 0xFFFFFFFF, "{ 0 : 0 }");
}

void Editor::DrawAreaServerListControls()
{
	static bool enable = true;//myWorldServer.CanStartAreaServer();
	if (!enable)
	{
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}
	if (ImGui::Button("Start Area Server"))
	{
		myWorldServer.InstantiateAreaServers();
	}
	if (!enable)
	{
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}
}

void Editor::DrawAreaServerList()
{
	const auto& areaServers = myWorldServer.GetAreaServerInstanceArray();
	int id = 0;

	std::stringstream header;
	for (const auto& server : areaServers)
	{
		if (server.myStatus != eAreaServerStatus::Shutdown)
		{
			header << "Area Server " << std::to_string(id) << std::endl;
			if (ImGui::CollapsingHeader(header.str().c_str()))
			{
				// Status
				ImGui::Text("Status: ");
				ImGui::SameLine();
				ImGui::TextColored(AreaServerStatusToColor(server.myStatus), AreaServerStatusToText(server.myStatus).c_str());
				// IP
				ImGui::LabelText("##IPAddress", "Address: %s", server.myAddress.ToString().c_str());
				// Entity Count
				ImGui::Text("Entity Count: %i", static_cast<int>(server.myEntities.size()));
			}
			header.str(std::string());
		}
		id++;
	}
}

void Editor::DrawRegions()
{
	const auto& registry = myWorldServer.GetRegistry();
	const float halfsize = myZoomFactor * 2.f;
	auto draw = ImGui::GetWindowDrawList();

	for (int i = 0; i < myWorldServer.GetAreaServerInstanceArray().size(); i++)
	{
		auto& areaServer = myWorldServer.GetAreaServerInstanceArray()[i];
		for (auto& entity : areaServer.myEntities)
		{
			const components::Transform& transform = registry.get<components::Transform>(entity);
			ImVec2 screenPos = WorldToScreenPos(ImVec2(transform.myPosition.x, transform.myPosition.z));

			draw->AddRectFilled(
				ImVec2(screenPos.x - halfsize, screenPos.y - halfsize), 
				ImVec2(screenPos.x + halfsize, screenPos.y + halfsize),
				IM_COL32(255, 255, 255, 125),
				0.2f,
				ImDrawFlags_RoundCornersAll);
		}
		ImVec2 topL = ImVec2(static_cast<float>((i % REGION_ROW_COL)) * REGION_SIZE, static_cast<float>((i / REGION_ROW_COL)) * REGION_SIZE);
		ImVec2 botR = ImVec2(topL.x + REGION_SIZE, topL.y + REGION_SIZE);
		int alpha = 30;
		ImU32 color = 
			areaServer.myStatus == eAreaServerStatus::Running ? IM_COL32(0, 255, 0, alpha) : 
			areaServer.myStatus == eAreaServerStatus::Loading ? IM_COL32(255, 255, 0, alpha) :
			IM_COL32(255, 0, 0, alpha);
		
		draw->AddRectFilled(WorldToScreenPos(topL), WorldToScreenPos(botR), color);
	}
}

ImVec4 Editor::AreaServerStatusToColor(eAreaServerStatus aStatus) const
{
	switch (aStatus)
	{
	case eAreaServerStatus::Shutdown: return ImVec4(1.f, 1.f, 1.f, 1.f);
	case eAreaServerStatus::Idling:   return ImVec4(1.f, 1.f, 1.f, 1.f);
	case eAreaServerStatus::Loading:  return ImVec4(1.f, 1.f, 0.f, 1.f);
	case eAreaServerStatus::Running:  return ImVec4(0.f, 1.f, 0.f, 1.f);
	default: return ImVec4(1.f, 1.f, 1.f, 1.f);
	}
	return ImVec4(1.f, 1.f, 1.f, 1.f);
}

std::string Editor::AreaServerStatusToText(eAreaServerStatus aStatus) const
{
	switch (aStatus)
	{
	case eAreaServerStatus::Shutdown: return "Shutdown";
	case eAreaServerStatus::Idling:   return "Idling";
	case eAreaServerStatus::Loading: return "Loading";
	case eAreaServerStatus::Running: return "Running";
	default:
		return "";
	}
	return std::string();
}

ImVec2 Editor::WorldToScreenPos(const ImVec2& aWorldPosition) const
{
	return ImGui::GetWindowPos() + ImVec2(aWorldPosition.x, -aWorldPosition.y) * myZoomFactor * 0.25f + myScrolling;
}
