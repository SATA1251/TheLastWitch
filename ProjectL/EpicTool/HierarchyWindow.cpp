#include "HierarchyWindow.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "FileSave.h"
#include "KunrealAPI.h"
#include "SceneManager.h"
#include "DebugWindow.h"
#include "DebugType.h"

#include <algorithm>

EpicTool::HierarchyWindow::HierarchyWindow()
    : _createEmptyCount(1), _cubeCount(1), _sphereCount(1), _draggedIndex(-1), _dropTargetIndex(-1), _gameObjectlist(NULL), _tempgameObjectList(NULL),
	_hierarchyWindowX1(0), _hierarchyWindowX2(0), _hierarchyWindowY1(0), _hierarchyWindowY2(0), _isDragged(false), _payloadIndex(0), _show_Context_Menu(false), _isListUpdatePlus(false), _isListUpdateDel(false)
{

}

EpicTool::HierarchyWindow::~HierarchyWindow()
{

}

void EpicTool::HierarchyWindow::Initialize()
{
	_gameObjectlist = KunrealEngine::GetCurrentScene()->
		GetObjectList();
}

void EpicTool::HierarchyWindow::ShowWindow(int& selectedObjectIndex)
{
	UpdateObjectList(selectedObjectIndex);
	_draggedIndex = -1;
	_dropTargetIndex = -1;
	int childCound = 0;
	_DebugType = DebugType::None;
 
	// 딜리트 키로 오브젝트 삭제
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
	{
		if (ImGui::IsKeyReleased(ImGuiKey_Delete) && !_gameObjectlist.empty() && selectedObjectIndex != -1)
		{
			_DebugType = DebugType::DeleteObject;
			_deleteObjectName = _gameObjectlist[selectedObjectIndex]->GetObjectName();

			DeleteObject(_gameObjectlist, selectedObjectIndex);
			_gameObjectlist = KunrealEngine::GetCurrentScene()->GetObjectList();
		}
	}


	if (ImGui::IsMouseReleased(1) && ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows))
	{
		ImGui::CloseCurrentPopup();
		_show_Context_Menu = true;					// 서브를 불러서 팝업에서 팝업을 부름 => 삭제
		ImGui::OpenPopup("Context");
	}

	CopyObject(selectedObjectIndex); // 복사기능
	
	if (_show_Context_Menu)
	{
		if (ImGui::BeginPopupContextItem("Context"))
		{
			if (selectedObjectIndex == -1)
			{
				if (ImGui::MenuItem("Create Empty"))
				{
					std::string objectName = "Create Empty";
					// 조건문으로 하기
					KunrealEngine::GetCurrentScene()->CreateObject(objectName);
					_DebugType = DebugType::CreateObject;
				}
				if (ImGui::MenuItem("Cube"))  // 이 부분은 클릭할때 추가로 띄워야하기에 수정해야함
				{
					std::string objectName = "Cube";
					CreateObject(objectName, _cubeCount);
					_DebugType = DebugType::CreateObject;
				}
				if (ImGui::MenuItem("Sphere"))
				{
					std::string objectName = "Sphere";
					CreateObject(objectName, _sphereCount);
					_DebugType = DebugType::CreateObject;
				}
				UpdateObjectList(selectedObjectIndex);
			}
			if (!_gameObjectlist.empty() && selectedObjectIndex != -1)
			{
				if (ImGui::MenuItem("Delete"))
				{
					_DebugType = DebugType::DeleteObject;
					_deleteObjectName = _gameObjectlist[selectedObjectIndex]->GetObjectName();
					DeleteObject(_gameObjectlist, selectedObjectIndex);
					_gameObjectlist = KunrealEngine::GetCurrentScene()->GetObjectList();
					_isListUpdateDel = true;
					_show_Context_Menu = false;
				}
			}

			ImGui::EndPopup();

		}
	}

	int previousIndentLevel = 0;

	// 들여쓰기
	for (int i = 0; i < _gameObjectlist.size(); ++i)
	{

		ImGui::PushID(i);

		bool activated = _gameObjectlist[i]->GetActivated();

		if (!activated)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // 회색 텍스트 색상
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_Text]);
		}

		int currentIndentLevel = 0;
		KunrealEngine::GameObject* currentObject = _gameObjectlist[i];
		while (currentObject->GetParent() != nullptr)
		{
			currentObject = currentObject->GetParent();
			++currentIndentLevel;
		}


		// 들여쓰기 레벨 체크
		if (currentIndentLevel > previousIndentLevel)
		{
			IndentAll(currentIndentLevel - previousIndentLevel);
		}

		if (currentIndentLevel < previousIndentLevel)
		{
			for (int j = currentIndentLevel; j < previousIndentLevel; ++j)
			{
				ImGui::Unindent();
			}
		}

		// 리스트 출력
		if (ImGui::Selectable(_gameObjectlist[i]->GetObjectName().c_str(), (i == selectedObjectIndex)))  // 드래그 앤 드롭 UI 처리중
		{
			if (i == selectedObjectIndex)
			{
				selectedObjectIndex = -1;
			}
			else
			{
				selectedObjectIndex = i;
			}
		}
		else if (ImGui::IsMouseClicked(0))
		{
			ImVec2 clickPos = ImGui::GetMousePos();
			ImVec2 windowPos = ImGui::GetWindowPos();
			ImVec2 windowSize = ImGui::GetWindowSize();
			//if (!(clickPos.x < windowPos.x || clickPos.y < windowPos.y || clickPos.x > windowPos.x + windowSize.x || clickPos.y > windowPos.y + windowSize.y))
			//{
			//	selectedObjectIndex = -1;
			//}
		}

		ImGui::PopStyleColor();



		// 드래그 앤 드롭
		HandleDragAndDrop(i, selectedObjectIndex, _gameObjectlist);



		previousIndentLevel = currentIndentLevel;

		ImGui::PopID();

    }


	// "Hierarchy" 윈도우 내에서 컨텍스트 메뉴 호출

	_isListUpdatePlus = false;
	_isListUpdateDel = false;

		
}


