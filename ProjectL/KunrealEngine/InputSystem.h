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

		char _keybuffer[256] = {};		// 현 프레임에 대한 키버퍼
		char _prevkeybuffer[256] = {};	// 이전 프레임에 대한 키버퍼
		DIMOUSESTATE _mouseState;		// 현 프레임에 대한 마우스 버퍼
		BYTE _prevrgbButtons[4] = {};	// 이전 프레임에 대한 마우스 버퍼
		
		bool _kdown[256] = {};			// 현 프레임의 키다운 상태를 보관하는 버퍼
		bool _kup[256] = {};			// 현 프레임의 키업 상태를 버관하는 버퍼
		bool _mdown[4] = {};			// 현 프레임의 키다운 상태를 보관하는 버퍼
		bool _mup[4] = {};				// 현 프레임의 키업 상태를 버관하는 버퍼

		int _mouseWheelData;			// DIMOUSESTATE의 lz값이 휠 입력값이다. 
		int _prevmouseWheelData;		// 이 변수에 lz 값을 더하는 식으로 휠이 얼마나 움직였는지 알 수 있다.
		int _mouseX = 0;				// 마우스 좌표를 저장하기 위한 부분
		int _mouseY = 0;				// 마우스 좌표를 저장하기 위한 부분

		POINT _editorMousePos;			// 에디터의 마우스 좌표

		RECT _clientRect = {};
		POINT _centerpos = {};
		
		// DPI계산 및 클램프 설정을 위해 존재하는 변수들
		int _screenWidth;
		int _screenHeight;
		int _windowsHeight;
		int _windowsWidth;

		int dpiX;
		int dpiY;

	public:
		// 인스턴스에 접근할 수 있는 메서드
		static InputSystem* GetInstance();

		// 입력시스템 초기화 함수
		bool Initialize(HINSTANCE instance, HWND hwnd, int screenHeight, int screenWidth);
		void Release();

		// 매 프레임마다 작동. 지금은 디바이스와의 연결이 끊어졌는지 아닌지를 체크. 
		void Update(float fTime);

		// 에디터 마우스 좌표를 받아옴
		void UpdateEditorMousePos(POINT pos);
		POINT GetEditorMousePos();

		// 입력된 버튼을 bool타입으로 반환 (눌렸는가 안눌렸는가)
		bool WINAPI KeyInput(KEY keycode);
		bool WINAPI KeyUp(KEY keycode);
		bool WINAPI KeyDown(KEY keycode);

		// 입력된 마우스를 bool타입으로 반환 (눌렸는가 안눌렸는가)
		bool WINAPI MouseButtonInput(int button);
		bool WINAPI MouseButtonUp(int button);
		bool WINAPI MouseButtonDown(int button);

		// 마우스 휠값을 bool 타입으로 반환
		bool WINAPI WheelUp();
		bool WINAPI WheelDown();

		// 매개변수 xpos, ypos에 마우스의 좌표를 전달하는 함수.
		void GetMousePosition(int& xpos, int& ypos);

		// 매개변수 curState, prevState에 휠의 현재값, 이전프레임값을 전달하는 함수.
		void GetWheelState(int& curState, int& prevState);
		int GetWheelState();

	private:
		bool MBUp(int button);
		bool MBDown(int button);


		bool KUp(KEY keycode);
		bool KDown(KEY keycode);
	};
}