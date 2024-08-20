#pragma once
#include <vector>
#include "IWindow.h"

/// <summary>
/// 코드 분리 작업중
/// 하이어라키 부분을 담당함
/// </summary>

namespace KunrealEngine
{
    class GameObject;
}

namespace EpicTool
{
    enum DebugType;
    class DebugWindow;

    class HierarchyWindow : public IWindow // 인터페이스가 필요할지 고민된다
    {
    public:
        HierarchyWindow();
        ~HierarchyWindow();

    public:

        void Initialize();

        virtual void ShowWindow() override; // 창을 출력
        virtual void ShowWindow(bool* p_open, std::vector<Object>& object) override;

        void ShowWindow(int& selectedObjectIndex);  // 하이어라키와 인스펙터에서만 사용

        void CreateObject(std::string objectName, int& count);

        void DeleteObject(std::vector<KunrealEngine::GameObject*>& gameObjectlist, int& selectedObjectIndex);

        void UpdateGameObject();  // 오브젝트의 순서를 바꿈

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
        // 각 오브젝트의 갯수
        int _createEmptyCount;   
        int _cubeCount;
        int _sphereCount;

        // 오브젝트 드롭을 관리할 변수
		int _draggedIndex;
		int _dropTargetIndex;

        // imgui 창을 계산하기 위한 변수
		float _hierarchyWindowX1;
		float _hierarchyWindowY1;
		float _hierarchyWindowX2;
		float _hierarchyWindowY2;

        // 오브젝트 리스트
        std::vector<KunrealEngine::GameObject*> _gameObjectlist;

        // 오브젝트 리스트가 업데이트 되었을때 보존용
        std::vector<KunrealEngine::GameObject*> _tempgameObjectList;

        // 드래그시 출현하는 반투명 박스 위치
	private:
		DebugWindow* _debugWindow;  // 디버그 테스트용 객체
        DebugType _DebugType;

        std::string _deleteObjectName;

        std::string _copyObjectName;

        // 드래그를 관리할 변수
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

