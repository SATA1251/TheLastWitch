#include "DebugWindow.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "KunrealAPI.h"
#include "SceneManager.h"
#include "DebugType.h"

// 아래는 필요한지 확인 필요


EpicTool::DebugWindow::DebugWindow()
{

}

EpicTool::DebugWindow::~DebugWindow()
{

}


void EpicTool::DebugWindow::ShowHierarchyDebug()
{	
	std::string str;
	switch (_typeHierarchy)
	{
	case EpicTool::CreateObject: // 오브젝트 생성 후 출력해야함
		str = _gameObjectlist[_gameObjectlist.size()-1]->GetObjectName().insert(0, "CreateObject : ");
		_debugTypeList.insert(_debugTypeList.begin(),str);
		break;
	case EpicTool::DeleteObject: // 여기는 지우기 전에 출력해야 할것
		str = _deleteObjectName.insert(0, "DeleteObject : "); //벡터로 담는 형식으로 변경 뒤에 사용해야할것
		_debugTypeList.insert(_debugTypeList.begin(), str);
		break;
	default:
		break;
	}
}




void EpicTool::DebugWindow::ShowInspectorDebug()
{
	std::string str;
	switch (_typeInspector)
	{
	case EpicTool::AddMeshRenderer:
		str = _gameObjectlist[_selectedObjectIndex]->GetObjectName().insert(0, "Add MeshRenderer! ");
		_debugTypeList.insert(_debugTypeList.begin(), str);
		break;
	case EpicTool::AddMesh:
		str = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetMeshName().insert(0, "Add Mesh Name : ");
		_debugTypeList.insert(_debugTypeList.begin(), str);
		break;
	case EpicTool::ChangeMesh:
		str = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetMeshName().insert(0, "Change Mesh Name : ");
		_debugTypeList.insert(_debugTypeList.begin(), str);
		break;
	case EpicTool::AddNormal:
		str = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetNormalName().insert(0, "Add Normal Name : ");
		_debugTypeList.insert(_debugTypeList.begin(), str);
		break;
	case EpicTool::ChangeNormal:
		str = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetNormalName().insert(0, "Change Normal Name : ");
		_debugTypeList.insert(_debugTypeList.begin(), str);
		break;
	case EpicTool::AddDiffuse:
		str = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetTextureName().insert(0, "Add Diffuse Name : ");  // 디퓨즈 가져오는게 없는?
		_debugTypeList.insert(_debugTypeList.begin(), str);
		break;
	case EpicTool::ChangeDiffuse:
		str = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetTextureName().insert(0, "Change Diffuse Name : ");  // 디퓨즈 가져오는게 없는?
		_debugTypeList.insert(_debugTypeList.begin(), str);
		break;
	case EpicTool::AddLight:
		str = _gameObjectlist[_selectedObjectIndex]->GetObjectName().insert(0, "Add Light! ");
		_debugTypeList.insert(_debugTypeList.begin(), str);
		break;
	case EpicTool::AddDirectional:
		str = _gameObjectlist[_selectedObjectIndex]->GetObjectName().insert(0, "Add DirectionalLight! ");
		_debugTypeList.insert(_debugTypeList.begin(), str);
		break;
	case EpicTool::AddPoint:
		str = _gameObjectlist[_selectedObjectIndex]->GetObjectName().insert(0, "Add PointLight! ");
		_debugTypeList.insert(_debugTypeList.begin(), str);
		break;
	case EpicTool::AddCamera:
		str = _gameObjectlist[_selectedObjectIndex]->GetObjectName().insert(0, "Add Camera! ");
		_debugTypeList.insert(_debugTypeList.begin(), str);
		break;
	case EpicTool::DeleteComponentDebug:
		str = _deleteComponentName.insert(0, "DeleteComponent : ");
		_debugTypeList.insert(_debugTypeList.begin(), str);
		break;
	default:
		break;
	}
	_typeInspector = EpicTool::None;
}

void EpicTool::DebugWindow::GetDeleteObjectName(std::string& objectName)
{
	_deleteObjectName = objectName;
}

void EpicTool::DebugWindow::GetDeleteComponentName(std::string& objectName)
{
	_deleteComponentName = objectName;
}

void EpicTool::DebugWindow::ShowDebugLog(const char* text)
{
	ImGui::Text("%s", text);
}

void EpicTool::DebugWindow::ShowWindow(int& selectedObjectIndex)
{
	_gameObjectlist = KunrealEngine::GetCurrentScene()->
		GetObjectList();
	_selectedObjectIndex = selectedObjectIndex;

	static DebugType typeHierarchy = DebugType::None;
	static DebugType typeInspector = DebugType::None;

	ImGui::Begin("Debug Window");// , NULL, ImGuiWindowFlags_NoMove);

	if (_isdebug == true)
	{
		if (typeInspector != _typeInspector || _typeInspector == DebugType::ChangeDiffuse || _typeInspector == DebugType::ChangeNormal)
		{
			ShowInspectorDebug();
		}
		else
		{
			ShowHierarchyDebug();
		}
		_isdebug = false;
	}

	for (auto& debugType : _debugTypeList)
	{
		ImGui::Text(debugType.c_str());
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	if (ImGui::Button("Reset List"))
	{
		_debugTypeList.clear();
	}

	ImGui::End();

	typeHierarchy = _typeHierarchy;
	typeInspector = _typeInspector;
}

void EpicTool::DebugWindow::Initialize()
{
	_gameObjectlist = KunrealEngine::GetCurrentScene()->
		GetObjectList();

	_isdebug = false;
}

void EpicTool::DebugWindow::ShowWindow()
{

}

EpicTool::DebugType EpicTool::DebugWindow::GetDebugType()
{
	return this->_typeHierarchy;
}

void EpicTool::DebugWindow::SetHierarchyDebugType(DebugType debugType)
{
	if (debugType != NULL)
	{
		_typeHierarchy = debugType;
		_isdebug = true;
	}
}

void EpicTool::DebugWindow::SetInspectorDebugType(DebugType debugType)
{
	if (debugType != NULL)
	{
		_typeInspector = debugType;
		_isdebug = true;
	}
}

void EpicTool::DebugWindow::ShowWindow(bool* _open, std::vector<Object>& object)
{

}
