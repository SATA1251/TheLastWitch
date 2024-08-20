#include <iostream>
#include <windows.h>
#include <windowsx.h>
#include <functional>
#include <assert.h>

#include "InputSystem.h"
#include "GraphicsSystem.h"

#define Assert(formula, message) assert(formula && message)
#define Assert(message) assert(0 && message)

#define KEYDOWN(name, key) ((name[key] & 0x80) ? true : false)
#define KEYUP(name, key) ((name[key] & 0x80) ? false : true)

namespace KunrealEngine
{
	InputSystem::InputSystem()
		:_input(nullptr), _keyboard(nullptr), _mouse(nullptr)
		, _windowsHeight(0), _windowsWidth(0), _mouseX(0), _mouseY(0)
		, _mouseWheelData(0), _prevmouseWheelData(0)
		, _mouseState(), _cKey()
	{
		_screenWidth = GetSystemMetrics(SM_CXSCREEN);
		_screenHeight = GetSystemMetrics(SM_CYSCREEN);

		HDC screen = GetDC(0);
		dpiX = GetDeviceCaps(screen, LOGPIXELSX);
		dpiY = GetDeviceCaps(screen, LOGPIXELSY);
		ReleaseDC(0, screen);
	}

	InputSystem::~InputSystem()
	{

	}

	InputSystem* InputSystem::GetInstance()
	{
		static InputSystem* instance;

		if (instance == nullptr)
		{
			instance = new InputSystem();
		}
		return instance;
	}

