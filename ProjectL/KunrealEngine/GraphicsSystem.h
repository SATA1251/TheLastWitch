#pragma once
#include <windows.h>
#include "CommonHeader.h"
#include "GameObject.h"
#include "../GraphicsInterface/GraphicsHeaders.h"

/// <summary>
/// �׷��Ƚ� �������̽� Ŭ������ ����� �پ��� �κп��� ����ϱ� ���� ���̾�
/// �̱���ȭ�ؼ� ��𼭵� ȣ�� �����ϰ�
/// 
/// �̰�ȣ
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
		// �̱��� ��ü ��ȯ
		static GraphicsSystem& GetInstance();

		void Initialize(HWND hwnd, int screenWidth, int screenHeight);
		void Update(int x, int y);
		void Release();

		GInterface::GraphicsInterface* GetGraphics();

	public:
		// ť��� ����
		void CreateCubeMap(const char* cubeMapName, const char* textureName = "", bool isCube = false);

		// ť��� ����	// ���� �� ����ߴ� �̸����� Ž���ؼ� ������
		void DeleteCubeMap(const char* cubeMapName);

		// ȭ�鿡 ��� �� ���� ť��� ����
		// ���� ������Ʈ������ �ϳ��� ť��ʸ� ��� ����
		void SetMainCubeMap(std::string cubeMapName);

		// ť��� ����Ʈ ��ȯ
		const std::vector<std::string> GetCubeMapList();

	private:
		// pick ������ ������Ʈ �����̳�
		std::vector<GameObject*> _pickableList;
		GameObject* _pickedObject;

		// picking���� ���� GraphicsRenderable�� ������ �ִ� ������Ʈ ��ȯ
		void SetPickedObject(int mouseX, int mouseY);

	public:
		// pick�� ������Ʈ ��ȯ
		GameObject* GetPickedObject();

		// pick�� ������Ʈ�� nullptr��
		void ClearPickedObject();

	public:
		// picking�� ������ ������Ʈ���� �����̳� ��ȯ
		const std::vector<GameObject*> GetPickableList();

		// Render ������ ������Ʈ���� ��� ��ȯ
		const std::vector<std::string> GetRenderableList();

		// ��밡���� �ؽ�ó ��� ��ȯ
		const std::vector<std::string> GetTextureList();
		 
	private:
		GInterface::GraphicsInterface* _graphics;
	};
}
