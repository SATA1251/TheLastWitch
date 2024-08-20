#include "MainTool.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "ShowWindowManager.h"
#include "ResourceWindow.h"
#include <d3d11.h>
#include <dxgi1_2.h>
#include <tchar.h>
#include <Windows.h>
#include "imgui_internal.h"
#include "DataControlWindow.h"
#include "GraphicWindow.h"
#include "GameWindow.h"
#include "NavimeshEditor.h"

#include "IWindow.h"
#include "FileLoad.h"
#include "../ArkEngine/ArkDevice.h"
#include "EngineCore.h"
#include "KunrealAPI.h"
#include "Deserialize.h"

#include <nfd.h>
#include <ImGuizmo.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 메시지 핸들러 (윈도 콜백)
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK EditorWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 임시 d3d를 담을 변수들, 옮기는 중이라 컨벤션 안맞음

static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;


EpicTool::MainTool::MainTool()
    :_resourceWindow(nullptr), _graphicWindow(nullptr), _saveloadWindow(nullptr), _gameWindow(nullptr), _windowManager(nullptr), _toolClose(false), _hwnd(nullptr),
    wc(), _core(nullptr), _console(), _selectedObjectIndex(-1), _loadSuccess(true)
{

}

EpicTool::MainTool::~MainTool()
{

}

