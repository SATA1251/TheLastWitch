#pragma once
#include "imgui.h"
#include "IWindow.h"
#include <vector>
#include <string>

/// <summary>
/// �������ۿ� ����� �� ���ҽ����� ������ â
/// ���� �κ��� �ϼ��Ǹ� ������ ����
/// 2023.10.19 �輺��
/// </summary>

namespace EpicTool
{
    class ResourceWindow : public IWindow
    {
    public:
        ResourceWindow();
        ~ResourceWindow();

    public:
        virtual void ShowWindow() override; // ������ â�� ����ϴ� �κ�
        // virtual void ShowWindow(bool* p_open, std::vector<Object> object) override;
        virtual void ShowWindow(bool* _open, std::vector<Object>& object) override;

    };
}

