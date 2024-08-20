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
	std::string originalName = "GameObject";	// 기본 이름 저장용
	this->_objectName = "GameObject";			// 아무 이름도 지정해주지 않았을 경우 GameObject라는 이름으로 생성
	this->_originalName = "GameObject";

	for (auto object : smInstance.GetCurrentScene()->GetObjectList())
	{
		if (this->_objectName == object->GetObjectName())		// 중복 확인
		{
			objectCount++;			// 이름이 중복될 경우 뒤에 괄호와 숫자를 붙여 다른 이름으로 생성
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
	// 오브젝트가 생성되자마자 트랜스폼을 만들어서 넣어줌
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
	/// if안에 반복문안에 if문.. 비효율적이다
	/// 오브젝트들의 상태를 체크 후 각각을 컨테이너로 나눠서 반복문을 돌리는 방법 구상중
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
	// 부모에 이 오브젝트와의 관계를 해제하라고 전달
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
	// 해당 scene에 이 오브젝트 추가
	scene->AddObject(this);

	// 기존 scene에서 이 오브젝트 삭제
	auto iter = find(GetObjectScene()->GetObjectList().begin(), GetObjectScene()->GetObjectList().end(), this);
	if (iter != GetObjectScene()->GetObjectList().end())
	{
		GetObjectScene()->GetObjectList().erase(iter);
	}

	this->_sceneName = scene->GetSceneName();

	// 이동한 scene과 현재 scene이 다르다면 비활성화	// 그래픽스 쪽에서 render 하지 않기 위해
	if (SceneManager::GetInstance().GetCurrentScene() != scene)
	{
		SetActive(false);
	}
}

void KunrealEngine::GameObject::MoveToScene(std::string sceneName)
{
	// 매개변수로 받은 이름과 일치하는 scene 탐색
	auto sceneIter = find_if(SceneManager::GetInstance().GetSceneList()->begin(), SceneManager::GetInstance().GetSceneList()->end(), [&](Scene* scene)
		{
			return scene->GetSceneName() == sceneName;
		});

	// 있다면
	if (sceneIter != SceneManager::GetInstance().GetSceneList()->end())
	{
		// 해당 scene에 이 오브젝트 추가
		(*sceneIter)->AddObject(this);

		// 기존 scene에서 이 오브젝트 삭제
		auto objectIter = find(GetObjectScene()->GetObjectList().begin(), GetObjectScene()->GetObjectList().end(), this);
		if (objectIter != GetObjectScene()->GetObjectList().end())
		{
			GetObjectScene()->GetObjectList().erase(objectIter);
		}

		this->_sceneName = sceneName;

		// 이동한 scene과 현재 scene이 다르다면 비활성화	// 그래픽스 쪽에서 render 하지 않기 위해
		if (SceneManager::GetInstance().GetCurrentScene() != (*sceneIter))
		{
			SetActive(false);
		}
	}
}

void KunrealEngine::GameObject::SetActive(bool active)
{
	this->_isActivated = active;

	// 그래픽스쪽에 Render 관련 업데이트가 있어서 어쩔 수 없었다
	// 오브젝트가 비활성화 되어 있을 때
	if (!this->_isActivated)
	{
		// 그래픽스 관련 된 컴포넌트가 있는지 확인
		if (this->GetComponent<MeshRenderer>() != nullptr)
		{
			// 있다면 비활성화
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

		// physx 관련 부분
		// collider는 오브젝트가 비활성화 상태일 때 simulation이 종료되어야 함
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
			// 오브젝트가 비활성화 상태가 아니라면 해당 컴포넌트의 active 상태를 따라감
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

	// 자식 오브젝트들도 이 함수의 영향을 받음
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

	if (typeName.substr(0, classPrefix.size()) == classPrefix)			// 클래스인가
	{
		typeName = typeName.substr(classPrefix.size());
	}
	else if (typeName.substr(0, structPrefix.size()) == structPrefix)	// 구조체인가
	{
		typeName = typeName.substr(structPrefix.size());
	}

	size_t colon = typeName.rfind("::");								// 네임스페이스 찾아서 제거
	if (colon != std::string::npos)
	{
		typeName = typeName.substr(colon + 2);
	}

	return typeName;
}

void KunrealEngine::GameObject::DeleteComponent(Component* component)
{
	auto iter = find(this->_componentContainer.begin(), this->_componentContainer.end(), component);

	if (iter != _componentContainer.end())			// 찾는게 있으면
	{
		(*iter)->Release();							// 컴포넌트에서 해제할게 있으면 해주고
		delete* iter;								// 해당 컴포넌트를 삭제
		this->_componentContainer.erase(iter);			// 컨테이너에서 비워줌
	}
}

void KunrealEngine::GameObject::ClearComponent()
{
	for (Component* component : this->_componentContainer)
	{
		component->Release();						// 해제할 내용이 있으면 먼저 해제해주고
		delete component;							// clear하기전에 delete 다 해줘야함
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