void EpicTool::HierarchyWindow::CreateObject(std::string objectName, int& count)
{
    int tempCount = count;

	auto iter = find_if(_gameObjectlist.begin(), _gameObjectlist.end(), [&](KunrealEngine::GameObject* object)
		{
			return object->GetObjectName() == objectName;
		});

	if (iter != _gameObjectlist.end())
	{
		KunrealEngine::GetScene("Main")->CreateObject(objectName + "(" + std::to_string(count) + ")");
        count = tempCount + 1;
	}
	else
	{
        KunrealEngine::GetScene("Main")->CreateObject(objectName);
        ++count;
	}
}

// 오브젝트 삭제시 부모관계가 있으면 터짐 
void EpicTool::HierarchyWindow::DeleteObject(std::vector<KunrealEngine::GameObject*>& gameObjectlist, int& selectedObjectIndex)
{
    ImGui::Separator();
    std::string deleteObjectName = gameObjectlist[selectedObjectIndex]->GetObjectName();
    selectedObjectIndex = -1;

    KunrealEngine::GetCurrentScene()->DeleteGameObject(deleteObjectName);

    ImGui::Separator();
}

void EpicTool::HierarchyWindow::UpdateGameObject()  // 리스트 업데이트 부분 현재는 사용 x
{
	if (_draggedIndex != -1 && _dropTargetIndex != -1)
	{
        KunrealEngine::GameObject* draggedItem = _gameObjectlist[_draggedIndex];
        _gameObjectlist.erase(_gameObjectlist.begin() + _draggedIndex);
        _gameObjectlist.insert(_gameObjectlist.begin() + _dropTargetIndex, draggedItem);

        _draggedIndex = -1;
        _dropTargetIndex = -1;
    }
}

