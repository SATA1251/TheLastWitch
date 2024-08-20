#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "FileSave.h"
#include "KunrealAPI.h"
#include "SceneManager.h"
#include "InspectorWindow.h"
#include "DebugType.h"
#include "DebugWindow.h"
#include "GraphicWindow.h"
#include "ImGuizmo.h"
#include "HierarchyWindow.h"


EpicTool::InspectorWindow::InspectorWindow()
	:_gameObjectlist(NULL), _selectedObjectIndex(0), _tranform(nullptr), _meshState(false), _cameraFix(false)//, _selectedMesh(0)
	, _selectedNormal(0), _comboMeshSelect(-1), _comboNormalSelect(-1), _comboDiffuseSelect(-1), _selectObjectNumber(-1), _isObjectActive(true), _comboLightSelect(-1), _comboImageSelect(-1), _comboSoundSelect(-1), _soundVolEditor(100), _comboNewSoundSelect(-1)
	, _isLightActive(true), _isCameraActive(true), _ambient{ 0 }, _diffuse{ 0 }, _specular{ 0 }, _direction{ 0 }, _lightGet(true), _pointDiffuse{ 0 }, _pointRange(0),
	_pointAmbient{0}, _pointSpecular{0}, _isPickedObjectName{0}, _quaternion(), _animationSpeed(10.0f), _offset{0},_boxSize{0}, _selectedDiffuse{0}, isDiffuseMax(false), isNormalMax(false), _currentNormal(0), _currentDiffuse(0)
	, _isSound3DEditor(false), _isLoopSoundEditor(false), _controlSoundInfo{}, _newSoundName{0}, _isNewSoundVol(0), _setTargetPosition{0}, _velocityParticle(0), _randomParticle(false), _fadeoutTimeParticle(0), _lifeTimeParticle(0), _colorParticle{1.0f, 1.0f, 1.0f}, _directionParticle{0}, _sizeParticle{0}, _isInvisible(1.0f)
	, _rotationParticle{ 0 }, _AngleParticle{0}, _isParticleCamera(false), _isActiveParticle(false)
{ 
																			
}																			

EpicTool::InspectorWindow::~InspectorWindow()
{

}

template<typename ComponentType>
void EpicTool::InspectorWindow::ComboControl(std::vector<std::string> list, std::vector<std::string> listEditor, int& selected, const char* name, ComponentType* instance)
{
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Parameter is incorrect");
}


template<>
void EpicTool::InspectorWindow::ComboControl<KunrealEngine::MeshRenderer>(std::vector<std::string> list, std::vector<std::string> listEditor, int& selected, const char* name, KunrealEngine::MeshRenderer* instance)
{

	if (_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetMeshStatus())
	{
		std::string listStr;
		if (list == _meshList)  // ���⼭ �Ǵ��ؼ� �б��Ұ�
		{
			listStr = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetMeshName();
			StringRemove(listStr, _meshStringToRemove);
		}
	}

	if (ImGui::Combo(name, &selected, [](void* data, int idx, const char** out_text)
		{
			auto& items = *static_cast<std::vector<std::string>*>(data);
			if (idx < 0 || idx >= static_cast<int>(items.size()))
				return false;
			*out_text = items[idx].c_str();
			return true;
		},
		static_cast<void*>(&listEditor), static_cast<int>(listEditor.size())))
	{
		if (list == _meshList)  // ���⼭ �Ǵ��ؼ� �б��Ұ�
		{
			SetMeshObjectEditor(list, selected);
			_comboMeshSelect = -1;
		}
		if (_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetMeshStatus())
		{
			if (list == _textureList && name == "Normal")
			{
				_DebugType = DebugType::AddNormal;
				if (_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetNormalName() != list[selected]
					&& _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetNormalName() != "")
				{
					_DebugType = DebugType::ChangeNormal;
				}

				_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->
					SetNormalTexture(_currentNormal, list[selected].c_str());
				
				_comboNormalSelect = -1;
			}

			if (list == _textureList && name == "Diffuse")
			{
				_DebugType = DebugType::AddDiffuse;
				if (_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetTextureName() != list[selected]
					&& _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetTextureName() != "")
				{
					_DebugType = DebugType::ChangeDiffuse;
				}

				_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->
					SetDiffuseTexture(_currentDiffuse, list[selected].c_str());
				
				_comboDiffuseSelect = -1;
			}

		}
	}


	
}

template<>
void EpicTool::InspectorWindow::ComboControl<KunrealEngine::Light>(std::vector<std::string> list, std::vector<std::string> listEditor, int& selected, const char* name, KunrealEngine::Light* instace)
{

	if (_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetLightStatus())
	{
		if (_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetLightType() == KunrealEngine::LightType::DirectionalLight)
		{
			ImGui::Text("%s", list[KunrealEngine::LightType::DirectionalLight].c_str());
		}
		else if (_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetLightType() == KunrealEngine::LightType::PointLight)
		{
			ImGui::Text("%s", list[KunrealEngine::LightType::PointLight].c_str());
		}
	}


	if (ImGui::Combo(name, &selected, [](void* data, int idx, const char** out_text)
		{
			auto& items = *static_cast<std::vector<std::string>*>(data);
			if (idx < 0 || idx >= static_cast<int>(items.size()))
				return false;
			*out_text = items[idx].c_str();
			return true;
		},
		static_cast<void*>(&listEditor), static_cast<int>(listEditor.size())))
	{
		if (list[selected] == _directionalLight)
		{
			_DebugType = DebugType::AddDirectional;
			_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->CreateDirectionalLight();
		}

		if (list[selected] == _pointLight)
		{
			_DebugType = DebugType::AddPoint;
			_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->CreatePointLight();
		}
		_selectObjectName = _gameObjectlist[_selectedObjectIndex]->GetObjectName();
	}
}


