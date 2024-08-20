#pragma once
#include <vector>
#include "IWindow.h"

/// <summary>
/// �ڵ� �и� �۾���
/// ���̾��Ű �κ��� �����
/// </summary>

namespace KunrealEngine
{
    class GameObject;
}

namespace EpicTool
{
    enum DebugType;
    class DebugWindow;

    class HierarchyWindow : public IWindow // �������̽��� �ʿ����� ��εȴ�
    {
    public:
        HierarchyWindow();
        ~HierarchyWindow();

    public:

        void Initialize();

        virtual void ShowWindow() override; // â�� ���
        virtual void ShowWindow(bool* p_open, std::vector<Object>& object) override;

        void ShowWindow(int& selectedObjectIndex);  // ���̾��Ű�� �ν����Ϳ����� ���

        void CreateObject(std::string objectName, int& count);

        void DeleteObject(std::vector<KunrealEngine::GameObject*>& gameObjectlist, int& selectedObjectIndex);

        void UpdateGameObject();  // ������Ʈ�� ������ �ٲ�

        void GetDebugType(DebugType& instance);

        void GetDeleteObjectName(std::string& objectName);

		void HandleDragAndDrop(int index, int& selected, std::vector<KunrealEngine::GameObject*>& gameObjectlist);

        void CheckChildListFromUp(std::vector<KunrealEngine::GameObject*>& gameObjectlist, int& index, int& payloadIndex);

        void CheckChildListFromDown(std::vector<KunrealEngine::GameObject*>& gameObjectlist, int& index, int& payloadIndex);
   
        void CheckInDentParent(KunrealEngine::GameObject* gameObject, KunrealEngine::GameObject* previousGameObject);

        void CheckInDent(KunrealEngine::GameObject* gameObject);

		void CheckUninDentParent(KunrealEngine::GameObject* gameObject, KunrealEngine::GameObject* previousGameObject);
        
        void CheckUninDent(KunrealEngine::GameObject* gameObject);

        void GetHierarchyList(std::vector<KunrealEngine::GameObject*>& instance);

        void UpdateObjectList(int& selectedObjectIndex);

        void IndentAll(int indentLevel);

        void CopyObject(int& selectedObjectIndex);
    private:
        // �� ������Ʈ�� ����
        int _createEmptyCount;   
        int _cubeCount;
        int _sphereCount;

        // ������Ʈ ����� ������ ����
		int _draggedIndex;
		int _dropTargetIndex;

        // imgui â�� ����ϱ� ���� ����
		float _hierarchyWindowX1;
		float _hierarchyWindowY1;
		float _hierarchyWindowX2;
		float _hierarchyWindowY2;

        // ������Ʈ ����Ʈ
        std::vector<KunrealEngine::GameObject*> _gameObjectlist;

        // ������Ʈ ����Ʈ�� ������Ʈ �Ǿ����� ������
        std::vector<KunrealEngine::GameObject*> _tempgameObjectList;

        // �巡�׽� �����ϴ� ������ �ڽ� ��ġ
	private:
		DebugWindow* _debugWindow;  // ����� �׽�Ʈ�� ��ü
        DebugType _DebugType;

        std::string _deleteObjectName;

        std::string _copyObjectName;

        // �巡�׸� ������ ����
    private:
        bool _isDragged;

    private:
        int _payloadIndex;
        bool _show_Context_Menu;
        bool _indentCheck;

        bool _isListUpdatePlus;
        bool _isListUpdateDel;
    };
}

