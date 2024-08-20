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

		// ���� �� �Լ��� ������ �߻��ߴٸ� 3�ܰ�� Ȯ�� �Ұ�.
		// Assert�� ���� �޼����� ��Ÿ���� �Լ��� ��ġ�� Ȯ�� �ϼ���.
		// �Լ��� ��ġ�� hr ������ Ȯ�� �ϼ���.
		// �ش� hr ������ ������� ������ �����ϸ� ���� ����̴ϴ�.
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
		////////////////////// ������� Ű���� �ʱ�ȭ //////////////////////

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
		////////////////////// ������� ���콺 �ʱ�ȭ //////////////////////
		// ���콺 Ŀ�� ��ġ ���.
		// ���콺 Ŀ���� (��ũ�� ����)
		POINT curspos = {};
		GetClientRect(_hWnd, &_clientRect);	// Ŭ���̾�Ʈ�� ������ ũ�⸦ �޾ƿ���
		GetCursorPos(&curspos);	// ���콺�� ������ġ�� �޾ƿ���
		ScreenToClient(_hWnd, &curspos);	// �޾ƿ� ���콺�� ���� ��ġ�� Ŭ�� �������� ����

		// Ŭ���̾�Ʈ�� �߾��� ����
		_centerpos.x = _clientRect.right / 2;
		_centerpos.y = _clientRect.bottom / 2;
		_mouseX = _centerpos.x;					// Ŭ���̾�Ʈ�� ���콺 ��ǥ�� �ű�� �ű��
		_mouseY = _centerpos.y;

		ClientToScreen(_hWnd, &_centerpos);	// �� ��ǥ�� ��ũ�� �������� ����
		SetCursorPos(_centerpos.x, _centerpos.y);	// �װ��� ���콺 Ŀ���� ������ �α�

		_windowsHeight = screenHeight;
		_windowsWidth = screenWidth;

		return true;
	}

	void InputSystem::Release()
	{
		// Ű���� ����
		if (_mouse)
		{
			_mouse->Unacquire();
			_mouse->Release();
			_mouse = 0;
		}

		// Ű���� ����
		if (_keyboard)
		{
			_keyboard->Unacquire();
			_keyboard->Release();
			_keyboard = 0;
		}

		// DirectInput8 ����
		if (_input)
		{
			_input->Release();
			_input = 0;
		}
	}

	// �� �Լ����� �ϴ� ���� ����̽����� ������ Ȯ���ϰ� ������ �������ٸ� �ٽ� ������ �õ��մϴ�.
	// ������â�� �������ų� �ֻ����� �������� ���ϸ� ����̽��ν�Ʈ ���°� �˴ϴ�.
	// ASSERT �ּ��� �����ϰ� ����� �� ��� �ߴ����� �ɸ������� Assert�� �ɸ��Ƿ�
	// �Է°��� ������� ������ �ƴ϶�� �ּ��� Ǯ�� ������.
	void InputSystem::Update(float fTime)
	{
		HRESULT hr;


		// �� �κ� �������� ���� �� ����... ���� ��
		// Ŭ���̾�Ʈ�� ��Ŀ���� ���������� ��ǲ �ްڴٴ°Ŷ� �����Ϳ��� �Է��� ���޴´�.
		//if (GetFocus() != _hWnd )
		//{
		//	return;
		//}

		// ����̽��� ���¸� Ȯ���ϴ� �Լ�.
		// 1������ = 2������ �Ķ������ ���� ����Ʈ ������
		// 2������ = ��ġ�� ������¸� ������ ����ü�� �ּ�
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

		// �����Ӹ��� ���콺�� ��ġ�� ����
		POINT curspos = {};
		GetCursorPos(&curspos);				// ���콺�� ������ġ�� �޾ƿ���
		ScreenToClient(_hWnd, &curspos);	// �޾ƿ� ���콺�� ���� ��ġ�� Ŭ�� �������� ���� 

		_mouseX = curspos.x;
		_mouseY = curspos.y;
		_mouseWheelData += _mouseState.lZ;

		GRAPHICS->DrawDebugText(120,120,20,"x %d / y %d", _mouseX, _mouseY);

		/////////////////////////////////////////////////////////////////
		//////////	  ��ũ�� ������ �ȳ����� Ŭ���� �Ŵ� �κ�	   //////////
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
		/////	���������� Ű�� �ٿ�� ���� �Ǵ��ϰ� ���� ���ִ� �κ�	   ////
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
