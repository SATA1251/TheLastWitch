#pragma once
#include "IWindow.h"
#include <windows.h>


/// <summary>
/// 그래픽스 엔진에서 받은 화면을 띄울 창
/// 2023.10.19 배성근
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

        virtual void ShowWindow() override; // 실제로 창을 출력하는 함수
        // virtual void ShowWindow(bool* p_open, std::vector<Object> object) override;
        virtual void ShowWindow(bool* _open, std::vector<Object>& object) override; // 기능없음

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
