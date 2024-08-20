#pragma once
#include <vector>
#include <string>

class Object;

/// <summary>
/// �������� �������̽�
/// 2023.10.19 �輺��
/// </summary>

namespace EpicTool
{
    class IWindow
    {
    public:
        virtual void ShowWindow() = 0; // ������ â�� ����ϴ� �Լ��̸� �Ű��� ���� �ٸ��� �����ص״�
        //virtual void ShowWindow(bool* p_open, std::vector<Object> object) = 0;
        virtual void ShowWindow(bool* _open, std::vector<Object>& object) = 0;
    };
}
