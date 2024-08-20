#pragma once
#include <vector>
#include "IWindow.h"

/// <summary>
/// 하이어라키, 인스펙터를 관리
/// 다른 창을 모두 관리할지 고민중
/// 2023.10.19 배성근
/// </summary>

/// <summary>
/// 하이어라키와 인스펙터를 따로 분리하였다
/// </summary>

namespace KunrealEngine
{
    class GameObject;
}


namespace EpicTool
{
    class InspectorWindow;
    class HierarchyWindow;
    class DebugWindow;
    class Serialize;
    enum DebugType;

    class ShowWindowManager : public IWindow
    {
    public:
        ShowWindowManager();
        ~ShowWindowManager();

    public:

        void Initialize();

        virtual void ShowWindow() override; // 창을 출력

        virtual void ShowWindow(bool* _open, std::vector<Object>& object) override;

        virtual void ShowWindow(int& selectedObjectIndex); // 창을 출력

        void GetGameObjetcList(std::vector<KunrealEngine::GameObject*>& instance);

    private:

        // 각 윈도우의 객체 설정
        InspectorWindow* _inspector;
        HierarchyWindow* _hierarchy;

        DebugWindow* _debugWindow;

        DebugType _debugHierarchyType;
        DebugType _debugInspectorType;

	private:
		std::string _deleteObject;
		std::string _deleteComponent;

    private:
        Serialize* _serialize;

    private:
        std::vector<KunrealEngine::GameObject*> _getGameObjectList;
    };
}
