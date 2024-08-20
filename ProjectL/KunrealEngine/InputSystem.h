#pragma once
#include <dinput.h>
#include "CommonHeader.h"
#include "InputSystemStruct.h"

#pragma comment(lib, "dinput8")
#pragma comment(lib, "dxguid")

namespace KunrealEngine
{
	class _DECLSPEC InputSystem
	{
	private:
		InputSystem();
		~InputSystem();

	private:
		HWND _hWnd;
		IDirectInput8* _input;
		IDirectInputDevice8* _keyboard;
		IDirectInputDevice8* _mouse;
		LPVOID _cKey;

		char _keybuffer[256] = {};		// �� �����ӿ� ���� Ű����
		char _prevkeybuffer[256] = {};	// ���� �����ӿ� ���� Ű����
		DIMOUSESTATE _mouseState;		// �� �����ӿ� ���� ���콺 ����
		BYTE _prevrgbButtons[4] = {};	// ���� �����ӿ� ���� ���콺 ����
		
		bool _kdown[256] = {};			// �� �������� Ű�ٿ� ���¸� �����ϴ� ����
		bool _kup[256] = {};			// �� �������� Ű�� ���¸� �����ϴ� ����
		bool _mdown[4] = {};			// �� �������� Ű�ٿ� ���¸� �����ϴ� ����
		bool _mup[4] = {};				// �� �������� Ű�� ���¸� �����ϴ� ����

		int _mouseWheelData;			// DIMOUSESTATE�� lz���� �� �Է°��̴�. 
		int _prevmouseWheelData;		// �� ������ lz ���� ���ϴ� ������ ���� �󸶳� ���������� �� �� �ִ�.
		int _mouseX = 0;				// ���콺 ��ǥ�� �����ϱ� ���� �κ�
		int _mouseY = 0;				// ���콺 ��ǥ�� �����ϱ� ���� �κ�

		POINT _editorMousePos;			// �������� ���콺 ��ǥ

		RECT _clientRect = {};
		POINT _centerpos = {};
		
		// DPI��� �� Ŭ���� ������ ���� �����ϴ� ������
		int _screenWidth;
		int _screenHeight;
		int _windowsHeight;
		int _windowsWidth;

		int dpiX;
		int dpiY;

	public:
		// �ν��Ͻ��� ������ �� �ִ� �޼���
		static InputSystem* GetInstance();

		// �Է½ý��� �ʱ�ȭ �Լ�
		bool Initialize(HINSTANCE instance, HWND hwnd, int screenHeight, int screenWidth);
		void Release();

		// �� �����Ӹ��� �۵�. ������ ����̽����� ������ ���������� �ƴ����� üũ. 
		void Update(float fTime);

		// ������ ���콺 ��ǥ�� �޾ƿ�
		void UpdateEditorMousePos(POINT pos);
		POINT GetEditorMousePos();

		// �Էµ� ��ư�� boolŸ������ ��ȯ (���ȴ°� �ȴ��ȴ°�)
		bool WINAPI KeyInput(KEY keycode);
		bool WINAPI KeyUp(KEY keycode);
		bool WINAPI KeyDown(KEY keycode);

		// �Էµ� ���콺�� boolŸ������ ��ȯ (���ȴ°� �ȴ��ȴ°�)
		bool WINAPI MouseButtonInput(int button);
		bool WINAPI MouseButtonUp(int button);
		bool WINAPI MouseButtonDown(int button);

		// ���콺 �ٰ��� bool Ÿ������ ��ȯ
		bool WINAPI WheelUp();
		bool WINAPI WheelDown();

		// �Ű����� xpos, ypos�� ���콺�� ��ǥ�� �����ϴ� �Լ�.
		void GetMousePosition(int& xpos, int& ypos);

		// �Ű����� curState, prevState�� ���� ���簪, ���������Ӱ��� �����ϴ� �Լ�.
		void GetWheelState(int& curState, int& prevState);
		int GetWheelState();

	private:
		bool MBUp(int button);
		bool MBDown(int button);


		bool KUp(KEY keycode);
		bool KDown(KEY keycode);
	};
}