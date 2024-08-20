#pragma once
#include "CommonHeader.h"

/// <summary>
/// ������Ʈ Ŭ����
/// ������Ʈ���� �� Ŭ������ ���� �����Ǹ�, ������Ʈ �������� Ŭ������ ����� ���� �ƴ϶�
/// ��� ������Ʈ�� �� Ŭ������ ���ϵǰ� �̸�, ������Ʈ�� ���еȴ�
/// 
/// �̰�ȣ
/// </summary>

#pragma warning(disable: 4251)

namespace KunrealEngine
{
	class Component;
	class Transform;
	class Collider;

	class _DECLSPEC GameObject
	{
		friend class Scene;
		friend class SceneManager;
	private:							// ���� ������ �� ����, scene�� ���ؼ� ����
		GameObject();
		GameObject(std::string name);
		~GameObject();

	private:
		// ��� ������ scene, Ȥ�� LifeCycle�� ���� ����
		void Initialize();				
		void Awake();
		void OnEnable();
		void Start();

		void FixedUpdate();
		void Update();
		void LateUpdate();

		void OnDisable();
		void Release();

	private: 
		std::vector<Component*> _componentContainer;		// ������Ʈ���� ���� ����
		bool _isActivated;									// Ȱ��ȭ ����
		std::string _objectName;							// ������Ʈ �̸�
		std::string _originalName;							// ������Ʈ�� ����Ǿ����� �̸� ������ ����
		std::string _sceneName;								// � scene�� ����ִ°�
		Transform* _transform;								// ������Ʈ�� transform�� ���� �� ������ �̸� �־���
		GameObject* _parent;								// �θ� ������Ʈ
		int _layer;											// ���� ������ ���õ� ���̾�
		std::string _tag;									// ������Ʈ�� ���� �±�

		std::vector<GameObject*> _childContainer;			// �ڽ� ������Ʈ�� ���� ����

		std::vector<bool> _childStatus;						// �ڽ� ������Ʈ���� active ����

		Collider* _collider;								// collider ��ü

	public:
		bool _autoAwake;									// scene �̵� �� �ڵ����� ������
		
		// ������Ʈ�� �̸��� ��ȯ
		std::string GetObjectName();

		// ������Ʈ�� ���� �̸��� ��ȯ (��ȣ ���� ��)
		std::string GetObjectOriginalName();

		// ������Ʈ�� �̸� ����
		void SetObjectName(const std::string& name);

		// �θ� ������Ʈ ����
		void SetParent(GameObject* obj);

		/// �θ��ڽ� ���踸 ������ �� �޸𸮸� �����ϴ� ���� �ƴϴ� ����
		// �θ� �ڽİ��� ����		// �θ�κ��� ����
		void DetachFromParent();

		// �θ� �ڽİ��� ����		// Ư�� �ڽ� ������Ʈ ����
		void DetachChild(GameObject* child);

		// �θ� ������Ʈ ��ȯ
		GameObject* GetParent();

		// �ڽ� ������Ʈ�� �ִ��� bool�� ��ȯ
		bool CheckChild();

		// �ڽ� ������Ʈ���� ����ִ� �����̳� ��ȯ
		std::vector<GameObject*> GetChilds();

		GameObject* GetChild(std::string name);

		// ���� �Ҽӵ� scene ��ȯ
		Scene* GetObjectScene();

		// �Ҽӵ� scene�� �ٲ��ִ� �Լ�
		void MoveToScene(Scene* scene);
		void MoveToScene(std::string sceneName);

		// Ȱ��ȭ ���θ� �����ִ� �Լ�
		// true = Ȱ��ȭ / false = ��Ȱ��ȭ
		void SetActive(bool active);

		// ������Ʈ�� Ȱ��ȭ ���θ� ��ȯ true / false
		bool GetActivated();

		// ���� ������ ���õ� ���̾ �����ϴ� �Լ�
		// ���� ���� ���� ���� -> ������ ��
		void SetLayer(int layer);

		// ���̾� ��ġ�� ��ȯ�ϴ� �Լ�
		int GetLayer() const;

		// �±� ����
		void SetTag(const std::string& tag);

		// �±� ��ȯ
		std::string GetTag();

		// collider ����
		void SetCollider(Collider* col);

		// object�� collider ��ȯ
		Collider* GetCollider();

	/// ������Ʈ ����
	private:
		// ���ø��� typeid.Name()�� ����ϸ� class ���ӽ����̽�::Ŭ���� �̸� <- �̷��� ��ȯ��
		// class�� ���ӽ����̽� �̸��� �����ϰ� ���� �̸��� ��ȯ���ִ� �Լ�
		static std::string GetOriginalTypeName(std::string name);

	public:
		// ������Ʈ�� ������Ʈ�� �߰��ϴ� �Լ�
		template<typename T>
		T* AddComponent()
		{
			T* newComponent = new T();

			this->_componentContainer.emplace_back(newComponent);
			if (newComponent->GetOwner() == nullptr)
			{
				newComponent->SetOwner(this);											// ������Ʈ�� �����ָ� �� ������Ʈ��
				newComponent->_componentName = GetOriginalTypeName(typeid(T).name());	// ������Ʈ�� �̸��� �־���
			}
			newComponent->Initialize();
			return newComponent;
		}

		// ������Ʈ�� �Ű������� ���� ������Ʈ�� �߰��ϴ� �Լ�
		template<typename T, typename U>
		void AddComponentParam(U param)
		{
			T* newComponent = new T(param);

			this->_componentContainer.emplace_back(newComponent);
			if (newComponent->GetOwner() == nullptr)
			{
				newComponent->SetOwner(this);											// ������Ʈ�� �����ָ� �� ������Ʈ��
				newComponent->_componentName = GetOriginalTypeName(typeid(T).name());	// ������Ʈ�� �̸��� �־���
			}
			newComponent->Initialize();
		}

		// ������Ʈ�� Ư�� ������Ʈ�� ��ȯ�ϴ� �Լ�
		// componentContainer�� ������Ʈ �������� �����̳�
		template<typename T>
		T* GetComponent()
		{
			for (int i = 0; i < _componentContainer.size(); i++)
			{
				// dynamic_cast�� ����ȯ ���н� nullptr�� ��ȯ�Ѵ�
				Component* component = dynamic_cast<T*>(_componentContainer[i]);
				if (component != nullptr)			// ã�°� �־ nullptr�� �ƴ� ���
				{
					return (T*)component;			// ������Ʈ�� ���޹��� T �������� ��ȯ�ؼ� ����
				}
			}

			return nullptr;
		}

		// Ư�� ������Ʈ�� �����ϴ� �Լ�
		void DeleteComponent(Component* component);

		// ������Ʈ�� �Ҵ�� ��� ������Ʈ�� �����ϴ� �Լ�
		void ClearComponent();

		// ������Ʈ ����Ʈ�� ��ȯ
		std::vector<Component*> GetComponentList();

		// ��� ������Ʈ�� ���� ų �� �ֵ���
		void SetTotalComponentState(bool flag);
	};
}

#pragma warning(default: 4251)