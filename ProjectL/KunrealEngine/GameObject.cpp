#include "GameObject.h"
#include "Component.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ComponentHeaders.h"

KunrealEngine::GameObject::GameObject()
	:_isActivated(true), _objectName(""), _layer(0), _parent(nullptr), _transform(nullptr), _collider(nullptr), _autoAwake(false)
{
	SceneManager& smInstance = SceneManager::GetInstance();
	int objectCount = 0;
	std::string originalName = "GameObject";	// �⺻ �̸� �����
	this->_objectName = "GameObject";			// �ƹ� �̸��� ���������� �ʾ��� ��� GameObject��� �̸����� ����
	this->_originalName = "GameObject";

	for (auto object : smInstance.GetCurrentScene()->GetObjectList())
	{
		if (this->_objectName == object->GetObjectName())		// �ߺ� Ȯ��
		{
			objectCount++;			// �̸��� �ߺ��� ��� �ڿ� ��ȣ�� ���ڸ� �ٿ� �ٸ� �̸����� ����
			this->_objectName = originalName + " (" + std::to_string(objectCount) + ")";
		}
	}
}

KunrealEngine::GameObject::GameObject(std::string name)
	:_isActivated(true), _objectName(name), _layer(0), _parent(nullptr), _transform(nullptr)
{
	this->_objectName = name;
	this->_originalName = name;
	int objectCount = 0;

	for (auto object : SceneManager::GetInstance().GetCurrentScene()->GetObjectList())
	{
		if (this->_objectName == object->GetObjectName() && object != this)
		{
			objectCount++;
			this->_objectName = name + " (" + std::to_string(objectCount) + ")";
		}
	}
}

KunrealEngine::GameObject::~GameObject()
{

}

void KunrealEngine::GameObject::Initialize()
{
	// ������Ʈ�� �������ڸ��� Ʈ�������� ���� �־���
	AddComponent<Transform>();
	this->_transform = GetComponent<Transform>();
}

void KunrealEngine::GameObject::Awake()
{

}

void KunrealEngine::GameObject::OnEnable()
{

}

void KunrealEngine::GameObject::Start()
{

}

void KunrealEngine::GameObject::FixedUpdate()
{
	/// if�ȿ� �ݺ����ȿ� if��.. ��ȿ�����̴�
	/// ������Ʈ���� ���¸� üũ �� ������ �����̳ʷ� ������ �ݺ����� ������ ��� ������
	if (this->_isActivated)
	{
		for (auto& components : this->_componentContainer)
		{
			if (components->GetActivated())
			{
				components->FixedUpdate();
			}
		}
	}
}

void KunrealEngine::GameObject::Update()
{
	if (this->_isActivated)
	{
		for (auto& components : this->_componentContainer)
		{
			if (components->GetActivated())
			{
				components->Update();
			}
		}
	}
}

void KunrealEngine::GameObject::LateUpdate()
{
	if (this->_isActivated)
	{
		for (auto& components : this->_componentContainer)
		{
			if (components->GetActivated())
			{
				components->LateUpdate();
			}
		}
	}
}

void KunrealEngine::GameObject::OnDisable()
{

}

void KunrealEngine::GameObject::Release()
{
	ClearComponent();
}

std::string KunrealEngine::GameObject::GetObjectName()
{
	return this->_objectName;
}

std::string KunrealEngine::GameObject::GetObjectOriginalName()
{
	return this->_originalName;
}

void KunrealEngine::GameObject::SetObjectName(const std::string& name)
{
	this->_objectName = name;
	int objectCount = 0;

	for (auto object : SceneManager::GetInstance().GetCurrentScene()->GetObjectList())
	{
		if (this->_objectName == object->GetObjectName() && object != this)
		{
			objectCount++;
			this->_objectName = name + " (" + std::to_string(objectCount) + ")";
		}
	}

	this->_originalName = name;
}

void KunrealEngine::GameObject::SetParent(GameObject* obj)
{
	if (this->_parent == nullptr || this->_parent != obj)
	{
		this->_parent = obj;
		obj->_childContainer.emplace_back(this);

		this->_transform->RecalculateTransform();
	}
	else
	{
		return;
	}
}

void KunrealEngine::GameObject::DetachFromParent()
{
	// �θ� �� ������Ʈ���� ���踦 �����϶�� ����
	if (_parent != nullptr)
	{
		_parent->DetachChild(this);
	}
}

void KunrealEngine::GameObject::DetachChild(GameObject* child)
{
	auto iter = find(this->_childContainer.begin(), this->_childContainer.end(), child);

	if (iter != _childContainer.end())
	{
		//child->_transform->RevertToOriginal();
		this->_childContainer.erase(iter);
		child->_parent = nullptr;
	}
}

KunrealEngine::GameObject* KunrealEngine::GameObject::GetParent()
{
	if (_parent != nullptr)
	{
		return _parent;
	}
	else
	{
		return nullptr;
	}
}

