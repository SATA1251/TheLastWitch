
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
 // NFD�� �����ε� ���� lib ���尡 �ȵǾ��� Ȯ���ؾ���
// ���� �Ϸ� ���� ���������� �۵�

/// 2023.10.19 �輺��
/// �������� ///�� ������ �ȵǴ� ���� // �ӽ��ذ� ���� ���������� ���� ��
/// lib�� svn�� �ȿö�ͼ� ���尡 �ȵǴ� ������ �ִ�.
/// 

/// <summary>
/// 2024.03.14 �輺��
/// ������ ����� �ε带 ���� ����⸦ �����ϰ��� ������ 
/// �浹�� ���� ����
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

void EpicTool::DataControlWindow::ShowWindow(bool& close, int& selectedObjectIndex) //���̺� ��ư �ٸ� ������ �̵� �ؾ��ҵ�
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

		ImGui::SameLine(); // ���� ���ο� ��ġ

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
				if (ImGui::MenuItem("Inspector"), NULL, &_save_Scene)  // ���� �ӽ÷� �־�� window���� �����Ҽ� �ִ� â��
					// �����츦 �����ϴ� ������ ������ ���� �����ͼ� ������ ���� ������?
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

	// ���� ������ ��ġ�� ���丮�� �̵��ؼ� ProjectL\Bin\x64\Debug ��η� ����
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
			_serialize = new Serialize();  // �������� ����� ������?
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
			
			//for (auto currentObject : KunrealEngine::GetCurrentScene()->GetObjectList()) // ����� ���߿� �� �Ѿ�� �� ����� �κ�
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
		// ���ϸ� ����
		std::string filename = scene; // ���� ���ϸ�

		// ������ ���� ��� ����
		std::string filePath = "Resources/mapData/" + filename;	

		Deserialize* _deserialize = new Deserialize();
		_deserialize->Initialize(filePath);

		_scene2First = true;
		selectedObjectIndex = -1;
	}
	else if (scene == "mapTest3.json" && _scene3First == false)
	{
		// ���ϸ� ����
		std::string filename = scene; // ���� ���ϸ�

		// ������ ���� ��� ����
		std::string filePath = "Resources/mapData/" + filename;

		Deserialize* _deserialize = new Deserialize();
		_deserialize->Initialize(filePath);

		_scene3First = true;
		selectedObjectIndex = -1;
	}
	else if (scene == "mapTest4.json" && _scene4First == false)
	{
		// ���ϸ� ����
		std::string filename = scene; // ���� ���ϸ�

		// ������ ���� ��� ����
		std::string filePath = "Resources/mapData/" + filename;

		Deserialize* _deserialize = new Deserialize();
		_deserialize->Initialize(filePath);

		_scene4First = true;
		selectedObjectIndex = -1;
	}
	else if (scene == "mapTest6.json" && _scene5First == false)
	{
		// ���ϸ� ����
		std::string filename = scene; // ���� ���ϸ�

		// ������ ���� ��� ����
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
			 if (mainCamera->GetComponent<KunrealEngine::Camera>() != NULL)  // ī�޶� ������ ����� Ư�� ī�޶� �˼��ִ� ������ �ʿ� (�±� ��)
			 {
				 mainCamera->GetComponent<KunrealEngine::Camera>()->SetMainCamera();
			}
		 }

	 }

}

