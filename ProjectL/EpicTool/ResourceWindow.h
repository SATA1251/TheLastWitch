#pragma once
#include "imgui.h"
#include "IWindow.h"
#include <vector>
#include <string>

/// <summary>
/// 게임제작에 사용할 각 리소스들을 관리할 창
/// 저장 부분이 완성되면 응용할 예정
/// 2023.10.19 배성근
/// </summary>

namespace EpicTool
{
    class ResourceWindow : public IWindow
    {
    public:
        ResourceWindow();
        ~ResourceWindow();

    public:
        virtual void ShowWindow() override; // 실제로 창을 출력하는 부분
        // virtual void ShowWindow(bool* p_open, std::vector<Object> object) override;
        virtual void ShowWindow(bool* _open, std::vector<Object>& object) override;

    };
}