bool KunrealEngine::GameObject::CheckChild()
{
	if (_childContainer.empty())
	{
		return false;
	}
	else
	{
		return true;
	}
}

std::vector<KunrealEngine::GameObject*> KunrealEngine::GameObject::GetChilds()
{
	return this->_childContainer;
}

KunrealEngine::GameObject* KunrealEngine::GameObject::GetChild(std::string name)
{
	GameObject* result = nullptr;

	for (auto child : _childContainer)
	{
		if (child->GetObjectName() == name)
		{
			result = child;
			break;
		}
	}

	return result;
}

KunrealEngine::Scene* KunrealEngine::GameObject::GetObjectScene()
{
	return SceneManager::GetInstance().GetScene(_sceneName);
}

void KunrealEngine::GameObject::MoveToScene(Scene* scene)
{
	// �ش� scene�� �� ������Ʈ �߰�
	scene->AddObject(this);

	// ���� scene���� �� ������Ʈ ����
	auto iter = find(GetObjectScene()->GetObjectList().begin(), GetObjectScene()->GetObjectList().end(), this);
	if (iter != GetObjectScene()->GetObjectList().end())
	{
		GetObjectScene()->GetObjectList().erase(iter);
	}

	this->_sceneName = scene->GetSceneName();

	// �̵��� scene�� ���� scene�� �ٸ��ٸ� ��Ȱ��ȭ	// �׷��Ƚ� �ʿ��� render ���� �ʱ� ����
	if (SceneManager::GetInstance().GetCurrentScene() != scene)
	{
		SetActive(false);
	}
}

void KunrealEngine::GameObject::MoveToScene(std::string sceneName)
{
	// �Ű������� ���� �̸��� ��ġ�ϴ� scene Ž��
	auto sceneIter = find_if(SceneManager::GetInstance().GetSceneList()->begin(), SceneManager::GetInstance().GetSceneList()->end(), [&](Scene* scene)
		{
			return scene->GetSceneName() == sceneName;
		});

	// �ִٸ�
	if (sceneIter != SceneManager::GetInstance().GetSceneList()->end())
	{
		// �ش� scene�� �� ������Ʈ �߰�
		(*sceneIter)->AddObject(this);

		// ���� scene���� �� ������Ʈ ����
		auto objectIter = find(GetObjectScene()->GetObjectList().begin(), GetObjectScene()->GetObjectList().end(), this);
		if (objectIter != GetObjectScene()->GetObjectList().end())
		{
			GetObjectScene()->GetObjectList().erase(objectIter);
		}

		this->_sceneName = sceneName;

		// �̵��� scene�� ���� scene�� �ٸ��ٸ� ��Ȱ��ȭ	// �׷��Ƚ� �ʿ��� render ���� �ʱ� ����
		if (SceneManager::GetInstance().GetCurrentScene() != (*sceneIter))
		{
			SetActive(false);
		}
	}
}

