#pragma once
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp> 
#include <vector>

/// <summary>
/// ������ ������ �о ���� ������ ������ �ҷ���
/// 2023.10.19 �輺��
/// </summary>


namespace EpicTool
{
    // ó���� �ҷ����� ���Ŀ� ȣ��ɶ� �����ؼ� �ҷ��;������� �ϴ��� ó���� �ҷ����⸸
    class FileLoad
    {
    public:
        FileLoad();
        ~FileLoad();

    public:
        void Initialize(); // �ʱ�ȭ
        void Update(); // ������ ��������� �Ƹ� �̰ɷ� ����� ������ �Լ��� �ɰͰ���
    };
}

