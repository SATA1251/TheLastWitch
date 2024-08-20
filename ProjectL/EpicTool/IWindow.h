#pragma once
#include <vector>
#include <string>

class Object;

/// <summary>
/// 윈도우의 인터페이스
/// 2023.10.19 배성근
/// </summary>

namespace EpicTool
{
    class IWindow
    {
    public:
        virtual void ShowWindow() = 0; // 실제로 창을 출력하는 함수이며 매개에 따라 다르게 선언해뒀다
        //virtual void ShowWindow(bool* p_open, std::vector<Object> object) = 0;
        virtual void ShowWindow(bool* _open, std::vector<Object>& object) = 0;
    };
}
