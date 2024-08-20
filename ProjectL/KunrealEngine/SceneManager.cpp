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
	// scene����� ��������� Main�̶�� �̸����� �ϳ� �������
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
		scenes->Release();						// ���� release�� ���ְ�
		delete scenes;							// �� ������ ����
	}

	_sceneList.clear();
	_sceneList.shrink_to_fit();
}

KunrealEngine::Scene* KunrealEngine::SceneManager::CreateScene(std::string sceneName)
{
	auto iter = std::find_if(_sceneList.begin(), _sceneList.end(), [&sceneName](KunrealEngine::Scene* scene) { return scene->GetSceneName() == sceneName; });

	if (iter != _sceneList.end())
	{
		// �ߺ��� �̸��� �ֱ� ������ �� �̸����δ� ���� �� ����
		// ���� ���ؼ� ���� �� ���ٴ� �޼����� ����ڰ� �� �� �ְ� �������
		return nullptr;
	}
	else
	{
		Scene* createdScene = new Scene(sceneName);
		_sceneList.emplace_back(createdScene);					// scene�� �����ϰ� ��Ͽ� �־���

		if (_currentScene == nullptr)
		{
			_currentScene = createdScene;						// ���� scene�� ���ٸ� ������� scene���� ����
		}

		createdScene->Initialize();								// ���� �� �ʱ�ȭ
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
	{	/// ������ ���� �޼����� ������ �� ���� �� ���� ����
		MessageBox(0, L"�ش� ���� �����ϴ� nullptr�� ��ȯ�մϴ�", 0, 0);
		return nullptr;
	}
}

void KunrealEngine::SceneManager::ChangeScene(std::string sceneName)
{
	auto iter = std::find_if(_sceneList.begin(), _sceneList.end(), [&sceneName](KunrealEngine::Scene* scene) { return scene->GetSceneName() == sceneName; });

	if (iter != _sceneList.end())
	{
		// ã���� �ִٸ� ���� scene�� ��û�� scene���� ��������
		_currentScene = *iter;
		// ���⼭ Initialize�� ���Ѵ� �ʱ�ȭ�� scene ��ȯ�� �޶�

		// �ҷ��� scene�� �ִ� ��� ������Ʈ Ȱ��ȭ
		// Ư�� ������Ʈ�� ��Ȱ��ȭ�ϰ� �ʹٸ� Object�� SetActive �Լ� ���
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

		//scene�� ��ȯ���� �� ����scene�� ���� ������Ʈ���� ��Ȱ��ȭ
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
		// �Է��� �̸��� ������ scene�� ���� ������ ���� �Ұ�
		/// �̰͵� ���� ��� ���� �ϼ��Ǹ� ���� ����
		MessageBox(0, L"�ش� ���� �����ϴ� ��ȯ�� �� �����ϴ�", 0, 0);
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

	if (iter != _sceneList.end())			// ã�°� ������
	{
		(*iter)->Release();				// ������ �͵� �������ְ�
		_sceneList.erase(iter);				// scene ��Ͽ��� �����Ѵ�
	}
	else
	{										/// ���� �ϼ��Ǹ� �ش� �޼��� ��� ����
		return;								// ã�°� ������ �ƹ��͵� ���� �ʴ´�
	}
}
