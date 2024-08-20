#include "NavimeshEditor.h"
#include "imgui.h"
#include "KunrealAPI.h"

#include <iostream>
#include <fstream>

EpicTool::NavimeshEditor::NavimeshEditor()
	: _naviIndex(0), _agentHeight(2.0f), _agentRadius(1.2f), _agentMaxClimb(0.9f), _agentMaxSlope(45.0f)
	, _navimeshEditor(nullptr)
{
}

EpicTool::NavimeshEditor::~NavimeshEditor()
{

}

void EpicTool::NavimeshEditor::Initialize()
{
	_navimeshEditor = &KunrealEngine::Navigation::GetInstance();
	_navmeshpolys.resize(_navimeshEditor->GetPackageSize());
	_filePath = "Resources/Navimesh/";

	_fileNameList = _navimeshEditor->GetNavimeshPathList();
	_objList = _navimeshEditor->GetMapObjPathList();
}

void EpicTool::NavimeshEditor::DrawCylinder(ImDrawList* drawList, ImVec2 windowPos, ImVec2 windowSize, float centerX, float centerY, float radius, float height)
{
	ImVec2 center(windowPos.x + centerX, windowPos.y + centerY);

	const int numSegments = 12;
	const float angleStep = 2 * 3.14 / numSegments;


	for (int i = 0; i < numSegments; ++i)
	{
		float angle1 = i * angleStep;
		float angle2 = (i + 1) * angleStep;

		ImVec2 p1(center.x + radius * cosf(angle1), center.y - radius * sinf(angle1));
		ImVec2 p2(center.x + radius * cosf(angle2), center.y - radius * sinf(angle2));


		drawList->AddLine(p1, p2, IM_COL32(0, 255, 0, 255));
		drawList->AddLine(ImVec2(p1.x, p1.y - height), ImVec2(p2.x, p2.y - height), IM_COL32(0, 255, 0, 255));
		drawList->AddLine(p1, ImVec2(p1.x, p1.y - height), IM_COL32(0, 255, 0, 255));
	}

	//drawList->AddCircleFilled(center, radius, IM_COL32(255, 0, 0, 255));
	drawList->AddCircle(ImVec2(center.x, center.y - (_agentMaxClimb * 20.0f)), radius, IM_COL32(0, 255, 0, 255));
	//drawList->AddCircleFilled(ImVec2(center.x, center.y + height), radius, IM_COL32(255, 0, 0, 255));
}

void EpicTool::NavimeshEditor::UnDrawAll()
{
	for (auto navpoly : _navmeshpolys)
	{
		if (navpoly == nullptr)
		{
			continue;
		}
		navpoly->SetActive(false);
	}
}

