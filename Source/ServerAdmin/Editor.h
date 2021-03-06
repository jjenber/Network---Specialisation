#pragma once
#include <imgui.h>
#include <memory>
class Window;
class WorldServer;
class Editor
{
public:
	static constexpr int GRID_SIZE = 1000;

	Editor(WorldServer& aWorldServer) : myWorldServer(aWorldServer) {}
	void Init(std::shared_ptr<Window> aWindowPtr);
	void Show();
	
private:
	void DrawGrid();
	void DrawMenuControls();
	void DrawAreaServerList();
	void DrawRegions();

	void InstantiateMockClient() const;

#pragma region ImGui Utility
	ImVec4 AreaServerStatusToColor(eAreaServerStatus aStatus) const;
	std::string AreaServerStatusToText(eAreaServerStatus aStatus) const;
	ImVec2 WorldToScreenPos(const ImVec2& aWorldPosition) const;
#pragma endregion

	const float myWorldResolutionFactor = 0.25f;
	float myZoomFactor = 1.f;
	ImVec2 myWindowPos;
	ImVec2 myScrolling;
	std::shared_ptr<Window> myWindowPtr;
	WorldServer& myWorldServer;
};

