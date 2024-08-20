#include "GameClient.h"
#include "Deserialize.h"
#include "../KunrealEngine/KunrealAPI.h"

KunrealEngine::EngineCore* engineInstance = KunrealEngine::CreateEngine();

GameClient::GameClient::GameClient()
	:_hWnd(), _MSG(), _windowPosX(0), _windowPosY(0), _screenWidth(2560), _screenHeight(1440),
	_core(nullptr)
{

}

GameClient::GameClient::~GameClient()
{

}

void GameClient::GameClient::SetWindowSize()
{
	RECT nowRect;
	DWORD _style = (DWORD)GetWindowLong(_hWnd, GWL_STYLE);
	DWORD _exstyle = (DWORD)GetWindowLong(_hWnd, GWL_EXSTYLE);

	GetWindowRect(_hWnd, &nowRect);

	RECT newRect{};
	newRect.left = 0;
	newRect.top = 0;
	newRect.right = _screenWidth;
	newRect.bottom = _screenHeight;

	AdjustWindowRectEx(&newRect, _style, NULL, _exstyle);

	// Ŭ���̾�Ʈ �������� ���� ũ��� �� Ŀ�� �Ѵ�. (�ܰ���, Ÿ��Ʋ ��)
	int _newWidth = (newRect.right - newRect.left);
	int _newHeight = (newRect.bottom - newRect.top);

	SetWindowPos(_hWnd, HWND_NOTOPMOST, nowRect.left, nowRect.top,
		_newWidth, _newHeight, SWP_SHOWWINDOW);
}

void GameClient::GameClient::RecalculateWindowSize()
{
	RECT screen;
	GetClientRect(_hWnd, &screen);

	// ũ�Ⱑ �ٲ���ٸ�
	if (_screenWidth != screen.right)
	{
		// �ٲ� ũ��� ���� ������Ʈ
		_screenWidth = screen.right;
		_screenHeight = screen.bottom;

		SetWindowSize();
	}
	else
	{
		int a = 10;
	}
}

HRESULT GameClient::GameClient::Initialize(HINSTANCE hInstance)
{
	/// Win32 ����
	// ���� Ŭ����
	wchar_t szAppName[] = L"The Last Witch";
	WNDCLASSEXW wcex{};

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = this->WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = szAppName;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	// ���� Ŭ���� ���
	RegisterClassExW(&wcex);

	// ���� ����
	_hWnd = CreateWindowW(szAppName, szAppName,
		WS_POPUP | WS_VISIBLE,
		_windowPosX, _windowPosY, _screenWidth, _screenHeight,
		nullptr, nullptr, hInstance, NULL);


	if (!_hWnd)
	{
		return S_FALSE;
	}
	// ������ ������ ȭ�鿡 ǥ��
	ShowWindow(_hWnd, SW_SHOWNORMAL);
	UpdateWindow(_hWnd);

	// Ŭ���̾�Ʈ�� ������ ���ϴ� ���� �ػ󵵰� �ǵ��� ��������
	SetWindowSize();

	//���⿡ ���ӿ��� ��ü ���� �� �ʱ�ȭ

	engineInstance->Initialize(_hWnd, hInstance, _screenWidth, _screenHeight);
	//KunrealEngine::ChangeScene("Main");

	//std::string testMap = "Resources/mapData/mapTest.json";
	//
	//_deserialize = new Deserialize();
	//_deserialize->Initialize(testMap);

	return S_OK;
}

void GameClient::GameClient::UpdateAll()
{
	//������ ������Ʈ �κ��� ��
	engineInstance->UpdateAll();
	//_deserialize->SetChangeSceneData();
}

LRESULT CALLBACK GameClient::GameClient::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC	hdc{};
	PAINTSTRUCT ps{};

	switch (message)
	{
		case WM_ACTIVATE:

			return 0;

		case WM_SIZE:
		{
			// â ũ�� ��ȯ
		}
		return 0;

		case WM_ENTERSIZEMOVE:
		{
			// â ũ�� ����;
		}
		return 0;

		case WM_EXITSIZEMOVE:
		{
			// â ũ�� ����;
		}
		return 0;

		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:

			return 0;

		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:

			return 0;

		case WM_MOUSEMOVE:


		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void GameClient::GameClient::GameLoop()
{
	while (true)
	{
		if (PeekMessage(&_MSG, NULL, 0, 0, PM_REMOVE))
		{
			if (_MSG.message == WM_QUIT) break;

			DispatchMessage(&_MSG);
		}
		else
		{
			RecalculateWindowSize();
			UpdateAll();
		}
	}
}

void GameClient::GameClient::Release()
{
	//�޸� ������ �κе� ������ Release�� ���⿡
	engineInstance->Release();
}
