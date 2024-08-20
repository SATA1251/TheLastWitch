#pragma once
#include "CommonHeader.h"

/// <summary>
/// Scene을 관리해줄 클래스
/// 하나만 존재해야하고 여기저기 불려다니는 친구다보니 싱글톤으로 만들었음
/// 
/// 이건호
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
		// 생성, 초기화 관련
		void Initialize();

		// SceneManager가 release될 때 존재하는 scene들을 모두 release해주고 그 후 삭제
		void Release();

		// scene을 관리하기 위한 함수들
	public:
		// scene을 만들어주는 함수
		Scene* CreateScene(std::string sceneName);
		
		// 현재 설정된 scene을 반환해주는 함수
		Scene* GetCurrentScene();

		// 특정 scene을 반환해주는 함수
		// 찾는 scene이 없으면 nullptr 반환
		Scene* GetScene(std::string sceneName);

		// 현재 scene을 바꿔주는 함수
		// string 매개변수를 받아서 해당 이름이 있으면 _currentScene을 해당 scene으로 변경
		void ChangeScene(std::string sceneName);

		// scene 목록을 반환		// DLL에서 extern"C"를 통한 STL 함수 리턴이 불가능하므로
		std::vector<Scene*>* GetSceneList();

	public:
		void UpdateScene(Scene* scene);
		void FixedUpdateScene(Scene* scene);
		void LateUpdateScene(Scene* scene);

		// 특정 scene 삭제
		void DeleteScene(std::string sceneName);
	private:
		Scene* _currentScene;				// 현재 scene을 담아둘 변수
		std::vector<Scene*> _sceneList;		// 만들어진 scene을 담아둘 벡터컨테이너
	};
}

