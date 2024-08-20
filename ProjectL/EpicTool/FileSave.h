#pragma once
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp> 
#include <vector>
//#include <string.h>

/// <summary>
/// ������ ������ ���
/// 2023.10.19 �輺��
/// </summary>

struct Object;

namespace EpicTool
{
    class FileSave
    {
    public:
        FileSave();
        ~FileSave();

    public:
        void Initalize();

        void OnFileSave(const std::string& filePath); // ���� �Լ�

        std::string GetDirectoryFromPath(const std::string& filePath); // ������ ��� ���� �Լ�

        nlohmann::json Serialize(std::string sceneName, std::string objectName); // �ӽ� ����ȭ, ���濹�� 

    private:
        bool _makejson = false;  // ó�� json ������ üũ �ϱ� ���� ����
    };
}