void EpicTool::HierarchyWindow::GetDebugType(DebugType& instance)
{
	instance = _DebugType;
}

void EpicTool::HierarchyWindow::GetDeleteObjectName(std::string& objectName)
{
	objectName = _deleteObjectName;
}

void EpicTool::HierarchyWindow::ShowWindow()
{

}

void EpicTool::HierarchyWindow::ShowWindow(bool* p_open, std::vector<Object>& object)
{

}


void EpicTool::HierarchyWindow::HandleDragAndDrop(int index, int& selected, std::vector<KunrealEngine::GameObject*>& gameObjectlist)
{
	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("GameObjectIndex", &index, sizeof(int));
		ImGui::Text("%s", gameObjectlist[index]->GetObjectName().c_str());
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObjectIndex"))
		{
			int payloadIndex = *(const int*)payload->Data;  // payloadIndex : 드래그 한 인덱스, index : 드롭 할 인덱스 



			if (_isListUpdateDel == true)
			{
				--payloadIndex;
			}
			else if(_isListUpdatePlus == true)
			{
				++payloadIndex;
			}

			// 드래그한 오브젝트를 드롭한 오브젝트 아래로 이동
			if (index != payloadIndex)
			{
				if (index < payloadIndex)
				{
					CheckChildListFromUp(gameObjectlist, index, payloadIndex);
				}
				else
				{
					CheckChildListFromDown(gameObjectlist, index, payloadIndex);
				}
				//_isListUpdate = true;
			}
		}
		ImGui::EndDragDropTarget();
	}
}

void EpicTool::HierarchyWindow::CheckChildListFromUp(std::vector<KunrealEngine::GameObject*>& gameObjectlist, int& index, int& payloadIndex)
{
	KunrealEngine::GameObject* draggedObject = gameObjectlist[payloadIndex];
	gameObjectlist.erase(gameObjectlist.begin() + payloadIndex);
	gameObjectlist.insert(gameObjectlist.begin() + (index + 1), draggedObject);

	gameObjectlist[index + 1]->SetParent(gameObjectlist[index]);

	//if (gameObjectlist[index + 1]->CheckChild() == true)   // 위치를 옮기고 
	//{
	//	payloadIndex++;
	//	index++;
	//	CheckChildListFromUp(gameObjectlist, index, payloadIndex);
	//}
}  

void EpicTool::HierarchyWindow::CheckChildListFromDown(std::vector<KunrealEngine::GameObject*>& gameObjectlist, int& index, int& payloadIndex)
{
	KunrealEngine::GameObject* draggedObject = gameObjectlist[payloadIndex];


	gameObjectlist.erase(gameObjectlist.begin() + payloadIndex);
	gameObjectlist.insert(gameObjectlist.begin() + index, draggedObject);

	gameObjectlist[index]->SetParent(gameObjectlist[index - 1]);

	//if (gameObjectlist[index]->CheckChild() == true)
	//{
	//	CheckChildListFromDown(gameObjectlist, index, payloadIndex);
	//}
}

