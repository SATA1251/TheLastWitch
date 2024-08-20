#include "ShowWindowManager.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "FileSave.h"
#include "KunrealAPI.h"
#include "SceneManager.h"
#include "InspectorWindow.h"
#include "HierarchyWindow.h"
#include "DebugWindow.h"
#include "DebugType.h"
#include "Serialize.h"

EpicTool::ShowWindowManager::ShowWindowManager()
    : _inspector(nullptr), _hierarchy(nullptr)
{

}

EpicTool::ShowWindowManager::~ShowWindowManager()
{

}



void EpicTool::ShowWindowManager::Initialize()
{
    _inspector = new InspectorWindow();
    _inspector->Initialize();
    _hierarchy = new HierarchyWindow();
    _hierarchy->Initialize();
	_debugWindow = new DebugWindow;
	_debugWindow->Initialize();
    _serialize = new Serialize(); // �߻�Ŭ������ �ν��Ͻ�ȭ �Ұ�
}

void EpicTool::ShowWindowManager::ShowWindow(int& selectedObjectIndex)
{ 
    IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing Dear ImGui context. Refer to examples app!");

    _debugWindow->ShowWindow(selectedObjectIndex);

    ImGui::Begin("Hierarchy", NULL, ImGuiWindowFlags_NoMove);

    _hierarchy->ShowWindow(selectedObjectIndex);

	//if (ImGui::Button("Save"))
	//{
	//	_serialize->SaveFile();
	//}

    ImGui::End();
   
    _hierarchy->GetHierarchyList(_getGameObjectList);
   
    _inspector->SetGameObjectList(_getGameObjectList);

   // �ſ� �ϵ彺��� ���ø��� ���� �����غ���, ������Ʈ �κ� ������ ������Ʈ�� �����ϴ°� �ƴ�
                          // ������ �ʿ��Ұ��̴�.
    ImGui::Begin("inspector", NULL , ImGuiWindowFlags_NoMove);

    _inspector->ShowWindow(selectedObjectIndex);

    ImGui::End();

	DebugType debugHierarchyType;
	DebugType debugInspectorType;
    std::string deleteObject;

    debugHierarchyType = _debugHierarchyType;
    debugInspectorType = _debugInspectorType;

	_hierarchy->GetDebugType(_debugHierarchyType);
	_inspector->GetDebugType(_debugInspectorType);

	_hierarchy->GetDeleteObjectName(_deleteObject);
	_debugWindow->GetDeleteObjectName(_deleteObject);

    _inspector->GetDeleteComponentName(_deleteComponent);
    _debugWindow->GetDeleteComponentName(_deleteComponent);

    if (debugHierarchyType != _debugHierarchyType)
    {
        _debugWindow->SetHierarchyDebugType(_debugHierarchyType);
    }

    if (debugInspectorType != _debugInspectorType)
    {
        _debugWindow->SetInspectorDebugType(_debugInspectorType);
    }

	

   
}

void EpicTool::ShowWindowManager::GetGameObjetcList(std::vector<KunrealEngine::GameObject*>& instance)
{
    instance = _getGameObjectList;
}

void EpicTool::ShowWindowManager::ShowWindow()
{

}

void EpicTool::ShowWindowManager::ShowWindow(bool* _open, std::vector<Object>& object)
{

}
