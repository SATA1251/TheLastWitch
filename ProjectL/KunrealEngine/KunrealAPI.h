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
/// 특정 클래스 자체는 클라에 넘겨주고 싶지 않은데 그 클래스의 함수는 꼭 호출할 필요가 있을때
/// ex) sceneManager 클래스 자체는 클라가 몰라야하지만 그 기능들은 필요
/// 
/// 이 API를 넘겨주고, 필요한 함수 자체만 return 받아서 사용할 수 있게 만든다
/// 
/// 이건호
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