template<>
void EpicTool::InspectorWindow::ComboControl<KunrealEngine::ImageRenderer>(std::vector<std::string> list, std::vector<std::string> listEditor, int& selected, const char* name, KunrealEngine::ImageRenderer* instace)
{
	if (selected == -1)
	{
		//if (_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::ImageRenderer>()->GetImageStatus())
		//{  // �̹����� �����ִ��� Ȯ��
			ImGui::Text("None"); // �����ִ� �̹����� ���� �� �� ����
		//}
	}
	else
	{
		ImGui::Text("%s", listEditor[selected].c_str());
	}
	

	if (ImGui::Combo(name, &selected, [](void* data, int idx, const char** out_text)
		{
			auto& items = *static_cast<std::vector<std::string>*>(data);
			if (idx < 0 || idx >= static_cast<int>(items.size()))
				return false;
			*out_text = items[idx].c_str();
			return true;
		},
		static_cast<void*>(&listEditor), static_cast<int>(listEditor.size())))
	{
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::ImageRenderer>()->SetImage(listEditor[selected]);
	}

}

template<>
void EpicTool::InspectorWindow::ComboControl<KunrealEngine::SoundPlayer>(std::vector<std::string> list, std::vector<std::string> listEditor, int& selected, const char* name, KunrealEngine::SoundPlayer* instace)
{
	if (ImGui::Combo(name, &selected, [](void* data, int idx, const char** out_text)
		{
			auto& items = *static_cast<std::vector<std::string>*>(data);
			if (idx < 0 || idx >= static_cast<int>(items.size()))
				return false;
			*out_text = items[idx].c_str();
			return true;
		},
		static_cast<void*>(&listEditor), static_cast<int>(listEditor.size())))
	{
		// ����Ʈ ���°� �ѱ� ����

		_newSoundName = list[selected];
	}

}



void EpicTool::InspectorWindow::ComboControlSound(std::vector<KunrealEngine::SoundPlayer::SoundInfo> list, std::vector<KunrealEngine::SoundPlayer::SoundInfo> listEditor, int& selected, const char* name)
{
	if (ImGui::Combo(name, &selected, [](void* data, int idx, const char** out_text)
		{
			auto& items = *static_cast<std::vector<KunrealEngine::SoundPlayer::SoundInfo>*>(data);
			if (idx < 0 || idx >= static_cast<int>(items.size()))
				return false;
			*out_text = items[idx]._soundPath.c_str();
			return true;
		},
		static_cast<void*>(&listEditor), static_cast<int>(listEditor.size())))
	{
		_controlSoundInfo = listEditor[selected];
	}

	ImGui::Spacing();

	if (selected != -1)
	{
		SetSoundInfo(_controlSoundInfo);
	}

	ImGui::Spacing();


	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SoundCreate");

	SoundCreate();
}

//
// bool _is3DSound;				// 3D ��������					// �⺻�� false
//bool _isLoop;					// �ݺ��Ǵ� ��������			// �⺻�� false
//int _volume;
// unsigned int _innerIndex;
//

void EpicTool::InspectorWindow::SetSoundInfo(KunrealEngine::SoundPlayer::SoundInfo& instance)
{
	// ���� ���� ���� ��������

	if (instance._is3DSound == true)
	{
		_isSound3DEditor = true;
	}
	else if(instance._is3DSound == false)
	{
		_isSound3DEditor = false;
	}

	if (instance._isLoop == true)
	{
		_isLoopSoundEditor = true;
	}
	else if (instance._isLoop == false)
	{
		_isLoopSoundEditor = false;
	}
	
	_soundVolEditor = instance._volume;


	if (ImGui::Checkbox("3D Sound", &_isSound3DEditor))
	{
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::SoundPlayer>()->Change3Dmode(instance._innerIndex);
		instance._is3DSound = _isSound3DEditor;
	}

	if (ImGui::Checkbox("Is Loop", &_isLoopSoundEditor))
	{
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::SoundPlayer>()->ChangeisLoop(instance._innerIndex);
		instance._isLoop = _isLoopSoundEditor;
	}

	if (ImGui::SliderInt("Sound Vol", &_soundVolEditor, 0.0f, 100.0f))
	{
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::SoundPlayer>()->SetVolume(_soundVolEditor, instance._innerIndex);
	}   


}

void EpicTool::InspectorWindow::SoundCreate()
{
	ComboControl(_isNewSoundPathList, _isNewSoundPathList, _comboNewSoundSelect, "New Sound Name", _soundPlayer);

	ImGui::Checkbox("is 3D", &_isNewSound3D);

	ImGui::Checkbox("is Loop", & _isNewSoundLoop);

	ImGui::DragInt("Volume", &_isNewSoundVol, 0, 100);


	if (ImGui::Button("Make Sound"))
	{
		std::string directory = "Resources/Sound/";
		_newSoundName = directory + _newSoundName;

		int soundInfoval = 0;
		soundInfoval = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::SoundPlayer>()->CreateSoundInfo(_newSoundName, _isNewSound3D, _isNewSoundLoop, _isNewSoundVol);
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::SoundPlayer>()->CreateSound(soundInfoval, 1);

		_isNewSoundPathList = KunrealEngine::SoundSystem::GetInstance().GetSoundPathList();
		
	}
}