//void EpicTool::HierarchyWindow::CheckInDentParent(KunrealEngine::GameObject* gameObject, KunrealEngine::GameObject* previousGameObject)
//{
//	
//		if (gameObject->GetParent())
//		{
//			if (previousGameObject->GetParent() != nullptr && (gameObject->GetParent()->GetObjectName() == previousGameObject->GetObjectName() || gameObject->GetParent()->GetObjectName() == previousGameObject->GetParent()->GetObjectName()))
//			{
//				ImGui::Indent();
//				CheckInDent(gameObject->GetParent());
//			}
//			else
//			{
//				int currentObjectNum = 0;
//				for (int i = 0; i < _gameObjectlist.size(); i++)
//				{
//					for (int j = 0; j < _gameObjectlist.size(); j++)
//					{
//						if (_gameObjectlist[j]->GetObjectName() == gameObject->GetObjectName())
//						{
//							currentObjectNum = j;
//						}
//					}
//					if (_gameObjectlist[i]->GetObjectName() == gameObject->GetParent()->GetObjectName())
//					{
//
//						if (_isListUpdatePlus == true) // _tempgameObjectList.size() != KunrealEngine::GetCurrentScene()->GetObjectList().size() || 를 빼 봤음
//						{
//							if (i < currentObjectNum)
//							{
//								KunrealEngine::GameObject* currentObject = _gameObjectlist[currentObjectNum];
//								_gameObjectlist.erase(_gameObjectlist.begin() + currentObjectNum);
//								_gameObjectlist.insert(_gameObjectlist.begin() + (i + 1), currentObject);    
//							}
//							else if (i > currentObjectNum)
//							{
//								KunrealEngine::GameObject* currentObject = _gameObjectlist[currentObjectNum];
//								_gameObjectlist.erase(_gameObjectlist.begin() + currentObjectNum);
//								_gameObjectlist.insert(_gameObjectlist.begin() + i, currentObject);
//							}
//
//							_tempgameObjectList = KunrealEngine::GetCurrentScene()->GetObjectList();
//						}
//					}
//				}
//				ImGui::Indent();
//			}
//		}
//		//_isListUpdatePlus = false;
//	
//}

void EpicTool::HierarchyWindow::CheckInDent(KunrealEngine::GameObject* gameObject)
{
	if (gameObject->GetParent())
	{
		ImGui::Indent();
		CheckInDent(gameObject->GetParent());
	}
}

//void EpicTool::HierarchyWindow::CheckUninDentParent(KunrealEngine::GameObject* gameObject, KunrealEngine::GameObject* previousGameObject)
//{
//	if (gameObject->GetParent())
//	{
//		
//		if (previousGameObject->GetParent() != nullptr && (gameObject->GetParent()->GetObjectName() == previousGameObject->GetObjectName() || gameObject->GetParent()->GetObjectName() == previousGameObject->GetParent()->GetObjectName()))
//		{
//			ImGui::Unindent();
//			CheckUninDent(gameObject->GetParent());
//		}
//		else
//		{
//			//if (_isListUpdatePlus == true) //  _tempgameObjectList.size() != KunrealEngine::GetCurrentScene()->GetObjectList().size() ||  를 빼봤음
//			//{
//			//	//ImGui::Unindent();
//			//	_isListUpdatePlus = false;
//			//}
//			
//			ImGui::Unindent();
//		}
//		
//	}
//}

void EpicTool::HierarchyWindow::CheckUninDent(KunrealEngine::GameObject* gameObject)
{
	if (gameObject->GetParent())
	{
		ImGui::Unindent();
		CheckUninDent(gameObject->GetParent());
	}
}

void EpicTool::HierarchyWindow::GetHierarchyList(std::vector<KunrealEngine::GameObject*>& instance)
{
	instance = _gameObjectlist;
}


void EpicTool::HierarchyWindow::UpdateObjectList(int& selectedObjectIndex)
{

	if (_gameObjectlist.size() != KunrealEngine::GetCurrentScene()->GetObjectList().size())
	{
		if (_gameObjectlist.size() > KunrealEngine::GetCurrentScene()->GetObjectList().size())
		{
			_isListUpdateDel = true;
		}
		else if (_gameObjectlist.size() < KunrealEngine::GetCurrentScene()->GetObjectList().size())
		{
			_isListUpdatePlus = true;
		}

		_tempgameObjectList = _gameObjectlist;
		_gameObjectlist = KunrealEngine::GetCurrentScene()->GetObjectList();

	}
}

void EpicTool::HierarchyWindow::IndentAll(int indentLevel)
{
	for (int i = 0; i < indentLevel; ++i)
	{
		ImGui::Indent();
	}
}

