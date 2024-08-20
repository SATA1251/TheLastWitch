#pragma once
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp> 
#include <vector>
//#include <string.h>

/// <summary>
/// 파일의 저장을 담당
/// 2023.10.19 배성근
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

        void OnFileSave(const std::string& filePath); // 저장 함수

        std::string GetDirectoryFromPath(const std::string& filePath); // 파일의 경로 설정 함수

        nlohmann::json Serialize(std::string sceneName, std::string objectName); // 임시 직렬화, 변경예정 

    private:
        bool _makejson = false;  // 처음 json 생성을 체크 하기 위한 변수
    };
}

