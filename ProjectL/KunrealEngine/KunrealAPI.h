#pragma once
#include "EngineCore.h"
#include "Scene.h"
#include "Component.h"
#include "GameObject.h"

#include "ComponentHeaders.h"
#include "GraphicsSystem.h"
#include "SoundSystem.h"
#include "PhysicsSystem.h"
#include "InputSystem.h"
#include "Navigation.h"

/// <summary>
/// Ư�� Ŭ���� ��ü�� Ŭ�� �Ѱ��ְ� ���� ������ �� Ŭ������ �Լ��� �� ȣ���� �ʿ䰡 ������
/// ex) sceneManager Ŭ���� ��ü�� Ŭ�� ����������� �� ��ɵ��� �ʿ�
/// 
/// �� API�� �Ѱ��ְ�, �ʿ��� �Լ� ��ü�� return �޾Ƽ� ����� �� �ְ� �����
/// 
/// �̰�ȣ
/// </summary>

namespace KunrealEngine
{
	extern "C" _DECLSPEC EngineCore* CreateEngine();
	extern "C" _DECLSPEC Scene* CreateScene(std::string sceneName);
	extern "C" _DECLSPEC Scene* GetScene(std::string sceneName);
	extern "C" _DECLSPEC Scene* GetCurrentScene();
	extern "C" _DECLSPEC void ChangeScene(std::string sceneName);
			   
	extern "C" _DECLSPEC std::vector<Scene*>* GetSceneLists();
			   
	extern "C" _DECLSPEC float GetDeltaTime();
	extern "C" _DECLSPEC float GetTimeScale();
	extern "C" _DECLSPEC void SetTimeScale(float scale);
}
