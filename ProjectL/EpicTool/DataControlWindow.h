#pragma once
#include "IWindow.h"
#include <vector>
#include <filesystem>
#include <nfd.h>

/// <summary>
/// 저장 등의 툴의 도구창을 출력
/// 저장과 함께 다른 창도 관리가 가능하게 할 것
/// 2023.10.19 배성근
/// </summary>

class FileSave;


namespace EpicTool
{
    class Serialize;
    class ExportObj;

    class DataControlWindow : public IWindow
    {
    public:
        DataControlWindow();
        ~DataControlWindow();

    public:

        virtual void ShowWindow() override;                                // 실제로 창을 출력하는 함수

        virtual void ShowWindow(bool* _open, std::vector<Object>& object) override;   // 저장자체는 다른 함수에서 하자

        void ShowWindow(bool& close, int& selectedObjectIndex);
        void Initialize(); // 초기화, 아직 미구현

        // 저장을 담당
        void SaveToFile(const std::string& filePath);
        void LoadToFile(const std::string& filePath);
        void ExportToObj(const std::string& filename); // obj 파일 출력을 위해 버텍스와 인덱스 버퍼를 

        void ChangeScene(int& selectedObjectIndex);

    private:
        FileSave* _fileSave; // 파일 저장용 객체
        Serialize* _serialize; // 파일 저장용 객체
        std::string _saveFilePath; // 파일의 경로 저장 , 초기화 하지말아봐
        std::string _loadFilePath; // 파일의 경로 저장 , 초기화 하지말아봐

        ExportObj* _exportObj;

        // 각 도구창을 관리하는 변수들
        bool _show_save_editor;
        bool _save_Scene;
        bool _new_Scene;
        bool _opt_padding;

        // scene을 관리할 변수들
        bool _scene1First;
        bool _scene2First;
        bool _scene3First;
        bool _scene4First;
        bool _scene5First;

        std::filesystem::path _executablePath;

        std::string _exportObjName;
    };
}