template<typename PieceType>
void EpicTool::InspectorWindow::DrawComponentPiece(PieceType& data, const std::string name)
{
    const int numberDataType = sizeof(PieceType) / sizeof(int);

    const char* cName = name.c_str();

    // int

    if constexpr (std::is_same_v<PieceType, int>)
    {
        ImGui::DragInt(cName, data);
    }
    else if constexpr (std::is_same_v<PieceType, int[numberDataType]>)
    {
        switch (numberDataType)
        {
            case 1:
                ImGui::DragInt(cName, data);
                break;
            case 2:
                ImGui::DragInt2(cName, data);
                break;
            case 3:
                ImGui::DragInt3(cName, data);
                break;
            default:
                break;
        }
    }
    else if constexpr (std::is_same_v< PieceType, float>)
    {
        ImGui::DragFloat(cName, data);
    }
    // float �迭���� ��� ���� ����
    else if constexpr (std::is_same_v<PieceType, float[numberDataType]>)
    {
        switch (numberDataType)
        {
            case 1:
                ImGui::DragFloat(cName, data);
                break;
            case 2:
                ImGui::DragFloat2(cName, data);
                break;
            case 3:
                ImGui::DragFloat3(cName, data);
				break;
			case 4:
				ImGui::DragFloat4(cName, data);
                break;
            default:
                break;
        }
    }
    // double
    else if constexpr (std::is_same_v<PieceType, double>)
    {
        ImGui::DragFloat(cName, data);
    }
    else if constexpr (std::is_same_v<PieceType, double[numberDataType]>)
    {
        switch (numberDataType)
        {
            case 1:
                ImGui::DragFloat(cName, data);
                break;
            case 2:
                ImGui::DragFloat2(cName, data);
                break;
            case 3:
                ImGui::DragFloat3(cName, data);
                break;
            default:
                break;
        }
    }

    else if constexpr (std::is_same_v<PieceType, std::vector>)
    {
        ImGui::InputDouble(cName, &data.x, 0.001f, 0.0f, "%.3f");
        ImGui::SameLine();
        ImGui::InputDouble(cName, &data.y, 0.001f, 0.0f, "%.3f");
        ImGui::SameLine();
        ImGui::InputDouble(cName, &data.z, 0.001f, 0.0f, "%.3f");
        ImGui::SameLine();
    }

	else if constexpr (std::is_same_v<PieceType, std::vector<float>>)
	{
		switch (data.size())
		{
		case 1:
			ImGui::DragFloat(cName, data);
			break;
		case 2:
			ImGui::DragFloat2(cName, data);
			break;
		case 3:
			ImGui::DragFloat3(cName, data);
			break;
		case 4:
			ImGui::DragFloat4(cName, data);
			break;
		default:
			break;
		}
	}

}


template<>
void EpicTool::InspectorWindow::DrawComponentPiece<bool>(bool& instance, const std::string name)
{
    //�̸��� �������� �κ�
    ImGui::Spacing();

    ImGui::Text("%s", name.c_str());

	ImGui::SameLine();

    ImGui::Checkbox(" ", &instance);

	ImGui::Spacing();
}

template<typename ComponentType>
void EpicTool::InspectorWindow::DrawComponentInfo(ComponentType* instance)
{

}

void EpicTool::InspectorWindow::DeleteComponent(KunrealEngine::Component* instance)
{
    if (ImGui::Button("DeleteCompoent"))
    {
		_DebugType = DebugType::DeleteComponentDebug;

		_deleteComponentName = instance->GetComponentName();
        _gameObjectlist[_selectedObjectIndex]->DeleteComponent(instance);
        _compoenetList = _gameObjectlist[_selectedObjectIndex]->GetComponentList();
	}
	//if (ImGui::Button("Cube"))
	//{
		//KunrealEngine::GraphicsSystem::GetInstance().CreateCubeMap("���� �����ϰ� ���� �̸�", "���");
		//KunrealEngine::GraphicsSystem::GetInstance().DeleteCubeMap("���� �����ϰ� ���� �̸�");
		//KunrealEngine::GraphicsSystem::GetInstance().SetMainCubeMap("���� �����ϰ� ���� �̸�");
		//KunrealEngine::GraphicsSystem::GetInstance().GetCubeMapList();		// std::vector<std::string> �̰ɷ� ��ȯ
	//}
}

/// <summary>
/// Transform�� �����ϴ� UI
/// </summary>
/// <param name="instance"></param>
template<> // ���� ����Ʈ�� �����ϸ� �ش� �κ��� ���ؼ� ����ϰ� �ɰ��̴�  // ������Ʈ ����Ʈ
void EpicTool::InspectorWindow::DrawComponentInfo<KunrealEngine::Transform>(KunrealEngine::Transform* instance)
{  
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Transform");

	float _positionArray[3];
	float _rotationArray[3];
	float _scaleArray[3];


	_positionArray[0] = instance->GetPosition().x;
	_positionArray[1] = instance->GetPosition().y;
	_positionArray[2] = instance->GetPosition().z;

	DrawComponentPiece(_positionArray, "Position");
	instance->SetPosition(_positionArray[0], _positionArray[1], _positionArray[2]);


	_rotationArray[0] = instance->GetRotation().x;
	_rotationArray[1] = instance->GetRotation().y;
	_rotationArray[2] = instance->GetRotation().z;

	DrawComponentPiece(_rotationArray, "Rotation");

	//DirectX::XMVECTOR rotationPitchYaw = DirectX::XMQuaternionRotationRollPitchYaw(DirectX::XMConvertToRadians(_rotationArray[0]), DirectX::XMConvertToRadians(_rotationArray[1]), DirectX::XMConvertToRadians(_rotationArray[2]));
	//DirectX::XMStoreFloat4(&_quaternion, rotationPitchYaw);
	instance->SetRotation(_rotationArray[0], _rotationArray[1], _rotationArray[2]);


    //instance->SetQuaternion(_quaternion);

	_scaleArray[0] = instance->GetScale().x;
	_scaleArray[1] = instance->GetScale().y;
	_scaleArray[2] = instance->GetScale().z;

	if (ImGui::DragFloat3("totalScale", _scaleArray, 0.01f))  // �ѹ��� �����ϱ� ���� ����, �� ������ �����̶��� �� ������ �ȵ����� ����� ����
	{
		instance->SetScale(_scaleArray[0], _scaleArray[1], _scaleArray[2]);
	}
	
	float totalScale;
	totalScale = instance->GetScale().x;

	if (ImGui::DragFloat("totalScale", &totalScale, 0.01f))  // �ѹ��� �����ϱ� ���� ����, �� ������ �����̶��� �� ������ �ȵ����� ����� ����
	{
		instance->SetScale(totalScale, totalScale, totalScale);
	}

}

