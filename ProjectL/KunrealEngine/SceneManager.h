#pragma once
#include "CommonHeader.h"

/// <summary>
/// Scene�� �������� Ŭ����
/// �ϳ��� �����ؾ��ϰ� �������� �ҷ��ٴϴ� ģ���ٺ��� �̱������� �������
/// 
/// �̰�ȣ
/// </summary>

namespace KunrealEngine
{
	class Scene;

	class SceneManager
	{
	private:
		SceneManager();
		~SceneManager();

	public:
		static SceneManager& GetInstance()
		{
			static SceneManager m_instance;
			return m_instance;
		}

	public:
		// ����, �ʱ�ȭ ����
		void Initialize();

		// SceneManager�� release�� �� �����ϴ� scene���� ��� release���ְ� �� �� ����
		void Release();

		// scene�� �����ϱ� ���� �Լ���
	public:
		// scene�� ������ִ� �Լ�
		Scene* CreateScene(std::string sceneName);
		
		// ���� ������ scene�� ��ȯ���ִ� �Լ�
		Scene* GetCurrentScene();

		// Ư�� scene�� ��ȯ���ִ� �Լ�
		// ã�� scene�� ������ nullptr ��ȯ
		Scene* GetScene(std::string sceneName);

		// ���� scene�� �ٲ��ִ� �Լ�
		// string �Ű������� �޾Ƽ� �ش� �̸��� ������ _currentScene�� �ش� scene���� ����
		void ChangeScene(std::string sceneName);

		// scene ����� ��ȯ		// DLL���� extern"C"�� ���� STL �Լ� ������ �Ұ����ϹǷ�
		std::vector<Scene*>* GetSceneList();

	public:
		void UpdateScene(Scene* scene);
		void FixedUpdateScene(Scene* scene);
		void LateUpdateScene(Scene* scene);

		// Ư�� scene ����
		void DeleteScene(std::string sceneName);
	private:
		Scene* _currentScene;				// ���� scene�� ��Ƶ� ����
		std::vector<Scene*> _sceneList;		// ������� scene�� ��Ƶ� ���������̳�
	};
}

