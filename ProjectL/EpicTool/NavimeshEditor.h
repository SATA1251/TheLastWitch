#pragma once
#include "IWindow.h"


namespace KunrealEngine
{
	class Navigation;
}

namespace GInterface
{
	class GraphicsDebug;
}

struct ImDrawList;
struct ImVec2;

namespace EpicTool
{

	class NavimeshEditor : public IWindow
	{
	public:
		NavimeshEditor();
		~NavimeshEditor();

		virtual void ShowWindow() override; // ������ â�� ����ϴ� �Լ�
		// virtual void ShowWindow(bool* p_open, std::vector<Object> object) override;
		virtual void ShowWindow(bool* _open, std::vector<Object>& object) override; // ��ɾ���

		void Initialize();

	private:
		void DrawCylinder(ImDrawList* drawList, ImVec2 windowPos, ImVec2 windowSize, float centerX, float centerY, float radius, float height);
		void UnDrawAll();
	private:

		int _naviIndex;

		float _agentHeight;
		float _agentRadius;
		float _agentMaxClimb;
		float _agentMaxSlope;

		KunrealEngine::Navigation* _navimeshEditor;

		std::vector<GInterface::GraphicsDebug*> _navmeshpolys;


	private:
		std::string _filePath;
		std::string _fileNameStr;

		std::string _selectedFileName;
		std::string _selectedObjName;

		std::vector<std::string> _fileNameList;

		std::vector<std::string> _objList;
	};
}
