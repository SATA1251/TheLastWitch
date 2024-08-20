#pragma once
#include "IWindow.h"

/// <summary>
/// ���� ȭ���� ����� â
/// �ӽ÷� ����� �װ� ��а� ������� �ʴ´�
/// 2023.10.19 �輺��
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

        virtual void ShowWindow() override;  // ������ â�� ����ϴ� �κ�
        virtual void ShowWindow(bool* _open, std::vector<Object>& object) override;  // ���� ��� ����

        void ShowWindowClose(); // â�� ������ �κ��� �и�����
    };
}