/// <summary>
/// Camera�� �����ϴ� UI
/// </summary>
/// <param name="instance"></param>
template<>
void EpicTool::InspectorWindow::DrawComponentInfo<KunrealEngine::Camera>(KunrealEngine::Camera* instance)
{
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Camera");

	if (_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Camera>() == NULL) // ���� �߰�
	{
		_gameObjectlist[_selectedObjectIndex]->AddComponent<KunrealEngine::Camera>();
	}

	_setTargetPosition[0] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Camera>()->GetTargetPosition().x;
	_setTargetPosition[1] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Camera>()->GetTargetPosition().y;
	_setTargetPosition[2] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Camera>()->GetTargetPosition().z;

	if (ImGui::DragFloat3("SetTargetPosition", _setTargetPosition))
	{
		instance->SetTargetPosition(_setTargetPosition[0], _setTargetPosition[1], _setTargetPosition[2]);
	}

	_isCameraActive = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Camera>()->GetActivated();

	if(ImGui::Checkbox("SetActiveCamera", &_isCameraActive))
	{
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Camera>()->SetActive(_isCameraActive);
	}

	if (ImGui::Button("SetMainCamera"))
	{
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Camera>()->SetMainCamera();
	}

	DrawComponentPiece<bool>(_cameraFix, "FixCamera");
   
	if (_cameraFix == true)
	{
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Camera>()->FixCamera();
	}
	else if (_cameraFix == false)
	{
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Camera>()->UnfixCamera();
	}

    DeleteComponent(_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Camera>());

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

}

/// <summary>
/// Mesh�� �����ϴ� UI
/// </summary>
/// ���⼭ Animator�� ������
/// <param name="instance"></param>
template<>
void EpicTool::InspectorWindow::DrawComponentInfo<KunrealEngine::MeshRenderer>(KunrealEngine::MeshRenderer* instance)
{
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f),"MeshRenderer");

    bool meshState = false;

	if (_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>() == NULL) // ���� �߰�
	{
	    _gameObjectlist[_selectedObjectIndex]->AddComponent<KunrealEngine::MeshRenderer>();
	}

	if (_selectObjectName != _gameObjectlist[_selectedObjectIndex]->GetObjectName())
	{
		_comboMeshSelect = -1;

		_comboNormalSelect = -1;

		_comboDiffuseSelect = -1;
	}

	bool isMeshRenderActive = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetActivated();

	if (ImGui::Checkbox("SetActiveMesh", &isMeshRenderActive))
	{
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->SetActive(isMeshRenderActive);
	}

	bool isSetPisckable = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetPickableState();

	if (ImGui::Checkbox("SetPisckable", &isSetPisckable))
	{
		if (_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetMeshStatus())
		{
			_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->SetPickableState(isSetPisckable);
		}
	}

	if (_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetMeshStatus())
	{
		bool isCartoonState = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetCartoonState();

		if (ImGui::Checkbox("SetCartoonRender", &isCartoonState))
		{
			_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->SetCartoonState(isCartoonState);
		}

		bool isShadowState = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetShadowState();

		if (ImGui::Checkbox("SetShadow", &isShadowState))
		{
			_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->SetShadowState(isShadowState);
		}
	}
    ComboControl(_meshList, _meshListEditor, _comboMeshSelect, "Mesh", instance);

	ImGui::Spacing();

    ComboControl(_textureList, _TextureListEditor, _comboNormalSelect, "Diffuse", instance);
    
	if (_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetMeshStatus())
	{

		ImGui::Spacing();

		std::vector<std::string> diffuses = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetTextures();

		std::vector<const char*> itemsDiffuse(diffuses.size());

		for (size_t i = 0; i < diffuses.size(); ++i)
		{
			itemsDiffuse[i] = diffuses[i].c_str();
		}

		ImGui::ListBox("DiffuseList", &_currentDiffuse, itemsDiffuse.data(), itemsDiffuse.size(), 4);
	}

	ImGui::Spacing();

	ComboControl(_textureList, _TextureListEditor, _comboDiffuseSelect, "Normal", instance);


	if (_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetMeshStatus())
	{
		ImGui::Spacing();

		std::vector<std::string> normals = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetNormals();
		std::vector<const char*> itemsNor(normals.size());
		for (size_t i = 0; i < normals.size(); ++i)
		{
			itemsNor[i] = normals[i].c_str();
		}

		ImGui::ListBox("NormalList2", &_currentNormal, itemsNor.data(), itemsNor.size(), 4);
	}


	ImGui::Spacing();

	if (_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetMeshStatus() != false)
	{
    	_isInvisible = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetAlpha();
	}

	if (ImGui::DragFloat("Transparency", &_isInvisible, 0.0f, 0.0f, 1.0f))
	{

		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->SetAlpha(_isInvisible);
	}

	ImGui::Spacing();

	ImGui::Text("Animator");

	if (ImGui::Button("Stop"))
	{
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Animator>()->Stop();
	}
	ImGui::SameLine();
	if (ImGui::Button("Pause"))
	{
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Animator>()->Pause();
	}
	ImGui::SameLine();
	if (ImGui::Button("Replay"))
	{
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Animator>()->Replay();
	}

	ImGui::Spacing();

	if (ImGui::Button("ResetList"))
	{
		_meshList = KunrealEngine::GraphicsSystem::GetInstance().GetRenderableList();

		_textureList = KunrealEngine::GraphicsSystem::GetInstance().GetTextureList();

		_textureList.insert(_textureList.begin(), "None");

		_comboMeshSelect = -1;

		_comboNormalSelect = -1;

		_comboDiffuseSelect = -1;

		_meshListEditor.clear();
		_TextureListEditor.clear();

		ListToRemove(_meshList, _meshListEditor, _meshStringToRemove);

		ListToRemove(_textureList, _TextureListEditor, _textureStringToRemove);
	}

	DeleteComponent(_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>());

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
}

