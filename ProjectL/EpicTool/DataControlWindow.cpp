
#include "imgui.h"
//#include "FileSave.h"
#include <nlohmann/json.hpp> 
#include <iostream>
#include <fstream>
#include "DataControlWindow.h"
#include "KunrealAPI.h"
#include "Serialize.h"
#include "Deserialize.h"
#include "ExportObj.h"

#include <string.h>
 // NFD를 쓸것인데 아직 lib 빌드가 안되었다 확인해야함
// 수정 완료 이제 정상적으로 작동

/// 2023.10.19 배성근
/// 집에서는 ///가 세팅이 안되는 문제 // 임시해결 뭐가 문제인지는 아직 모름
/// lib가 svn에 안올라와서 빌드가 안되는 문제가 있다.
/// 

/// <summary>
/// 2024.03.14 배성근
/// 에디터 사용중 로드를 통한 덮어쓰기를 구현하고자 했으나 
/// 충돌로 인해 보류
/// </summary>

EpicTool::DataControlWindow::DataControlWindow()
    :_show_save_editor(true), _save_Scene(true), _new_Scene(true), _opt_padding(false), _fileSave(nullptr), _scene1First(false), _scene2First(false), _scene3First(false), _scene4First(false), _scene5First(false)
{

}

EpicTool::DataControlWindow::~DataControlWindow()
{

}

void EpicTool::DataControlWindow::ShowWindow()
{

}

void EpicTool::DataControlWindow::ShowWindow(bool& close, int& selectedObjectIndex) //세이브 버튼 다른 곳으로 이동 해야할듯
{
	std::string samplefilePath = _saveFilePath;

	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	
	//ChangeScene(selectedObjectIndex);
	

	if (_show_save_editor)
	{
		ImGui::Begin(" ", &_show_save_editor, ImGuiWindowFlags_MenuBar);

		if (ImGui::Button("Save"))
		{
			SaveToFile(samplefilePath);

		}

		ImGui::SameLine();

		if (ImGui::Button("Load"))
		{
			LoadToFile(samplefilePath);
		}

		ImGui::SameLine();

		char fileName[255] = { 0 };

		if (ImGui::InputText(" ", fileName, sizeof(fileName)))
		{
			_exportObjName = fileName;
		}

		ImGui::SameLine();

		if (ImGui::Button("ExportObj"))
		{
			_exportObj->ExportToObj(_exportObjName);
		}

		ImGui::SameLine(); // 같은 라인에 배치

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene", NULL, &_new_Scene))
				{

				}
				if (ImGui::MenuItem("Save Scene", NULL, &_save_Scene))
				{

				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::MenuItem("Inspector"), NULL, &_save_Scene)  // 변수 임시로 넣어둠 window들을 조작할수 있는 창임
					// 윈도우를 조절하던 변수를 참조를 통해 가져와서 만지는 것이 좋을까?
				{

				}
				if (ImGui::MenuItem("Hierarchy"), NULL, &_save_Scene)
				{

				}
				if (ImGui::MenuItem("GameWindow"), NULL, &_save_Scene)
				{

				}
				if (ImGui::MenuItem("GraphicsWindow"), NULL, &_save_Scene)
				{

				}
				if (ImGui::MenuItem("ResouceWindow"), NULL, &_save_Scene)
				{

				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::End();
	}
	else
	{
		close = true;
	}
	
}

void EpicTool::DataControlWindow::ShowWindow(bool* _open, std::vector<Object>& object)
{

}

void EpicTool::DataControlWindow::Initialize()
{
	_exportObj = new ExportObj();

	TCHAR szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH);
	_executablePath = szPath;

	// 실행 파일이 위치한 디렉토리로 이동해서 ProjectL\Bin\x64\Debug 경로로 변경
	_executablePath = _executablePath.parent_path().parent_path().parent_path().parent_path() / "EpicTool" / "mapData";
}
 
