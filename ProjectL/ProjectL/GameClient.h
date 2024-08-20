#pragma once
#define _SILENCE_CXX20_CISO646_REMOVED_WARNING
#include <windows.h>


/// <summary>
/// ������ Ŭ���̾�Ʈ
/// ���α׷��� �⺻ ����, ���ӿ��� �ʱ�ȭ ��
/// 
/// ���� ������ ���α׷��Ӱ� ����� ���� �߰� ����
/// 
/// [�̰�ȣ]
/// </summary>

///
/// ��������!
/// 
/// �׷��Ƚ� �׽�Ʈ������ �׷��Ƚ� �������̽� ����� �߰�
/// �߰����Ӽ��� ArkEngine.lib �߰�
/// _pRenderer ������� �߰�
/// Initialize, Update�� ������ ���� �Լ� �߰�
///
/// ����ȭ
///

namespace KunrealEngine
{
	class EngineCore;
}

class Deserialize;

namespace GameClient 
{
	class GameClient
	{
	public:
		GameClient();
		~GameClient();

	private:
		// ���� ����
		HWND _hWnd;		// �ڵ�������
		MSG _MSG;		// �޼���

		// ȭ���� ��ġ	// �ʱⰪ�� 0, 0
		int _windowPosX;
		int _windowPosY;

		// ȭ���� ũ��	// �ʱⰪ�� 1920 x 1080
		int _screenWidth;
		int _screenHeight;

		// ���ӿ��� �ھ�
		KunrealEngine::EngineCore* _core;

	public:
		// ���� Ŭ���̾�Ʈ �ʱ�ȭ
		HRESULT Initialize(HINSTANCE hInstance);

		// ��� ������Ʈ�� ����, �޼��� ���¿� ���� ����
		void GameLoop();

		// ���� Ŭ���̾�Ʈ �޸� ����
		void Release();

		// Ŭ���̾�Ʈ â�� ������ ������ ũ�� ����
		void SetWindowSize();
		void RecalculateWindowSize();

	private:
		// ���ӿ����� ����������Ŭ�� ������
		void UpdateAll();

		// ������ ���ν��� ����
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	
	private:
		Deserialize* _deserialize;
	};
}