void EpicTool::HierarchyWindow::CopyObject(int& selectedObjectIndex)
{
	// 복사 기능 <메쉬와 트렌스폼 정도만>
	if (KunrealEngine::InputSystem::GetInstance()->KeyDown(KEY::D) && KunrealEngine::InputSystem::GetInstance()->KeyDown(KEY::LCTRL) && !_gameObjectlist.empty() && selectedObjectIndex != -1)
	{
		_copyObjectName = _gameObjectlist[selectedObjectIndex]->GetObjectOriginalName();

		KunrealEngine::GetCurrentScene()->CreateObject(_copyObjectName);
		_gameObjectlist = KunrealEngine::GetCurrentScene()->GetObjectList();

		_copyObjectName = _gameObjectlist[_gameObjectlist.size() - 1]->GetObjectName();


		// 트랜스폼 복사

		KunrealEngine::GetCurrentScene()->GetGameObject(_copyObjectName)->AddComponent<KunrealEngine::Transform>();

		KunrealEngine::GetCurrentScene()->GetGameObject(_copyObjectName)->GetComponent<KunrealEngine::Transform>()
			->SetPosition(_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Transform>()->GetPosition().x,
				_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Transform>()->GetPosition().y,
				_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Transform>()->GetPosition().z);


		KunrealEngine::GetCurrentScene()->GetGameObject(_copyObjectName)->GetComponent<KunrealEngine::Transform>()
			->SetRotation(_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Transform>()->GetRotation().x,
				_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Transform>()->GetRotation().y,
				_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Transform>()->GetRotation().z);


		KunrealEngine::GetCurrentScene()->GetGameObject(_copyObjectName)->GetComponent<KunrealEngine::Transform>()
			->SetScale(_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Transform>()->GetScale().x,
				_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Transform>()->GetScale().y,
				_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Transform>()->GetScale().z);


		if (_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>() != nullptr)
		{
			KunrealEngine::GetCurrentScene()->GetGameObject(_copyObjectName)->AddComponent<KunrealEngine::MeshRenderer>();

			KunrealEngine::GetCurrentScene()->GetGameObject(_copyObjectName)->GetComponent<KunrealEngine::MeshRenderer>()->SetMeshObject(
				_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetMeshName().c_str());
		}

		if (_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Particle>() != nullptr)  // 왜 메쉬가 추가로 생겨?
		{
			KunrealEngine::GetCurrentScene()->GetGameObject(_copyObjectName)->AddComponent<KunrealEngine::Particle>();
			KunrealEngine::GetCurrentScene()->GetGameObject(_copyObjectName)->GetComponent<KunrealEngine::Particle>()->SetParticleEffect("Fire", "Resources/Textures/Particles/flare.dds", 1000);

			KunrealEngine::GetCurrentScene()->GetGameObject(_copyObjectName)->GetComponent<KunrealEngine::Particle>()->SetParticleVelocity
			(_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetVelocity(),
				_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetRandomState()
			);

			KunrealEngine::GetCurrentScene()->GetGameObject(_copyObjectName)->GetComponent<KunrealEngine::Particle>()->SetParticleDuration
			(_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetFadeOutTime(),
				_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetLifeTime()
			);

			KunrealEngine::GetCurrentScene()->GetGameObject(_copyObjectName)->GetComponent<KunrealEngine::Particle>()->AddParticleColor
			(_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetColor().x,
				_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetColor().y,
				_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetColor().z
			);

			KunrealEngine::GetCurrentScene()->GetGameObject(_copyObjectName)->GetComponent<KunrealEngine::Particle>()->SetParticleDirection
			(_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetDirection().x,
				_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetDirection().y,
				_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetDirection().z
			);

			KunrealEngine::GetCurrentScene()->GetGameObject(_copyObjectName)->GetComponent<KunrealEngine::Particle>()->SetParticleSize
			(_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetSize().x,
				_gameObjectlist[selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetSize().y
			);

		}

	}
}
