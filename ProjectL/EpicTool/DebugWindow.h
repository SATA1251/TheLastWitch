#pragma once
#include <vector>
#include <string>
#include "IWindow.h"

namespace KunrealEngine
{
	class GameObject;
}

namespace EpicTool
{
	enum DebugType;

	class DebugWindow : public IWindow
	{
	public:
		DebugWindow();
		~DebugWindow();

	public:
		virtual void ShowWindow() override;  // 실제로 창을 출력하는 부분
		virtual void ShowWindow(bool* _open, std::vector<Object>& object) override;  // 아직 기능 없음

		void Initialize();

		void ShowWindow(int& selectedObjectIndex);

		DebugType GetDebugType();

		void SetHierarchyDebugType(DebugType debugType);

		void SetInspectorDebugType(DebugType debugType);

		void ShowDebugWindow();

		void ShowHierarchyDebug();

		void ShowInspectorDebug();


		void GetDeleteObjectName(std::string& objectName);

		void GetDeleteComponentName(std::string& objectName);

		void ShowDebugLog(const char* text);

	private:
		DebugType _typeHierarchy;
		DebugType _typeInspector;

		std::vector<std::string> _debugTypeList;

		bool _isdebug;
	private:

		std::vector<KunrealEngine::GameObject*> _gameObjectlist;

		int _selectedObjectIndex;

		std::string _deleteObjectName;

		std::string _deleteComponentName;

	};
}
