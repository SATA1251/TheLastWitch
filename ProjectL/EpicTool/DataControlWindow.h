#pragma once
#include "IWindow.h"
#include <vector>
#include <filesystem>
#include <nfd.h>

/// <summary>
/// ���� ���� ���� ����â�� ���
/// ����� �Բ� �ٸ� â�� ������ �����ϰ� �� ��
/// 2023.10.19 �輺��
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

        virtual void ShowWindow() override;                                // ������ â�� ����ϴ� �Լ�

        virtual void ShowWindow(bool* _open, std::vector<Object>& object) override;   // ������ü�� �ٸ� �Լ����� ����

        void ShowWindow(bool& close, int& selectedObjectIndex);
        void Initialize(); // �ʱ�ȭ, ���� �̱���

        // ������ ���
        void SaveToFile(const std::string& filePath);
        void LoadToFile(const std::string& filePath);
        void ExportToObj(const std::string& filename); // obj ���� ����� ���� ���ؽ��� �ε��� ���۸� 

        void ChangeScene(int& selectedObjectIndex);

    private:
        FileSave* _fileSave; // ���� ����� ��ü
        Serialize* _serialize; // ���� ����� ��ü
        std::string _saveFilePath; // ������ ��� ���� , �ʱ�ȭ �������ƺ�
        std::string _loadFilePath; // ������ ��� ���� , �ʱ�ȭ �������ƺ�

        ExportObj* _exportObj;

        // �� ����â�� �����ϴ� ������
        bool _show_save_editor;
        bool _save_Scene;
        bool _new_Scene;
        bool _opt_padding;

        // scene�� ������ ������
        bool _scene1First;
        bool _scene2First;
        bool _scene3First;
        bool _scene4First;
        bool _scene5First;

        std::filesystem::path _executablePath;

        std::string _exportObjName;
    };
}