/// <summary>
/// Light�� �����ϴ� UI
/// </summary>
/// <param name="instance"></param>
template<>
void EpicTool::InspectorWindow::DrawComponentInfo<KunrealEngine::Light>(KunrealEngine::Light* instance)
{
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Light");

	if (_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>() == NULL) // ���� �߰�
	{
		_gameObjectlist[_selectedObjectIndex]->AddComponent<KunrealEngine::Light>();
	}

	_isLightActive = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetActivated();

	if (ImGui::Checkbox("SetActiveLight", &_isLightActive))
	{
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->SetActive(_isLightActive);
	}

	ComboControl(_lightList, _lightList, _comboLightSelect, "Light", instance);

	if (_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetLightStatus()) // ���� ����� �ƴ�
	{																																					// �ش� �ڵ�� �ӽ÷�, �����ʱ�ȭ �Լ����� �����ڵ带 �ۼ������� ���Ǵ� �κ��̴�
		_ambient[0] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetAmbient().x;
		_ambient[1] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetAmbient().y;
		_ambient[2] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetAmbient().z;
		_ambient[3] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetAmbient().w;
		
		_diffuse[0] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetDiffuse().x;
		_diffuse[1] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetDiffuse().y;
		_diffuse[2] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetDiffuse().z;
		_diffuse[3] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetDiffuse().w;
		
		_specular[0] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetSpecular().x;
		_specular[1] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetSpecular().y;
		_specular[2] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetSpecular().z;
		_specular[3] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetSpecular().w;

		_direction[0] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetDirection().x;
		_direction[1] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetDirection().y;
		_direction[2] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetDirection().z;	

		_pointAmbient[0] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetAmbient().x;
		_pointAmbient[1] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetAmbient().y;
		_pointAmbient[2] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetAmbient().z;
		_pointAmbient[3] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetAmbient().w;

		_pointDiffuse[0] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetDiffuse().x;
		_pointDiffuse[1] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetDiffuse().y;
		_pointDiffuse[2] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetDiffuse().z;
		_pointDiffuse[3] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetDiffuse().w;

		_pointSpecular[0] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetSpecular().x;
		_pointSpecular[1] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetSpecular().y;
		_pointSpecular[2] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetSpecular().z;
		_pointSpecular[3] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetSpecular().w;

		_lightOffset[0] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetOffSet().x;
		_lightOffset[1] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetOffSet().y;
		_lightOffset[2] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetOffSet().z;
	}



	if (_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetLightType() == KunrealEngine::DirectionalLight)
	{		
		ImGui::DragFloat4("Ambient" , _ambient, 0.0f, 0.0f , 1.0f);
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->SetAmbient(_ambient[0], _ambient[1], _ambient[2], _ambient[3]);
		
		ImGui::DragFloat4("Diffuse", _diffuse,  0.0f, 0.0f, 1.0f);
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->SetDiffuse(_diffuse[0], _diffuse[1], _diffuse[2], _diffuse[3]);
		
		ImGui::DragFloat4( "Specular", _specular, 0.0f, 0.0f, 1.0f);
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->SetSpecular(_specular[0], _specular[1], _specular[2], _specular[3]);
		
		ImGui::DragFloat3("Direction", _direction, 0.0f, -1.0f, 1.0f);
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->SetDirection(_direction[0], _direction[1], _direction[2]);
	}

	if (_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetLightType() == KunrealEngine::PointLight)
	{
		ImGui::DragFloat4("Ambient", _pointAmbient, 0.0f, 0.0f, 1.0f);
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->SetAmbient(_pointAmbient[0], _pointAmbient[1], _pointAmbient[2], _pointAmbient[3]);

		ImGui::DragFloat4("Diffuse", _pointDiffuse, 0.0f, 0.0f, 1.0f);
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->SetDiffuse(_pointDiffuse[0], _pointDiffuse[1], _pointDiffuse[2], _pointDiffuse[3]);

		ImGui::DragFloat4("Specular", _pointSpecular, 0.0f, 0.0f, 1.0f);
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->SetSpecular(_pointSpecular[0], _pointSpecular[1], _pointSpecular[2], _pointSpecular[3]);

		// ���⿡ ���� range�� �޾ƿ��� �ڵ� �ʿ�

		_pointRange = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->GetPointRange();

		ImGui::DragFloat("Range", &_pointRange, 1.0f, 0.0f);
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->SetPointRange(_pointRange);

		ImGui::DragFloat3("LightOffset", _lightOffset, 1.0f);
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>()->SetOffSet(_lightOffset[0], _lightOffset[1], _lightOffset[2]);
	}


	DeleteComponent(_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>());

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
}

/// <summary>
/// ImageRenderer�� �����ϴ� UI
/// </summary>
/// <param name="instance"></param>
template<>
void EpicTool::InspectorWindow::DrawComponentInfo<KunrealEngine::ImageRenderer>(KunrealEngine::ImageRenderer* instance)
{
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "ImageRenderer");

	if (_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::ImageRenderer>() == NULL) // ���� �߰�
	{
		_gameObjectlist[_selectedObjectIndex]->AddComponent<KunrealEngine::ImageRenderer>();
	}

	_isImageActive = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::ImageRenderer>()->GetImageStatus();

	if (ImGui::Checkbox("SetActiveImage", &_isImageActive))
	{
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::ImageRenderer>()->SetImageStatus(_isImageActive);
	}


	ComboControl(_textureList, _TextureListEditor, _comboImageSelect, "Image", instance);

	DeleteComponent(_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::ImageRenderer>());

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
}

/// <summary>
/// SoundPlayer�� �����ϴ� UI
/// </summary>
/// <param name="instance"></param>
template<>
void EpicTool::InspectorWindow::DrawComponentInfo<KunrealEngine::SoundPlayer>(KunrealEngine::SoundPlayer* instance)
{

	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SoundPlayer");

	// �߻�Ŭ������ �ν��Ͻ�ȭ �� �� �����ϴ� => ���� �����󿡼� ������Ʈ�� �ƴѵ�
	if (_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::SoundPlayer>() == NULL) // ���� �߰�
	{
		_gameObjectlist[_selectedObjectIndex]->AddComponent<KunrealEngine::SoundPlayer>();
	}

	ComboControlSound(_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::SoundPlayer>()->_soundList
		, _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::SoundPlayer>()->_soundList,
		_comboSoundSelect, "SoundInfo");


	DeleteComponent(_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::SoundPlayer>());

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

}