void EpicTool::DataControlWindow::SaveToFile(const std::string& filePath)
{
    nfdchar_t* outPath = NULL;
    nfdresult_t result = NFD_SaveDialog("json", "json", &outPath);


    if (result == NFD_OKAY) {
        if (outPath) {
            std::string chosenPath = outPath;
            if (chosenPath.rfind(".json") == std::string::npos)
            {
                chosenPath += ".json";
            }

            _saveFilePath = chosenPath;

            free(outPath);
			_serialize = new Serialize();  // 루프문에 없어면 터지네?
			_serialize->SaveFile(_saveFilePath);
        }
    }
}

void EpicTool::DataControlWindow::LoadToFile(const std::string& filePath)
{
	nfdchar_t* outPath = NULL;
	nfdresult_t result = NFD_OpenDialog("json", NULL, &outPath);

	if (result == NFD_OKAY)
	{
		if (outPath)
		{
			std::string chosenPath = outPath;

			_loadFilePath = chosenPath;

			free(outPath);
			
			//for (auto currentObject : KunrealEngine::GetCurrentScene()->GetObjectList()) // 여기는 나중에 쓸 넘어갈때 다 지우는 부분
			//{
			//	KunrealEngine::GetCurrentScene()->DeleteGameObject(currentObject->GetObjectName());
			//}

			//KunrealEngine::GetCurrentScene()->GetObjectList().clear(); 

			Deserialize* _deserialize = new Deserialize();
			_deserialize->Initialize(_loadFilePath);			
		}
	}
}

void EpicTool::DataControlWindow::ChangeScene(int& selectedObjectIndex)
{
	std::string scene = KunrealEngine::GetCurrentScene()->GetSceneName();

	 if (scene == "mapTest2.json" && _scene2First == false)
	{
		// 파일명 설정
		std::string filename = scene; // 예시 파일명

		// 파일의 절대 경로 생성
		std::string filePath = "Resources/mapData/" + filename;	

		Deserialize* _deserialize = new Deserialize();
		_deserialize->Initialize(filePath);

		_scene2First = true;
		selectedObjectIndex = -1;
	}
	else if (scene == "mapTest3.json" && _scene3First == false)
	{
		// 파일명 설정
		std::string filename = scene; // 예시 파일명

		// 파일의 절대 경로 생성
		std::string filePath = "Resources/mapData/" + filename;

		Deserialize* _deserialize = new Deserialize();
		_deserialize->Initialize(filePath);

		_scene3First = true;
		selectedObjectIndex = -1;
	}
	else if (scene == "mapTest4.json" && _scene4First == false)
	{
		// 파일명 설정
		std::string filename = scene; // 예시 파일명

		// 파일의 절대 경로 생성
		std::string filePath = "Resources/mapData/" + filename;

		Deserialize* _deserialize = new Deserialize();
		_deserialize->Initialize(filePath);

		_scene4First = true;
		selectedObjectIndex = -1;
	}
	else if (scene == "mapTest6.json" && _scene5First == false)
	{
		// 파일명 설정
		std::string filename = scene; // 예시 파일명

		// 파일의 절대 경로 생성
		std::string filePath = "Resources/mapData/" + filename;

		Deserialize* _deserialize = new Deserialize();
		_deserialize->Initialize(filePath);

		_scene5First = true;
		selectedObjectIndex = -1;
	}
	else if (scene == "Main" && _scene1First == false)
	{
		 auto gameObjectList = KunrealEngine::GetCurrentScene()->GetObjectList();

		 for (auto mainCamera : gameObjectList)
		 {
			 if (mainCamera->GetComponent<KunrealEngine::Camera>() != NULL)  // 카메라를 여러개 쓸경우 특정 카메라를 알수있는 수단이 필요 (태그 등)
			 {
				 mainCamera->GetComponent<KunrealEngine::Camera>()->SetMainCamera();
			}
		 }

	 }

}

