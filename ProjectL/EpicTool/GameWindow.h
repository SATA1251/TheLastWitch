#pragma once
#include "IWindow.h"

/// <summary>
/// 게임 화면을 출력할 창
/// 임시로 만들어 뒀고 당분간 사용하진 않는다
/// 2023.10.19 배성근
/// </summary>
///

namespace EpicTool
{
    class GameWindow : public IWindow
    {
    public:
        GameWindow();
        ~GameWindow();

    public:

        virtual void ShowWindow() override;  // 실제로 창을 출력하는 부분
        virtual void ShowWindow(bool* _open, std::vector<Object>& object) override;  // 아직 기능 없음

        void ShowWindowClose(); // 창이 끝나는 부분을 분리했음
    };
}