/// <summary>
/// Collider�� �����ϴ� UI
/// </summary>
/// <param name="instance"></param>
template<>
void EpicTool::InspectorWindow::DrawComponentInfo<KunrealEngine::BoxCollider>(KunrealEngine::BoxCollider* instance)
{
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "BoxCollider");

	bool isComponent = false;
	bool isNewSetting = false;

	if (_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::BoxCollider>() == NULL) // ���� �߰�
	{
		_gameObjectlist[_selectedObjectIndex]->AddComponent<KunrealEngine::BoxCollider>();
		isNewSetting = true;
	}

	if (isNewSetting != true)
	{
		_offset[0] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::BoxCollider>()->GetOffset().x;
		_offset[1] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::BoxCollider>()->GetOffset().y;
		_offset[2] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::BoxCollider>()->GetOffset().z;

		_boxSize[0] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::BoxCollider>()->GetColliderScale().x;
		_boxSize[1] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::BoxCollider>()->GetColliderScale().y;
		_boxSize[2] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::BoxCollider>()->GetColliderScale().z;
	}

	if(ImGui::DragFloat3("Offset", _offset))
	{
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::BoxCollider>()->SetOffset(_offset[0], _offset[1], _offset[2]);
	}

	if(ImGui::DragFloat3("BoxSize", _boxSize))
	{
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::BoxCollider>()->SetColliderScale(_boxSize[0], _boxSize[1], _boxSize[2]);
	}

	DeleteComponent(_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::BoxCollider>());

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
}

/// <summary>
/// Particle�� �����ϴ� UI
/// </summary>
/// <param name="instance"></param>
template<>
void EpicTool::InspectorWindow::DrawComponentInfo<KunrealEngine::Particle>(KunrealEngine::Particle* instance)
{
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Particle");

	bool isNewSetting = false;

	if (_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>() == NULL) // ���� �߰�
	{
		_gameObjectlist[_selectedObjectIndex]->AddComponent<KunrealEngine::Particle>();
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->SetParticleEffect("Fire", "Resources/Textures/Particles/flare.dds", 1000); 
		isNewSetting = true;
	}

	if (isNewSetting != true)
	{
		_velocityParticle = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetVelocity();
		_randomParticle = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetRandomState();
		_fadeoutTimeParticle = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetFadeOutTime();
		_lifeTimeParticle = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetLifeTime();

		_colorParticle[0] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetColor().x;
		_colorParticle[1] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetColor().y;
		_colorParticle[2] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetColor().z;

		_directionParticle[0] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetDirection().x;
		_directionParticle[1] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetDirection().y;
		_directionParticle[2] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetDirection().z;

		_sizeParticle[0] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetSize().x;
		_sizeParticle[1] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetSize().y;

		_rotationParticle[0] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetRotation().x;
		_rotationParticle[1] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetRotation().y;
		_rotationParticle[2] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetRotation().z;

		_AngleParticle[0] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetParticleAngle().x;
		_AngleParticle[1] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetParticleAngle().y;
		_AngleParticle[2] = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetParticleAngle().z;

		_isParticleCamera = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetParticleCameraApply();
		_isActiveParticle = _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->GetActivated();
	}

	if (ImGui::Checkbox("ParticleActive", &_isActiveParticle))
	{
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->SetActive(_isActiveParticle);
	}

	// ��ƼŬ ����Ʈ?

	if (ImGui::DragFloat("Velocity", &_velocityParticle, 0.1f, 0.0f, FLT_MAX))
	{
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->SetParticleVelocity(_velocityParticle, _randomParticle);
	}

	if (ImGui::Checkbox("Random", &_randomParticle))
	{
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->SetParticleVelocity(_velocityParticle, _randomParticle);

	}

	if (ImGui::DragFloat("FadeoutTime", &_fadeoutTimeParticle, 0.1f, 0.0f, FLT_MAX))
	{
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->SetParticleDuration(_fadeoutTimeParticle, _lifeTimeParticle);
	}

	if (ImGui::DragFloat("LifeTimeParticle", &_lifeTimeParticle, 0.1f, 0.0f, FLT_MAX))
	{
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->SetParticleDuration(_fadeoutTimeParticle, _lifeTimeParticle);

	}

	if (ImGui::DragFloat3("Color", _colorParticle, 0.1f, 0.0f, FLT_MAX))
	{
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->AddParticleColor(_colorParticle[0], _colorParticle[1], _colorParticle[2]);
	}

	if (ImGui::DragFloat3("Direction", _directionParticle, 0.1f))
	{
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->SetParticleDirection(_directionParticle[0], _directionParticle[1], _directionParticle[2]);
	}

	if (ImGui::DragFloat2("Size", _sizeParticle, 0.1f, 0.0f, FLT_MAX))
	{
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->SetParticleSize(_sizeParticle[0], _sizeParticle[1]);

	}

	if (ImGui::DragFloat3("RotationParticle", _rotationParticle, 1.f, 1.f, FLT_MAX))
	{
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->SetParticleRotation(_rotationParticle[0], _rotationParticle[1], _rotationParticle[2]);;
	}

	if (ImGui::DragFloat3("ParticleAngle", _AngleParticle, 1.f, FLT_MIN, FLT_MAX))
	{
		DirectX::XMFLOAT3 angle;
		angle.x = _AngleParticle[0];
		angle.y = _AngleParticle[1];
		angle.z = _AngleParticle[2];

		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->SetParticleAngle(angle);
	}
	
	if (ImGui::Checkbox("isParticleCamera", &_isParticleCamera))
	{
		_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>()->SetParticleCameraApply(_isParticleCamera);
	}


	DeleteComponent(_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>());

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
}

/// <summary>
/// Player�� �����ϴ� UI
/// </summary>
/// <param name="instance"></param>
template<>
void EpicTool::InspectorWindow::DrawComponentInfo<KunrealEngine::Player>(KunrealEngine::Player* instance)
{
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Player");

	if (_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Player>() == NULL) // ���� �߰�
	{
		_gameObjectlist[_selectedObjectIndex]->AddComponent<KunrealEngine::Player>();
	}


	DeleteComponent(_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Player>());

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
}