void KunrealEngine::GameObject::SetActive(bool active)
{
	this->_isActivated = active;

	// �׷��Ƚ��ʿ� Render ���� ������Ʈ�� �־ ��¿ �� ������
	// ������Ʈ�� ��Ȱ��ȭ �Ǿ� ���� ��
	if (!this->_isActivated)
	{
		// �׷��Ƚ� ���� �� ������Ʈ�� �ִ��� Ȯ��
		if (this->GetComponent<MeshRenderer>() != nullptr)
		{
			// �ִٸ� ��Ȱ��ȭ
			this->GetComponent<MeshRenderer>()->SetRenderingState(false);
		}

		if (this->GetComponent<Light>() != nullptr)
		{
			this->GetComponent<Light>()->SetLightState(false);
		}

		if (this->GetComponent<ImageRenderer>() != nullptr)
		{
			this->GetComponent<ImageRenderer>()->SetImageStatus(false);
		}

		if (this->GetComponent<Particle>() != nullptr)
		{
			this->GetComponent<Particle>()->SetActive(false);
		}

		if (this->GetComponent<TransparentMesh>() != nullptr)
		{
			this->GetComponent<TransparentMesh>()->SetActive(false);
		}

		// physx ���� �κ�
		// collider�� ������Ʈ�� ��Ȱ��ȭ ������ �� simulation�� ����Ǿ�� ��
		if (this->GetComponent<BoxCollider>() != nullptr)
		{
			this->GetComponent<BoxCollider>()->SetActorState(false);
			this->GetComponent<BoxCollider>()->_debugObject->SetActive(false);
		}

		if (this->GetComponent<CylinderCollider>() != nullptr)
		{
			this->GetComponent<CylinderCollider>()->SetActorState(false);
			this->GetComponent<CylinderCollider>()->_debugObject->SetActive(false);
		}
	}
	else
	{
		if (this->GetComponent<MeshRenderer>() != nullptr)
		{
			// ������Ʈ�� ��Ȱ��ȭ ���°� �ƴ϶�� �ش� ������Ʈ�� active ���¸� ����
			this->GetComponent<MeshRenderer>()->SetRenderingState(this->GetComponent<MeshRenderer>()->GetActivated());
		}

		if (this->GetComponent<Light>() != nullptr)
		{
			this->GetComponent<Light>()->SetLightState(this->GetComponent<Light>()->GetActivated());
		}

		if (this->GetComponent<ImageRenderer>() != nullptr)
		{
			this->GetComponent<ImageRenderer>()->SetImageStatus(this->GetComponent<ImageRenderer>()->GetActivated());
		}

		if (this->GetComponent<Particle>() != nullptr)
		{
			this->GetComponent<Particle>()->SetActive(this->GetComponent<Particle>()->GetActivated());
		}

		if (this->GetComponent<TransparentMesh>() != nullptr)
		{
			this->GetComponent<TransparentMesh>()->SetActive(this->GetComponent<TransparentMesh>()->GetActivated());
		}

		if (this->GetComponent<BoxCollider>() != nullptr)
		{
			this->GetComponent<BoxCollider>()->SetActorState(this->GetComponent<BoxCollider>()->GetActivated());
			this->GetComponent<BoxCollider>()->_debugObject->SetActive(this->GetComponent<BoxCollider>()->GetActivated());
		}

		if (this->GetComponent<CylinderCollider>() != nullptr)
		{
			this->GetComponent<CylinderCollider>()->SetActorState(this->GetComponent<CylinderCollider>()->GetActivated());
			this->GetComponent<CylinderCollider>()->_debugObject->SetActive(this->GetComponent<CylinderCollider>()->GetActivated());
		}
	}

	// �ڽ� ������Ʈ�鵵 �� �Լ��� ������ ����
	if (this->_childContainer.empty())
	{
		return;
	}

	for (int i = 0; i < this->_childContainer.size(); i++)
	{
		if (!active)
		{
			_childStatus.emplace_back(_childContainer[i]->GetActivated());
			_childContainer[i]->SetActive(active);
		}
		else
		{
			if (!_childStatus.size() > 0)
			{
				break;
			}

			_childContainer[i]->SetActive(_childStatus[i]);
		}

		//_childContainer[i]->SetActive(active);
	}

}

bool KunrealEngine::GameObject::GetActivated()
{
	return this->_isActivated;
}

void KunrealEngine::GameObject::SetLayer(int layer)
{
	this->_layer = layer;
}

int KunrealEngine::GameObject::GetLayer() const
{
	return this->_layer;
}

void KunrealEngine::GameObject::SetTag(const std::string& tag)
{
	this->_tag = tag;
}

std::string KunrealEngine::GameObject::GetTag()
{
	return this->_tag;
}


void KunrealEngine::GameObject::SetCollider(Collider* col)
{
	this->_collider = col;
}


KunrealEngine::Collider* KunrealEngine::GameObject::GetCollider()
{
	return this->_collider;
}

std::string KunrealEngine::GameObject::GetOriginalTypeName(std::string name)
{
	std::string typeName = name;

	const std::string classPrefix = "class ";
	const std::string structPrefix = "struct ";

	if (typeName.substr(0, classPrefix.size()) == classPrefix)			// Ŭ�����ΰ�
	{
		typeName = typeName.substr(classPrefix.size());
	}
	else if (typeName.substr(0, structPrefix.size()) == structPrefix)	// ����ü�ΰ�
	{
		typeName = typeName.substr(structPrefix.size());
	}

	size_t colon = typeName.rfind("::");								// ���ӽ����̽� ã�Ƽ� ����
	if (colon != std::string::npos)
	{
		typeName = typeName.substr(colon + 2);
	}

	return typeName;
}

void KunrealEngine::GameObject::DeleteComponent(Component* component)
{
	auto iter = find(this->_componentContainer.begin(), this->_componentContainer.end(), component);

	if (iter != _componentContainer.end())			// ã�°� ������
	{
		(*iter)->Release();							// ������Ʈ���� �����Ұ� ������ ���ְ�
		delete* iter;								// �ش� ������Ʈ�� ����
		this->_componentContainer.erase(iter);			// �����̳ʿ��� �����
	}
}

void KunrealEngine::GameObject::ClearComponent()
{
	for (Component* component : this->_componentContainer)
	{
		component->Release();						// ������ ������ ������ ���� �������ְ�
		delete component;							// clear�ϱ����� delete �� �������
	}

	this->_componentContainer.clear();
	this->_componentContainer.shrink_to_fit();
}

std::vector<KunrealEngine::Component*> KunrealEngine::GameObject::GetComponentList()
{
	return this->_componentContainer;
}

void KunrealEngine::GameObject::SetTotalComponentState(bool flag)
{
	for (auto components : this->_componentContainer)
	{
		components->SetActive(flag);
	}
}