void EpicTool::NavimeshEditor::ShowWindow()
{

	char fileName[255] = {0};
	int selectedItem = -1;
	ImGui::Begin("Navimesh");

	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 windowSize = ImGui::GetWindowSize();

	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Navimesh");

	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Agent");

	ImGui::SliderFloat("Height", &_agentHeight, 0.0f, 5.0f);

	ImGui::SliderFloat("Radius", &_agentRadius, 0.0f, 10.0f);

	ImGui::SliderFloat("Max Climb", &_agentMaxClimb, 0.0f, 5.0f);

	ImGui::SliderFloat("Max Slope", &_agentMaxSlope, 0.0f, 90.0f);

	if (ImGui::Combo("ObjList", &selectedItem, [](void* data, int idx, const char** out_text)
		{
			auto& items = *static_cast<std::vector<std::string>*>(data);
			if (idx < 0 || idx >= static_cast<int>(items.size()))
				return false;
			*out_text = items[idx].c_str();
			return true;
		},
		static_cast<void*>(&_objList), static_cast<int>(_objList.size())))
	{
		_selectedObjName = _objList[selectedItem];
	}

	if (ImGui::Button("Build"))
	{
		_navimeshEditor->SetAgent(_naviIndex, _agentHeight, _agentMaxSlope, _agentRadius, _agentMaxClimb);
		_navimeshEditor->HandleBuild(_naviIndex, _selectedObjName);

		if (_navmeshpolys[_naviIndex] != nullptr)
		{
			GRAPHICS->DeleteDebugObject(_navmeshpolys[_naviIndex]);
			_navmeshpolys[_naviIndex] = nullptr;        
		}

		std::string name = "navmesh";
		name += std::to_string(_naviIndex);
		UnDrawAll();

		std::vector<DirectX::XMFLOAT3> vertices;
		std::vector<unsigned int> indices;
		_navimeshEditor->GetNavmeshRenderInfo(_naviIndex, vertices, indices);
		_navmeshpolys[_naviIndex] = GRAPHICS->CreateMapDebug(name.c_str(), vertices, indices);
	}

	ImGui::SameLine();

	if (ImGui::InputInt("ObjectIndex", &_naviIndex))
	{
		int maxindex = _navimeshEditor->GetPackageSize() - 1;

		if (_naviIndex > maxindex)
		{
			_naviIndex = maxindex;
		}
		else if (_naviIndex < 0)
		{
			_naviIndex = 0;
		}

		_navimeshEditor->GetAgent(_naviIndex, _agentHeight, _agentMaxSlope, _agentRadius, _agentMaxClimb);
	}


	if (ImGui::InputText("FileName", fileName, sizeof(fileName)))
	{
		_fileNameStr = fileName + std::string(".bin");
	}
	

	ImGui::SameLine();

	if (ImGui::Button("Save"))
	{
		_navimeshEditor->SaveAll(_naviIndex, (_filePath + _fileNameStr).c_str());
	}


	if (ImGui::Combo("LoadList", &selectedItem, [](void* data, int idx, const char** out_text)
		{
			auto& items = *static_cast<std::vector<std::string>*>(data);
			if (idx < 0 || idx >= static_cast<int>(items.size()))
				return false;
			*out_text = items[idx].c_str();
			return true;
		},
		static_cast<void*>(&_fileNameList), static_cast<int>(_fileNameList.size())))
	{
		_selectedFileName = _fileNameList[selectedItem];
	}

	ImGui::SameLine();

	if (ImGui::Button("Load"))
	{
		_selectedFileName = _filePath + _selectedFileName;
		_navimeshEditor->LoadAll(_selectedFileName.c_str(), _naviIndex);

		if (_navmeshpolys[_naviIndex] != nullptr)
		{
			GRAPHICS->DeleteDebugObject(_navmeshpolys[_naviIndex]);
			_navmeshpolys[_naviIndex] = nullptr;
		}

		std::string name = "navmesh";
		name += std::to_string(_naviIndex);
		UnDrawAll();

		std::vector<DirectX::XMFLOAT3> vertices;
		std::vector<unsigned int> indices;
		_navimeshEditor->GetNavmeshRenderInfo(_naviIndex, vertices, indices);
		_navmeshpolys[_naviIndex] = GRAPHICS->CreateMapDebug(name.c_str(), vertices, indices);
	}

	if (ImGui::Button("ResetList"))
	{
		_fileNameList = _navimeshEditor->GetNavimeshPathList();
	}

	ImGui::SameLine();

	DrawCylinder(ImGui::GetWindowDrawList(), windowPos, windowSize, 70.0f, 350.0f, (_agentRadius * 20.0f), (_agentHeight * 20.0f));

	if (ImGui::Button("Show"))
	{
		if (_navmeshpolys[_naviIndex] != nullptr)
		{
			UnDrawAll();
			_navmeshpolys[_naviIndex]->SetActive(true);
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Hide"))
	{
		if (_navmeshpolys[_naviIndex] != nullptr)
		{
			GRAPHICS->DeleteDebugObject(_navmeshpolys[_naviIndex]);
			_navmeshpolys[_naviIndex] = nullptr;
		}
		UnDrawAll();
	}

	ImGui::End();

}

void EpicTool::NavimeshEditor::ShowWindow(bool* _open, std::vector<Object>& object)
{

}