/// <summary>
/// Boss[Kamen]�� �����ϴ� UI
/// </summary>
/// <param name="instance"></param>
template<>
void EpicTool::InspectorWindow::DrawComponentInfo<KunrealEngine::Kamen>(KunrealEngine::Kamen* instance)
{
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Kamen");

	if (_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Kamen>() == NULL) // ���� �߰�
	{
		_gameObjectlist[_selectedObjectIndex]->AddComponent<KunrealEngine::Kamen>();
	}

	DeleteComponent(_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Kamen>());

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
}

void EpicTool::InspectorWindow::Initialize()
{
    _meshList = KunrealEngine::GraphicsSystem::GetInstance().GetRenderableList();

    _textureList = KunrealEngine::GraphicsSystem::GetInstance().GetTextureList();

	_isNewSoundPathList = KunrealEngine::SoundSystem::GetInstance().GetSoundPathList();

	_debugWindow = new DebugWindow;

	_textureList.insert(_textureList.begin(), "None");

	_isPickedObject = false;

    _lightList.push_back("DirectionalLight");
    _lightList.push_back("PointLight");

    ListToRemove(_meshList, _meshListEditor, _meshStringToRemove);

    ListToRemove(_textureList, _TextureListEditor, _textureStringToRemove);
}

/// <summary>
/// ���� â�� ������
/// </summary>
/// <param name="selectedObjectIndex"></param>
void EpicTool::InspectorWindow::ShowWindow(int& selectedObjectIndex)
{
	static bool selectedComponentIndex = false;
	bool transformOpen = false;
	bool meshStateOpen = false;
	bool cameraOpen = false;
	bool lightOpen = false;
	bool imageOpen = false;
	bool soundPlayerOpen = false;
	bool playerOpen = false;
	bool KamenOpen = false;
	bool AracneOpen = false;
	bool EntOpen = false;
	bool particleOpen = false;
	bool collider = false;
	_DebugType = DebugType::None;

	_selectedObjectIndex = selectedObjectIndex;

	if (!(IMGUIZMO_NAMESPACE::IsUsing() || IMGUIZMO_NAMESPACE::IsOver()))
	{
		if (KunrealEngine::GraphicsSystem::GetInstance().GetPickedObject() != nullptr && _isPickedObject == false) // ���϶� ����ó�� �ʿ�
		{
			int count = 0;
			for (auto gameObjList : _gameObjectlist)
			{
				if (gameObjList->GetObjectName() == KunrealEngine::GraphicsSystem::GetInstance().GetPickedObject()->GetObjectName())
				{
					selectedObjectIndex = count;
					_isPickedObject = true;
					_isPickedObjectName = KunrealEngine::GraphicsSystem::GetInstance().GetPickedObject()->GetObjectName();
				}
				
				count++;
			}
		}
		else if (KunrealEngine::GraphicsSystem::GetInstance().GetPickedObject() != nullptr && _isPickedObject == true)
		{
			if (_selectedObjectIndex != -1)
			{
				if (KunrealEngine::GraphicsSystem::GetInstance().GetPickedObject()->GetObjectName() != _isPickedObjectName)
				{
					selectedObjectIndex = -1;
					_isPickedObject = false;
					//_selectedObjectIndex = selectedObjectIndex;
				}
				else
				{
					_selectedObjectIndex = selectedObjectIndex;
				}
			}
		}
		else if (KunrealEngine::GraphicsSystem::GetInstance().GetPickedObject() == nullptr && _isPickedObject == true)
		{
			_isPickedObject = false;
			selectedObjectIndex = -1;
		}
	}



	if (_selectedObjectIndex != -1)
	{
		_compoenetList = _gameObjectlist[_selectedObjectIndex]->GetComponentList();
		SetSelectObject();

		if (selectedComponentIndex)
		{
			const char* items[] = { "MeshRenderer" , "Camera" , "Light", "ImageRenderer", "BoxCollider", "SoundPlayer","Particle", "Player", "Kamen", "Aracne", "Ent"};
			int selectedItem = -1;

			if (ImGui::Combo("Component", &selectedItem, items, 11)) {
				// ����ڰ� ���ο� �������� �������� �� ������ �ڵ�
				// �ӽ� �׽�Ʈ ���̸� �����Ұ���
				switch (selectedItem)
				{
				case 0:
					IsCheckItem(meshStateOpen);
					selectedComponentIndex = !selectedComponentIndex;
					_DebugType = DebugType::AddMeshRenderer;
					break;
				case 1:
					IsCheckItem(cameraOpen);
					selectedComponentIndex = !selectedComponentIndex;
					_DebugType = DebugType::AddCamera;
					break;
				case 2:
					IsCheckItem(lightOpen);
					selectedComponentIndex = !selectedComponentIndex;
					_DebugType = DebugType::AddLight;
					break;
				case 3:
					IsCheckItem(imageOpen);
					selectedComponentIndex = !selectedComponentIndex;
					break;
				case 4:
					IsCheckItem(collider);
					selectedComponentIndex = !selectedComponentIndex;
					break;
				case 5:
					IsCheckItem(soundPlayerOpen);
					selectedComponentIndex = !selectedComponentIndex;
					break;
				case 6:
					IsCheckItem(particleOpen);
					selectedComponentIndex = !selectedComponentIndex;
					break;
				case 7:
					IsCheckItem(playerOpen);
					selectedComponentIndex = !selectedComponentIndex;
					break;
				case 8:
					IsCheckItem(KamenOpen);
					selectedComponentIndex = !selectedComponentIndex;
					break;
				case 9:
					IsCheckItem(AracneOpen);
					selectedComponentIndex = !selectedComponentIndex;
					break;
				case 10:
					IsCheckItem(EntOpen);
					selectedComponentIndex = !selectedComponentIndex;
					break;

				default:
					break;
				}
			}
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
		}

		_compoenetList = _gameObjectlist[_selectedObjectIndex]->GetComponentList();

		for (auto componentlist : _compoenetList)
		{
			std::string checkComponentName = componentlist->GetComponentName();

			if (checkComponentName == "MeshRenderer" && checkComponentName != "Light")
			{
				meshStateOpen = true;
			}
			if (checkComponentName == "Camera" && checkComponentName != "Light")
			{
				cameraOpen = true;
			}
			if (checkComponentName == "Light")
			{
				lightOpen = true;
			}
			if (checkComponentName == "ImageRenderer")
			{
				imageOpen = true;
			}
			if (checkComponentName == "BoxCollider")
			{
				collider = true;
			}
			if (checkComponentName == "SoundPlayer")
			{
				soundPlayerOpen = true;
			}
			if (checkComponentName == "Player")
			{
				playerOpen = true;
			}
			if (checkComponentName == "Kamen")
			{
				KamenOpen = true;
			}
			if (checkComponentName == "Particle")
			{
				particleOpen = true;
			}
		}

		if (meshStateOpen == true)
		{
			auto asdfgcv = _meshRenderer;
			DrawComponentInfo(_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>());
		}

		if (cameraOpen == true)
		{
			DrawComponentInfo(_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Camera>());
		}

		if (lightOpen == true)
		{
			DrawComponentInfo(_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Light>());
		}

		if (imageOpen == true)
		{
			DrawComponentInfo(_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::ImageRenderer>());
		}

		if (collider == true)
		{
			DrawComponentInfo(_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::BoxCollider>());
		}

		if (soundPlayerOpen == true)  // ������ �ν��Ͻ��� ���ȴµ� ������ ����°��� �����Ѱ�, �����߻�Ȯ�� ����
		{
			DrawComponentInfo(_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::SoundPlayer>());
		}

		if (playerOpen == true)
		{
			DrawComponentInfo(_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Player>());
		}

		if (KamenOpen == true)
		{
			DrawComponentInfo(_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Kamen>());
		}

		if (particleOpen == true)
		{
			DrawComponentInfo(_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Particle>());
		}


		if (ImGui::Button("Add Component"))
		{
			selectedComponentIndex = !selectedComponentIndex;
		}

	}
}