	bool InputSystem::Initialize(HINSTANCE instance, HWND hwnd, int screenHeight, int screenWidth)
	{
		_windowsHeight = screenHeight;
		_windowsWidth = screenWidth;
		_hWnd = hwnd;

		// 만약 이 함수에 문제가 발생했다면 3단계로 확인 할것.
		// Assert로 인해 메세지로 나타나는 함수의 위치를 확인 하세요.
		// 함수의 위치의 hr 내용을 확인 하세요.
		// 해당 hr 내용을 기반으로 문제에 접근하면 비교적 쉬울겁니다.
		HRESULT hr;

		if (FAILED(hr = DirectInput8Create(instance, DIRECTINPUT_VERSION,
			IID_IDirectInput8, (void**)&_input, nullptr)))
		{
			Assert("FAILED DirectInput8Create");
			return false;
		}

		if (FAILED(hr = _input->CreateDevice(GUID_SysKeyboard, &_keyboard, nullptr)))
		{
			Assert("FAILED CreateDevice");
			return false;
		}


		if (FAILED(hr = _keyboard->SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
		{
			Assert("FAILED SetCooperativeLevel");
			return false;
		}

		if (FAILED(hr = _keyboard->SetDataFormat(&c_dfDIKeyboard)))
		{
			Assert("FAILED SetDataFormat");
			return false;
		}

		if (FAILED(hr = _keyboard->Acquire()))
		{
			//Assert("FAILED Acquire");
			return false;
		}
		////////////////////// 여기까진 키보드 초기화 //////////////////////

		if (FAILED(hr = _input->CreateDevice(GUID_SysMouse, &_mouse, NULL)))
		{
			Assert("FAILED CreateDevice(Mouse)");
			return false;
		}

		if (FAILED(hr = _mouse->SetDataFormat(&c_dfDIMouse)))
		{
			Assert("FAILED SetDataFormat(Mouse)");
			return false;
		}

		if (FAILED(hr = _mouse->SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
		{
			Assert("FAILED SetCooperativeLevel(Mouse)");
			return false;
		}

		if (FAILED(hr = _mouse->Acquire()))
		{
			//Assert("FAILED Acquire(Mouse)");
			return false;
		}
		////////////////////// 여기까지 마우스 초기화 //////////////////////
		// 마우스 커서 위치 잡기.
		// 마우스 커서는 (스크린 기준)
		POINT curspos = {};
		GetClientRect(_hWnd, &_clientRect);	// 클라이언트의 윈도우 크기를 받아오기
		GetCursorPos(&curspos);	// 마우스의 현재위치를 받아오기
		ScreenToClient(_hWnd, &curspos);	// 받아온 마우스의 현재 위치를 클라 기준으로 변경

		// 클라이언트의 중앙을 저장
		_centerpos.x = _clientRect.right / 2;
		_centerpos.y = _clientRect.bottom / 2;
		_mouseX = _centerpos.x;					// 클라이언트의 마우스 좌표도 거기로 옮기기
		_mouseY = _centerpos.y;

		ClientToScreen(_hWnd, &_centerpos);	// 그 좌표를 스크린 기준으로 변경
		SetCursorPos(_centerpos.x, _centerpos.y);	// 그곳에 마우스 커서를 가져다 두기

		_windowsHeight = screenHeight;
		_windowsWidth = screenWidth;

		return true;
	}

	void InputSystem::Release()
	{
		// 키보드 해제
		if (_mouse)
		{
			_mouse->Unacquire();
			_mouse->Release();
			_mouse = 0;
		}

		// 키보드 해제
		if (_keyboard)
		{
			_keyboard->Unacquire();
			_keyboard->Release();
			_keyboard = 0;
		}

		// DirectInput8 해제
		if (_input)
		{
			_input->Release();
			_input = 0;
		}
	}

	// 이 함수에서 하는 일은 디바이스와의 연결을 확인하고 연결이 끊어졌다면 다시 연결을 시도합니다.
	// 윈도우창이 내려가거나 최상위에 존재하지 못하면 디바이스로스트 상태가 됩니다.
	// ASSERT 주석을 해제하고 디버그 할 경우 중단점에 걸릴때마다 Assert에 걸리므로
	// 입력관련 디버깅이 목적이 아니라면 주석을 풀지 마세요.
	void InputSystem::Update(float fTime)
	{
		HRESULT hr;


		// 이 부분 메인툴로 실행 할 때는... 꺼야 함
		// 클라이언트로 포커스를 잡혔을때만 인풋 받겠다는거라 에디터에서 입력을 못받는다.
		//if (GetFocus() != _hWnd )
		//{
		//	return;
		//}

		// 디바이스의 상태를 확인하는 함수.
		// 1번인자 = 2번인자 파라미터의 버퍼 바이트 사이즈
		// 2번인자 = 장치의 현재상태를 얻어오는 구조체의 주소
		if (FAILED(hr = (_keyboard->GetDeviceState(sizeof(_keybuffer), (LPVOID)&_keybuffer))))
		{
			//Assert("FALID GetDeviceState");

			if (FAILED(_keyboard->Acquire()))
			{
				//Assert("FAILED Acquire");
			}
		}

		_mouse->Poll();

		if (FAILED(hr = (_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&_mouseState))))
		{
			//Assert("FALID GetDeviceState (Mouse)");

			if (FAILED(_mouse->Acquire()))
			{
				//Assert("FAILED Acquire (Mouse)");
			}
		}

		_prevmouseWheelData = _mouseWheelData;

		// 프레임마다 마우스의 위치를 갱신
		POINT curspos = {};
		GetCursorPos(&curspos);				// 마우스의 현재위치를 받아오기
		ScreenToClient(_hWnd, &curspos);	// 받아온 마우스의 현재 위치를 클라 기준으로 변경 

		_mouseX = curspos.x;
		_mouseY = curspos.y;
		_mouseWheelData += _mouseState.lZ;

		GRAPHICS->DrawDebugText(120,120,20,"x %d / y %d", _mouseX, _mouseY);

		/////////////////////////////////////////////////////////////////
		//////////	  스크린 밖으로 안나가게 클램프 거는 부분	   //////////
// 		if (_mouseX > _windowsWidth)
// 		{
// 			_mouseX = _windowsWidth;
// 		}
// 
// 		else if (_mouseX < 0)
// 		{
// 			_mouseX = 0;
// 		}
// 
// 		if (_mouseY < 0)
// 		{
// 			_mouseY = 0;
// 		}
// 
// 		else if (_mouseY > _windowsHeight)
// 		{
// 			_mouseY = _windowsHeight;
// 		}

		///////////////////////////////////////////////////////////////////
		/////	현프레임의 키의 다운과 업을 판단하고 저장 해주는 부분	   ////
		for (int fornum = 0; fornum < 4; fornum++)
		{
			_mdown[fornum] = MBDown(fornum);
			_mup[fornum] = MBUp(fornum);
		}

		for (int fornum = 0; fornum < 256; fornum++)
		{
			_kdown[fornum] = KDown((KEY)fornum);
			_kup[fornum] = KUp((KEY)fornum);
		}
		////////////////////////////////////////////////////////////////////
	}

	void InputSystem::UpdateEditorMousePos(POINT pos)
	{
		this->_editorMousePos = pos;
	}

	POINT InputSystem::GetEditorMousePos()
	{
		return this->_editorMousePos;
	}

	bool WINAPI InputSystem::KeyInput(KEY keycode)
	{
		int key = (int)keycode;

		if (KEYDOWN(_keybuffer, key))
		{
			return true;
		}

		return false;
	}

	bool WINAPI InputSystem::KeyUp(KEY keycode)
	{
		return _kup[(int)keycode];
	}

	bool WINAPI InputSystem::KeyDown(KEY keycode)
	{
		return _kdown[(int)keycode];
	}
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////

	bool __stdcall InputSystem::MouseButtonInput(int button)
	{

		if (KEYDOWN(_mouseState.rgbButtons, button))
		{
			return true;
		}

		return false;
	}

	bool __stdcall InputSystem::MouseButtonUp(int button)
	{
		return _mup[button];
	}

	bool __stdcall InputSystem::MouseButtonDown(int button)
	{
		return _mdown[button];
	}

	bool __stdcall InputSystem::WheelUp()
	{
		if (_mouseWheelData > _prevmouseWheelData)
		{
			return true;
		}

		return false;
	}

	bool __stdcall InputSystem::WheelDown()
	{

		if (_mouseWheelData < _prevmouseWheelData)
		{
			return true;
		}

		return false;
	}

	void InputSystem::GetMousePosition(int& xpos, int& ypos)
	{
		xpos = _mouseX;
		ypos = _mouseY;
	}

	void InputSystem::GetWheelState(int& curState, int& prevState)
	{
		curState = _mouseWheelData;
		prevState = _prevmouseWheelData;
	}

	int InputSystem::GetWheelState()
	{
		return _mouseWheelData;
	}

	////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////

	bool InputSystem::MBUp(int button)
	{
		if (KEYUP(_mouseState.rgbButtons, button) && KEYDOWN(_prevrgbButtons, button))
		{
			_prevrgbButtons[button] = _mouseState.rgbButtons[button];
			return true;
		}

		else if (KEYDOWN(_mouseState.rgbButtons, button) && KEYUP(_prevrgbButtons, button))
		{
			return false;
		}

		else
		{
			_prevrgbButtons[button] = _mouseState.rgbButtons[button];
			return false;
		}
	}

	bool InputSystem::MBDown(int button)
	{
		if (KEYDOWN(_mouseState.rgbButtons, button) && KEYUP(_prevrgbButtons, button))
		{
			_prevrgbButtons[button] = _mouseState.rgbButtons[button];
			return true;
		}

		else if (KEYUP(_mouseState.rgbButtons, button) && KEYDOWN(_prevrgbButtons, button))
		{
			return false;
		}

		else
		{
			_prevrgbButtons[button] = _mouseState.rgbButtons[button];
			return false;
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////

	bool InputSystem::KUp(KEY keycode)
	{
		int key = (int)keycode;

		if (KEYUP(_keybuffer, key) && KEYDOWN(_prevkeybuffer, key))
		{
			_prevkeybuffer[key] = _keybuffer[key];
			return true;
		}

		else if (KEYDOWN(_keybuffer, key) && KEYUP(_prevkeybuffer, key))
		{
			return false;
		}

		else
		{
			_prevkeybuffer[key] = _keybuffer[key];
			return false;
		}
	}
	bool InputSystem::KDown(KEY keycode)
	{
		int key = (int)keycode;

		if (KEYDOWN(_keybuffer, key) && KEYUP(_prevkeybuffer, key))
		{
			_prevkeybuffer[key] = _keybuffer[key];
			return true;
		}

		else if (KEYUP(_keybuffer, key) && KEYDOWN(_prevkeybuffer, key))
		{
			return false;
		}

		else
		{
			_prevkeybuffer[key] = _keybuffer[key];
			return false;
		}
	}
}
