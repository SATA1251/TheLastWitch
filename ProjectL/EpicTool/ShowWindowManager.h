#pragma once
#include <vector>
#include "IWindow.h"

/// <summary>
/// ���̾��Ű, �ν����͸� ����
/// �ٸ� â�� ��� �������� �����
/// 2023.10.19 �輺��
/// </summary>

/// <summary>
/// ���̾��Ű�� �ν����͸� ���� �и��Ͽ���
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

        virtual void ShowWindow() override; // â�� ���

        virtual void ShowWindow(bool* _open, std::vector<Object>& object) override;

        virtual void ShowWindow(int& selectedObjectIndex); // â�� ���

        void GetGameObjetcList(std::vector<KunrealEngine::GameObject*>& instance);

    private:

        // �� �������� ��ü ����
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
