#pragma once
#include <windows.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <string>
#include <vector>

struct ImVec4;

struct ImGuiIO;
/// <summary>
/// 메인 기믹을 분리중이다
/// 2023_10_23 배성근
/// </summary>

namespace KunrealEngine
{
    class EngineCore;
    class GameObject;
}

namespace EpicTool
{
    class ShowWindowManager;
    class ImGuiLayer;
    class ResourceWindow;
    class DataControlWindow;
    class GraphicWindow;
    class GameWindow;
    class Deserialize;
    class NavimeshEditor;

    class IWindow;

    class MainTool
    {
    public:
        MainTool();
       ~MainTool();
    public:
        HRESULT Initialize(); //  게임을 초기화 한다

        void Loop(); // 기본 루프

        void Release(); // 종료

        

    private:
        // 내부 구현 함수

        bool CreateDeviceD3D(HWND hWnd); // 이 부분은 그래픽스 오면 삭제

        void CleanupDeviceD3D();

        bool CreateSwapChain();

        void CreateRenderTarget();

        void CleanupRenderTarget();

        void CleanupSwapChain();

        void ShowWindowFunction(); // 윈도우 출력 부분을 모아둠

        void LoopImGuiRender(ImGuiIO& io);


        void UpdateAll();   // 이후 구현될 부분을 모음

        void RenderAll(ImGuiIO& io);

        void LoadData();
    private:
        // 객체선언 변수
        ResourceWindow* _resourceWindow;
        GraphicWindow* _graphicWindow;
        DataControlWindow* _saveloadWindow;
        GameWindow* _gameWindow;
        ShowWindowManager* _windowManager;
        Deserialize* _deserialize;
        NavimeshEditor* _navimeshEditor;


        // 엔진
        KunrealEngine::EngineCore* _core;

        // 프레임 확인용 변수
        //float _fps;

        // 에디터 종료 관리 변수
        bool _toolClose;

    private:

     HWND _hwnd; // 모두 임시 그래픽스 관련임
     HWND _eHwnd;

     HWND _console;

     WNDCLASSEXW wc;
     WNDCLASSEXW ewc;

	 ImVec4 _clearColor;

    private:
		int _selectedObjectIndex;  // 오브젝트리스트에서 무엇을 선택했는지 관리

    private:
        bool _loadSuccess;
        std::string _loadFilePath;

        std::vector<KunrealEngine::GameObject*> _gameObjectlist;
    };
}

