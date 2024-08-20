#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include <windows.h>

KunrealEngine::SceneManager::SceneManager()
	:_currentScene(nullptr)
{

}

KunrealEngine::SceneManager::~SceneManager()
{

}

void KunrealEngine::SceneManager::Initialize()
{
	// scene목록이 비어있으면 Main이라는 이름으로 하나 만들어줌
	if (_sceneList.empty())
	{
		CreateScene("Main");
	}
	ChangeScene("Main");
}

void KunrealEngine::SceneManager::Release()
{
	for (Scene* scenes : _sceneList)
	{
		scenes->Release();						// 먼저 release를 해주고
		delete scenes;							// 그 다음에 삭제
	}

	_sceneList.clear();
	_sceneList.shrink_to_fit();
}

KunrealEngine::Scene* KunrealEngine::SceneManager::CreateScene(std::string sceneName)
{
	auto iter = std::find_if(_sceneList.begin(), _sceneList.end(), [&sceneName](KunrealEngine::Scene* scene) { return scene->GetSceneName() == sceneName; });

	if (iter != _sceneList.end())
	{
		// 중복된 이름이 있기 때문에 이 이름으로는 만들 수 없어
		// 툴을 통해서 만들 수 없다는 메세지를 사용자가 볼 수 있게 해줘야함
		return nullptr;
	}
	else
	{
		Scene* createdScene = new Scene(sceneName);
		_sceneList.emplace_back(createdScene);					// scene을 생성하고 목록에 넣어줌

		if (_currentScene == nullptr)
		{
			_currentScene = createdScene;						// 현재 scene이 없다면 만들어준 scene으로 설정
		}

		createdScene->Initialize();								// 생성 후 초기화
		return createdScene;
	}
}

KunrealEngine::Scene* KunrealEngine::SceneManager::GetCurrentScene()
{
	if (_currentScene != nullptr)
	{
		return _currentScene;
	}
	else
	{
		return nullptr;
	}
}

KunrealEngine::Scene* KunrealEngine::SceneManager::GetScene(std::string sceneName)
{
	auto iter = find_if(_sceneList.begin(), _sceneList.end(), [&](Scene* scene) { return scene->GetSceneName() == sceneName; }
	);

	if (iter != _sceneList.end())
	{
		return *iter;
	}
	else
	{	/// 툴에서 에러 메세지를 보여줄 수 있을 때 삭제 예정
		MessageBox(0, L"해당 씬이 없습니다 nullptr를 반환합니다", 0, 0);
		return nullptr;
	}
}

void KunrealEngine::SceneManager::ChangeScene(std::string sceneName)
{
	auto iter = std::find_if(_sceneList.begin(), _sceneList.end(), [&sceneName](KunrealEngine::Scene* scene) { return scene->GetSceneName() == sceneName; });

	if (iter != _sceneList.end())
	{
		// 찾은게 있다면 현재 scene을 요청한 scene으로 변경해줌
		_currentScene = *iter;
		// 여기서 Initialize는 안한다 초기화랑 scene 전환은 달라

		// 불러온 scene에 있는 모든 오브젝트 활성화
		// 특정 오브젝트는 비활성화하고 싶다면 Object의 SetActive 함수 사용
		if (_currentScene->GetObjectList().size() > 0)
		{
			for (auto objects : _currentScene->GetObjectList())
			{
				if (objects->_autoAwake)
				{
					objects->SetActive(true);
				}
			}
		}

		//scene이 전환됐을 때 현재scene에 없는 오브젝트들은 비활성화
		for (auto scenes : _sceneList)
		{
			if (scenes != _currentScene)
			{
				for (auto objects : scenes->GetObjectList())
				{
					objects->SetActive(false);
				}
			}
		}
	}
	else
	{
		// 입력한 이름을 가지는 scene이 없기 때문에 설정 불가
		/// 이것도 툴이 어느 정도 완성되면 삭제 예정
		MessageBox(0, L"해당 씬이 없습니다 전환할 수 없습니다", 0, 0);
	}
}

std::vector<KunrealEngine::Scene*>* KunrealEngine::SceneManager::GetSceneList()
{
	return &_sceneList;
}

void KunrealEngine::SceneManager::UpdateScene(Scene* scene)
{
	scene->Update();
}

void KunrealEngine::SceneManager::FixedUpdateScene(Scene* scene)
{
	scene->FixedUpdate();
}

void KunrealEngine::SceneManager::LateUpdateScene(Scene* scene)
{
	scene->LateUpdate();
}

void KunrealEngine::SceneManager::DeleteScene(std::string sceneName)
{
	auto iter = std::find_if(_sceneList.begin(), _sceneList.end(), [&sceneName](KunrealEngine::Scene* scene) { return scene->GetSceneName() == sceneName; });

	if (iter != _sceneList.end())			// 찾는게 있으면
	{
		(*iter)->Release();				// 해제할 것들 해제해주고
		_sceneList.erase(iter);				// scene 목록에서 삭제한다
	}
	else
	{										/// 툴이 완성되면 해당 메세지 출력 예정
		return;								// 찾는게 없으면 아무것도 하지 않는다
	}
}
