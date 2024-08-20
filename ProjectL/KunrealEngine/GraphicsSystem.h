#pragma once
#include <windows.h>
#include "CommonHeader.h"
#include "GameObject.h"
#include "../GraphicsInterface/GraphicsHeaders.h"

/// <summary>
/// 그래픽스 인터페이스 클래스의 기능을 다양한 부분에서 사용하기 위한 레이어
/// 싱글톤화해서 어디서든 호출 가능하게
/// 
/// 이건호
/// </summary>

#define GRAPHICS KunrealEngine::GraphicsSystem::GetInstance().GetGraphics()

namespace GInterface
{
	class GraphicsInterface;
	class GraphicsCamera;
}

namespace KunrealEngine
{
	class _DECLSPEC GraphicsSystem
	{
		friend class MeshRenderer;
	private:
		GraphicsSystem();
		~GraphicsSystem();

	public:
		// 싱글톤 객체 반환
		static GraphicsSystem& GetInstance();

		void Initialize(HWND hwnd, int screenWidth, int screenHeight);
		void Update(int x, int y);
		void Release();

		GInterface::GraphicsInterface* GetGraphics();

	public:
		// 큐브맵 생성
		void CreateCubeMap(const char* cubeMapName, const char* textureName = "", bool isCube = false);

		// 큐브맵 삭제	// 생성 시 사용했던 이름으로 탐색해서 삭제함
		void DeleteCubeMap(const char* cubeMapName);

		// 화면에 출력 될 메인 큐브맵 설정
		// 현재 프로젝트에서는 하나의 큐브맵만 출력 가능
		void SetMainCubeMap(std::string cubeMapName);

		// 큐브맵 리스트 반환
		const std::vector<std::string> GetCubeMapList();

	private:
		// pick 가능한 오브젝트 컨테이너
		std::vector<GameObject*> _pickableList;
		GameObject* _pickedObject;

		// picking으로 얻은 GraphicsRenderable을 가지고 있는 오브젝트 반환
		void SetPickedObject(int mouseX, int mouseY);

	public:
		// pick된 오브젝트 반환
		GameObject* GetPickedObject();

		// pick된 오브젝트를 nullptr로
		void ClearPickedObject();

	public:
		// picking이 가능한 오브젝트들의 컨테이너 반환
		const std::vector<GameObject*> GetPickableList();

		// Render 가능한 오브젝트들의 목록 반환
		const std::vector<std::string> GetRenderableList();

		// 사용가능한 텍스처 목록 반환
		const std::vector<std::string> GetTextureList();
		 
	private:
		GInterface::GraphicsInterface* _graphics;
	};
}
