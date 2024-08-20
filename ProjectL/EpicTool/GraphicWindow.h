#pragma once
#include "IWindow.h"
#include <windows.h>


/// <summary>
/// �׷��Ƚ� �������� ���� ȭ���� ��� â
/// 2023.10.19 �輺��
/// </summary>
/// 

namespace KunrealEngine
{
	class GameObject;
}

namespace EpicTool
{
    class GraphicWindow : public IWindow
    {
    public:
        GraphicWindow();
        GraphicWindow(int screenWidth, int screenHeight);
        ~GraphicWindow();

    public:

        virtual void ShowWindow() override; // ������ â�� ����ϴ� �Լ�
        // virtual void ShowWindow(bool* p_open, std::vector<Object> object) override;
        virtual void ShowWindow(bool* _open, std::vector<Object>& object) override; // ��ɾ���

        void ShowWindow(void* _texture, int& selectedObjectIndex);

        POINT GetMousePosition();

        void SetGameObjectList(std::vector<KunrealEngine::GameObject*>& intance);

    private:
        std::vector<KunrealEngine::GameObject*> _gameObjectlist;

        float test[9];

        int _mosX;
        int	_mosY;
        int _screenWidth;
        int _screenHeight;

        int _imageSizeX;
        int _imageSizeY;

        ImVec2 _imguiWindowPosition;
        ImVec2 _imguiWindowSize;

        bool _isTestRotation;

		float _deltaX = 0.0f;
		float _deltaY = 0.0f;
		float _deltaZ = 0.0f;

        float _degree = 0;

        float _testTransform[9];

        bool _first;
    };
}