void EpicTool::InspectorWindow::ShowWindow()
{

}

/// <summary>
/// �� ������Ʈ�� �⺻���� ui
/// </summary>
void EpicTool::InspectorWindow::SetSelectObject()
{
    std::string ObjectName = _gameObjectlist[_selectedObjectIndex]->GetObjectName();
    char nameBuffer[256];
    

    std::strncpy(nameBuffer, ObjectName.c_str(), sizeof(nameBuffer));

    if (ImGui::InputText("Name", nameBuffer, sizeof(ObjectName)))
    {
		if (KunrealEngine::InputSystem::GetInstance()->KeyInput(KEY::ENTER))
		{
			ObjectName = nameBuffer;
			_gameObjectlist[_selectedObjectIndex]->SetObjectName(ObjectName);
		}
    }


    _isObjectActive = _gameObjectlist[_selectedObjectIndex]->GetActivated();

	if (ImGui::Checkbox("SetActive", &_isObjectActive))
	{
		_gameObjectlist[_selectedObjectIndex]->SetActive(_isObjectActive);
	}
    
	KunrealEngine::GameObject* ac = KunrealEngine::GraphicsSystem::GetInstance().GetPickedObject();


    DrawComponentInfo(_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::Transform>());

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
}

void EpicTool::InspectorWindow::IsCheckItem(bool& Item)
{
	if (Item != true)
	{
        Item = true;
	}
	else
	{
        Item = false;
	}
}

void EpicTool::InspectorWindow::SetMeshObjectEditor(std::vector<std::string>& meshList, int selectedItem)
{
    if (_gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->GetMeshStatus())
    {
		_DebugType = DebugType::ChangeMesh;
        _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->ChangeMeshObject(meshList[selectedItem].c_str());
    }
    else
    {
		_DebugType = DebugType::AddMesh;
        _gameObjectlist[_selectedObjectIndex]->GetComponent<KunrealEngine::MeshRenderer>()->SetMeshObject(meshList[selectedItem].c_str());
    }
}

void EpicTool::InspectorWindow::GetDebugType(DebugType& instance)
{
	instance = _DebugType;
}

void EpicTool::InspectorWindow::GetDeleteComponentName(std::string& componentName)
{
	componentName = _deleteComponentName;
}

void EpicTool::InspectorWindow::SetGameObjectList(std::vector<KunrealEngine::GameObject*>& intance)
{
	_gameObjectlist = intance;
}

void EpicTool::InspectorWindow::ListToRemove(std::vector<std::string>& list, std::vector<std::string>& listEditor, const std::string stringToRemove)
{
	for (const auto& listString : list)
	{
		std::string ListStr = listString;

		size_t pos = ListStr.find(stringToRemove);

		if (pos != std::string::npos)
		{
			ListStr.erase(pos, stringToRemove.length());
		}

        listEditor.push_back(ListStr);
	}
}

void EpicTool::InspectorWindow::StringRemove(std::string& listStr, const std::string removeString)
{
	size_t pos = listStr.find(removeString);
	static int current_test = 0;
	if (pos != std::string::npos)
	{
        listStr.erase(pos, removeString.length());
		ImGui::Text("%s", listStr.c_str());
	}
	else
	{
		ImGui::Text("%s", listStr.c_str());
	}
}

//�ϴ� ListBox�� ����

void EpicTool::InspectorWindow::RenderListBoxVector(const char* label, int* current_item, std::vector<std::string>& items)
{
	if (items.empty())
	{
		return;
	}
	std::vector<const char*> items_cstr; // std::string�� const char*�� ��ȯ�� ����

	// std::vector<std::string>�� const char*�� ��ȯ
	for (const auto& item : items)
	{
		items_cstr.push_back(item.c_str());
	}

	ImGui::ListBox(label, current_item, items_cstr.data(), static_cast<int>(items_cstr.size()));
}

void EpicTool::InspectorWindow::ShowWindow(bool* p_open, std::vector<Object>& object)
{

}
