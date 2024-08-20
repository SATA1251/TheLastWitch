#pragma once
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp> 
#include <vector>

/// <summary>
/// 기존의 파일을 읽어서 툴에 덮어씌우는 것으로 불러옴
/// 2023.10.19 배성근
/// </summary>


namespace EpicTool
{
    // 처음에 불러오고 이후에 호출될때 선택해서 불러와야하지만 일단은 처음에 불러오기만
    class FileLoad
    {
    public:
        FileLoad();
        ~FileLoad();

    public:
        void Initialize(); // 초기화
        void Update(); // 지금은 비어있지만 아마 이걸로 덮어쓰는 형식의 함수가 될것같다
    };
}

