#include "Scene.h"
#include "GameObject.h"
#include "ComponentHeaders.h"

KunrealEngine::Scene::Scene()
	:_sceneName(""), _mainCamera(nullptr)
{

}

KunrealEngine::Scene::Scene(std::string sceneName)
	:_sceneName(sceneName), _mainCamera(nullptr)
{
	
}

KunrealEngine::Scene::~Scene()
{

}

void KunrealEngine::Scene::Initialize()
{
	if (_objectList.empty())
	{
		//scene�� �����Ǹ� ī�޶� ������Ʈ�� �������
		CreateObject("MainCamera");
		///ī�޶� �ʿ�
		//GetGameObject("MainCamera")->AddComponent<ObjectCamera>();
		//�⺻ ī�޶� ������Ʈ�� ���� ī�޶� ���� ī�޶�� ����
		SetMainCamera(GetGameObject("MainCamera"));
	}
}

void KunrealEngine::Scene::Release()
{
	for (auto objects : _objectList)
	{
		objects->Release();
		delete objects;
	}

	_objectList.clear();
	_objectList.shrink_to_fit();
}

void KunrealEngine::Scene::FixedUpdate()
{
	if (!_objectList.empty())
	{
		for (auto& objects : _objectList)
		{
			if (objects->GetActivated())
			{
				objects->FixedUpdate();
			}
		}
	}
}

void KunrealEngine::Scene::Update()
{
	if (!_objectList.empty())
	{
		for (auto& objects : _objectList)
		{
			if (objects->GetActivated())
			{
				objects->Update();
			}
		}
	}
}

void KunrealEngine::Scene::LateUpdate()
{
	if (!_objectList.empty())
	{
		for (auto& objects : _objectList)
		{
			if (objects->GetActivated())
			{
				objects->LateUpdate();
			}
		}
	}
}

void KunrealEngine::Scene::Render()
{
	/// ��ȿ������ �ڵ� �����ؾ��Ѵ�
	if (!_objectList.empty())
	{
		for (auto& objects : _objectList)
		{
			if (!objects->GetActivated())
			{
				
			}
		}
	}
}

KunrealEngine::GameObject* KunrealEngine::Scene::CreateObject()
{
	GameObject* gameObject = new GameObject();
	_objectList.emplace_back(gameObject);
	gameObject->Initialize();
	gameObject->_sceneName = this->_sceneName;
	SortObjects();

	return gameObject;
}

KunrealEngine::GameObject* KunrealEngine::Scene::CreateObject(std::string objectName)
{
	GameObject* gameObject = new GameObject(objectName);
	//gameObject->SetObjectName(objectName);
	_objectList.emplace_back(gameObject);
	gameObject->Initialize();
	gameObject->_sceneName = this->_sceneName;
	SortObjects();

	return gameObject;
}

void KunrealEngine::Scene::AddObject(GameObject* obj)
{
	GameObject* gameObject = obj;					// ������Կ�����
	_objectList.emplace_back(gameObject);
	SortObjects();
}

void KunrealEngine::Scene::DeleteGameObject(std::string objectName)
{
	auto iter = find_if(_objectList.begin(), _objectList.end(), [&](GameObject* object)
		{
			return object->GetObjectName() == objectName;
		});

	if (iter != _objectList.end())
	{
		// �ڽ� ������Ʈ�鵵 ����
		for (auto childObjects : (*iter)->_childContainer)
		{
			DeleteGameObject(childObjects);
			(*iter)->_childContainer.clear();
		}

		(*iter)->Release();							// ������Ʈ�� ����ְ�
		delete* iter;								// �����ϰ�
		_objectList.erase(iter);					// �����̳ʿ��� ���ش�
	}
	else
	{
		/// ���޹��� �̸��� ������Ʈ�� ���� ���� ǥ��
		return;
	}
}

void KunrealEngine::Scene::DeleteGameObject(GameObject* obj)
{                                                                                            
	auto iter = find(_objectList.begin(), _objectList.end(), obj);

	if (iter != _objectList.end())
	{
		// �ڽ� ������Ʈ�鵵 ����
		for (auto childObjects : (*iter)->_childContainer)
		{
			DeleteGameObject(childObjects);
			(*iter)->_childContainer.clear();
		}

		obj->Release();
		delete* iter;
		_objectList.erase(iter);
	}
	else
	{
		/// ���޹��� �̸��� ������Ʈ�� ���� ���� ǥ�� ����
		return;
	}
}

std::string KunrealEngine::Scene::GetSceneName()
{
	return this->_sceneName;
}

std::vector<KunrealEngine::GameObject*>& KunrealEngine::Scene::GetObjectList()
{
	return this->_objectList;
} 

KunrealEngine::GameObject* KunrealEngine::Scene::GetGameObject(std::string objectName)
{
	// ������Ʈ ����Ʈ���� ���޹��� �̸��� ���� ������Ʈ�� �ִ��� Ȯ��
	auto iter = find_if(_objectList.begin(), _objectList.end(), [&](GameObject* object)
		{
			return object->GetObjectName() == objectName;
		});

	if (iter != _objectList.end())
	{
		return *iter;
	}
	else
	{
		/// �ش� ������Ʈ�� ���ٴ� ǥ�� ����
		return nullptr;
	}
}

KunrealEngine::GameObject* KunrealEngine::Scene::GetObjectWithTag(std::string tag)
{
	// ������Ʈ ����Ʈ���� ���޹��� �±׸� ���� ������Ʈ�� �ִ��� Ȯ��
	auto iter = find_if(_objectList.begin(), _objectList.end(), [&](GameObject* object)
		{
			return object->GetTag() == tag;
		});

	if (iter != _objectList.end())
	{
		return *iter;
	}
	else
	{
		/// �ش� ������Ʈ�� ���ٴ� ǥ�� ����
		return nullptr;
	}
}

std::vector<KunrealEngine::GameObject*> KunrealEngine::Scene::GetObjectsWithTag(std::string tag)
{
	std::vector<GameObject*> taggedObjects;

	for (auto objects : _objectList)
	{
		if (objects->GetTag() == tag)
		{
			taggedObjects.emplace_back(objects);
		}
	}

	return taggedObjects;

}

void KunrealEngine::Scene::SetMainCamera(GameObject* obj)
{
	_mainCamera = obj;
}

KunrealEngine::GameObject* KunrealEngine::Scene::GetMainCamera()
{
	return this->_mainCamera;
}

void KunrealEngine::Scene::SortObjects()
{
	if (_objectList.size() > 0)
	{
		for (int i = 0, j = 0; i < _objectList.size(); i++)
		{
			GameObject* obj = _objectList[i];

			for (j = i - 1; j >= 0 && _objectList[i]->GetLayer() > obj->GetLayer(); j--)
			{
				_objectList[j + 1] = _objectList[j];
			}

			_objectList[j + 1] = obj;
		}
	}
}