HRESULT EpicTool::MainTool::Initialize()
{
	//_console = GetConsoleWindow();
	//ShowWindow(_console, SW_SHOW);

     wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    _hwnd = ::CreateWindowW(wc.lpszClassName, L"Game!", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);
    
    ewc = { sizeof(ewc), CS_CLASSDC, EditorWndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"Kunreal Editor", nullptr };
    ::RegisterClassExW(&ewc);
    _eHwnd = ::CreateWindowW(ewc.lpszClassName, L"Kunreal Editor", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, ewc.hInstance, nullptr);

	// 엔진 생성
    int screenWidth = 1920;
    int screenHeight = 1080;

	_core = KunrealEngine::CreateEngine();
	_core->Initialize(_hwnd, wc.hInstance, screenWidth, screenHeight);



    // Show the window
	::ShowWindow(_hwnd, SW_SHOW);
	::UpdateWindow(_hwnd);

	::ShowWindow(_eHwnd, SW_SHOW);
	::UpdateWindow(_eHwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform W

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    KunrealEngine::CreateScene("Main");

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    // 일단 임시로 여기다 넘김 더 필요한거있으면 바로 말해줘
	g_pd3dDevice = static_cast<ID3D11Device*>(GRAPHICS->GetDevice());
    g_pd3dDeviceContext = static_cast<ID3D11DeviceContext*>(GRAPHICS->GetDeviceContext());
   // g_mainRenderTargetView = static_cast<ID3D11RenderTargetView*>(_core->GetRenderTargetView());


	CreateSwapChain();

    ImGui_ImplWin32_Init(_eHwnd);

    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);



    _windowManager = new ShowWindowManager;
    _windowManager->Initialize();

    _resourceWindow = new ResourceWindow();
	_graphicWindow = new GraphicWindow(screenWidth, screenHeight);

	_saveloadWindow = new DataControlWindow();
	_saveloadWindow->Initialize();

    _gameWindow = new GameWindow();   

    _navimeshEditor = new NavimeshEditor();
    _navimeshEditor->Initialize();


    
    return 1;
}
bool asdftur = true;
void EpicTool::MainTool::Loop()
{
   

    ImGuiIO& io = ImGui::GetIO(); (void)io;  // 해당 부분이 도킹과 리사이즈에 반드시 필요함
    while (!_toolClose)
    {       

        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                _toolClose = true;
        }
        if (_toolClose)
        {
            break;
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();

        ImGui::NewFrame();
        IMGUIZMO_NAMESPACE::BeginFrame();

		//if (asdftur)
		//{
		//	float targetPos[3] = { 0.0f, -15.0f, 0.0f };
		//	auto playerobjectTransform = KunrealEngine::GetCurrentScene()->GetGameObject("Player")->GetComponent<KunrealEngine::Transform>();
		//	auto testObject = KunrealEngine::GetCurrentScene()->GetGameObject("testCamera");
		//	KunrealEngine::GetCurrentScene()->GetGameObject("testCamera")->GetComponent<KunrealEngine::Camera>()->SetTargetPosition(targetPos[0] + playerobjectTransform->GetPosition().x
		//		, targetPos[1] + playerobjectTransform->GetPosition().y, targetPos[2] + playerobjectTransform->GetPosition().z);
		//	auto position = testObject->GetComponent<KunrealEngine::Camera>()->GetTargetPosition();
		//	int adfs = 0;
		//
		//	asdftur = false;
		//}

        UpdateAll();

       // KunrealEngine::GetCurrentScene()->GetGameObject("testCamera")->GetComponent<KunrealEngine::Camera>()->SetTargetPosition(
       //     KunrealEngine::GetCurrentScene()->GetGameObject("testCamera")->GetComponent<KunrealEngine::Camera>()->GetTargetPosition().x,
       //     KunrealEngine::GetCurrentScene()->GetGameObject("testCamera")->GetComponent<KunrealEngine::Camera>()->GetTargetPosition().y + 0.001f,
       //     KunrealEngine::GetCurrentScene()->GetGameObject("testCamera")->GetComponent<KunrealEngine::Camera>()->GetTargetPosition().z
       // );

        RenderAll(io);
    }
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    ::DestroyWindow(_hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

}

void EpicTool::MainTool::Release()
{
    _core->Release();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_SIZE:
            if (wParam == SIZE_MINIMIZED)
            {
                return 0;
            }
            g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize // 이 부분이 리사이징 중요부분
            g_ResizeHeight = (UINT)HIWORD(lParam);
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            { 
                return 0;
            }
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
        case WM_DPICHANGED:
            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
            {
                const RECT* suggested_rect = (RECT*)lParam;
                ::SetWindowPos(hWnd, nullptr, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
            }
            break;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK EditorWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    {
		return true;
    }

	switch (msg)
	{
		case WM_SIZE:
            if (wParam == SIZE_MINIMIZED)
            {
				return 0;
            }
			g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize // 이 부분이 리사이징 중요부분
			g_ResizeHeight = (UINT)HIWORD(lParam);
			return 0;
		case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            {
                return 0;
            }
			break;
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
		case WM_DPICHANGED:
			if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
			{
				const RECT* suggested_rect = (RECT*)lParam;
				::SetWindowPos(hWnd, nullptr, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
			}
			break;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

bool EpicTool::MainTool::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC sd{};
 	sd.BufferCount = 2;
 	sd.BufferDesc.Width = 0;
 	sd.BufferDesc.Height = 0;
 	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
 	sd.BufferDesc.RefreshRate.Numerator = 60;
 	sd.BufferDesc.RefreshRate.Denominator = 1;
 	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
 	sd.OutputWindow = _eHwnd;
 	sd.SampleDesc.Count = 1;
 	sd.SampleDesc.Quality = 0;
 	sd.Windowed = TRUE;

		// IDXGIDevice : 이미지 데이터를 생성하는 DXGI 객체에 대한 파생 클래스를 구현
	IDXGIDevice* p_dxgiDevice = 0;
	// queryinterface의 첫번째 매개변수 : COM 객체에서 내가 요청한 IID를 넘겨줌
	// queryinterface의 두번째 매개변수 : COM 객체가 가지고 있는 인터페이스를 이 변수에 담아줌
    auto result1 =  g_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&p_dxgiDevice);

	// IDXGIAdapter : 디스플레이 서브시스템 (하나 이상의 GPU, DAC 및 비디오 메모리 포함)을 나타냄
	IDXGIAdapter* p_dxgiAdapter = 0;
    auto result2 = p_dxgiDevice->GetAdapter(&p_dxgiAdapter);									// msdn에서는 이 방법 사용
	//p_dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&p_dxgiAdapter);	// 책에서는 이 방법 사용

	// IDXGIFactory : 전체 화면 전환을 처리하는 DXGI 객체를 생성하는 매서드를 구현
	IDXGIFactory* p_dxgiFactory = 0;
    auto result3 = p_dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&p_dxgiFactory);

	// CreateSwapChain (ID3D11Device를 가리키는 포인터, 교환사슬 서술 구조체를 가리키는 포인터, 생성된 교환사슬 인터페이스를 돌려줄 변수)
    auto result4 = p_dxgiFactory->CreateSwapChain(g_pd3dDevice, &sd, &g_pSwapChain);

	// 획득한 COM 인터페이스들을 다 사용했으므로 해제
	p_dxgiFactory->Release();
	p_dxgiAdapter->Release();
	p_dxgiDevice->Release();

    CreateRenderTarget();

    return true;
}

void EpicTool::MainTool::CreateRenderTarget()
{
	// ID3D11Texture2D : 구조화된 메모리인 텍셀 데이터를 관리
    // 텍스처는 파이프라인에 직접 바인딩할 수 없으며 뷰를 생성하고 바인딩 해야 함
	ID3D11Texture2D* backBuffer;

	// 첫 매개변수는 후면 버퍼의 색인 -> 후면 버퍼가 하나이므로 0을 지정
	// 둘째 매개변수는 버퍼의 인터페이스 형식을 지정 -> 일반적으로는 2차원 텍스트를 위한 ID3D11Texture2D가 사용됨
	// 셋째 매개변수는 후면 버퍼를 가리키는 포인터를 이 변수에 돌려줌
    g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**> (&backBuffer));

	// 첫 매개변수는 렌더 대상으로 사용할 자원
	// 둘째 매개변수는 d3d11_RENDER_TARGET_VIEW_DESC 구조체를 가리키는 포인터, 무형식 자원이 아닐 시 이 매개변수에 NULL값을 지정해도 됨
	// 셋째 매개변수는 렌더 대상 뷰를 돌려줌
	if (backBuffer != nullptr)
	{
        g_pd3dDevice->CreateRenderTargetView(backBuffer, 0, &g_mainRenderTargetView);
	}
	// 획득한 COM 인터페이스들을 다 사용했으므로 해제
	backBuffer->Release();
}

void EpicTool::MainTool::CleanupRenderTarget()
{
    if (g_mainRenderTargetView)
    { 
        g_mainRenderTargetView->Release();
        g_mainRenderTargetView = nullptr;
    }
}

void EpicTool::MainTool::CleanupSwapChain()
{
    CleanupRenderTarget();
    if (g_pSwapChain)
    { 
        g_pSwapChain->Release(); g_pSwapChain = nullptr;
    }
}

void EpicTool::MainTool::ShowWindowFunction()
{
    auto textureSRV = GRAPHICS->GetRenderingImage();


    _core->SetEditorMousePos(_graphicWindow->GetMousePosition());

    _saveloadWindow->ShowWindow(_toolClose, _selectedObjectIndex);

	_windowManager->ShowWindow(_selectedObjectIndex);

    _windowManager->GetGameObjetcList(_gameObjectlist);

	_graphicWindow->ShowWindow(textureSRV, _selectedObjectIndex);
	_graphicWindow->SetGameObjectList(_gameObjectlist);

	_resourceWindow->ShowWindow();

	_gameWindow->ShowWindow();

    _navimeshEditor->ShowWindow();

  
}

void EpicTool::MainTool::LoopImGuiRender(ImGuiIO& io)
{
    // Rendering
    ImGui::Render();
    const float clear_color_with_alpha[4] = { _clearColor.x * _clearColor.w, _clearColor.y * _clearColor.w, _clearColor.z * _clearColor.w, _clearColor.w };
	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
	g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    g_pSwapChain->Present(1, 0); // Present with vsync
}

void EpicTool::MainTool::UpdateAll()
{
	
     _core->UpdateAll();

	 ShowWindowFunction();

}

void EpicTool::MainTool::RenderAll(ImGuiIO& io)
{
    LoopImGuiRender(io);
    _core->Render();  
}

void EpicTool::MainTool::LoadData()
{
    while (_loadSuccess)
    {
    	nfdchar_t* outPath = NULL;

		//std::string folderPath = std::filesystem::current_path().string();
		//
		//std::string filePath = folderPath + "/Resources/MapObj/";

    	nfdresult_t result = NFD_OpenDialog("json", NULL, &outPath);


    
    	if (result == NFD_OKAY) 
        {
    		if (outPath)
            {
    			std::string chosenPath = outPath;
    
    			_loadFilePath = chosenPath;
    
    			free(outPath);

    			Deserialize* _deserialize = new Deserialize();
    			_deserialize->Initialize(_loadFilePath);
    			_loadSuccess = false;
    		}
		}
		else
        {
			break;
		}
    }
    
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);