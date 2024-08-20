#pragma once
#include "CommonHeader.h"

/// <summary>
/// Scene
/// 
/// 
/// </summary>

#pragma warning(disable: 4251)

namespace KunrealEngine
{
	class GameObject;
	// ī�޶� �ʿ�

	class _DECLSPEC Scene
	{	// ���Ŵ��������� ���� ������ �� �ְ�
		friend class SceneManager;
	private:
		Scene();
		Scene(std::string sceneName);	// scene�� �̸��� string���� �Ű������� �޾Ƽ� �ٷ� ����
		~Scene();

	public:
		void Initialize();

		// scene�� ��� �ִ� ������Ʈ���� �������ְ�
		// scene ��ü�� ���� �޸� ������ SceneManager���� ���ش�
		void Release();
		void FixedUpdate();
		void Update();
		void LateUpdate();

		void Render();

	private:
		std::vector<GameObject*> _objectList;	// ���� scene�� ��� ������Ʈ��
		std::string _sceneName;					// scene�� �������� �̸�
		GameObject* _mainCamera;

	public:
		// scene�� ������Ʈ�� ������ִ� �Լ�
		GameObject* CreateObject();

		// scene�� ������Ʈ�� ������ִ� �Լ�
		// �Ű������� �̸��� �̸� �޾� object�̸����� �־���
		GameObject* CreateObject(std::string objectName);

		// �̹� ������� ������Ʈ�� scene�� �߰�
		void AddObject(GameObject* obj);

		// scene�� �ִ� ������Ʈ�� ����
		void DeleteGameObject(std::string objectName);
		void DeleteGameObject(GameObject* obj);

		// scene�� �̸��� string���� ��ȯ
		std::string GetSceneName();

		// scene�� ������ �ִ� ������Ʈ���� ����� ��ȯ
		std::vector<GameObject*>& GetObjectList();

		// ���޹��� �̸��� Ư�� ������Ʈ�� ��ȯ
		GameObject* GetGameObject(std::string objectName);

		// tag�� ���� ������Ʈ �˻� �� ��ȯ
		// �ϳ��� �ް� ���� ��
		/// ���� �±׸� ���� ������Ʈ�� ���� ���� ��� ���� ���� Ž���Ǵ� �ְ� ��ȯ�Ǵ� ����
		GameObject* GetObjectWithTag(std::string tag);

		// �ش� tag�� ���� ��� ������Ʈ�� �ް� ������
		std::vector<GameObject*> GetObjectsWithTag(std::string tag);
		

		// ī�޶� ������Ʈ �� ���޹��� ������Ʈ�� ���� ī�޶�� ����
		void SetMainCamera(GameObject* obj);
		GameObject* GetMainCamera();

	private:
		// ������Ʈ�� ���� Layer���� �������� scene�� �����ϴ� ������Ʈ���� ����
		// Layer���� ���� ������Ʈ�� ���� ������ -> ������ �򸰴�
		void SortObjects();
	};
}

#pragma warning (default: 4251)