#include "GameWindow.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "KunrealAPI.h"

EpicTool::GameWindow::GameWindow()
{

}

EpicTool::GameWindow::~GameWindow()
{

}

void EpicTool::GameWindow::ShowWindow()
{

   ImGui::Begin("GameWindow");

   ImDrawList* drawList = ImGui::GetWindowDrawList();

   ImGui::End();
    


}

void EpicTool::GameWindow::ShowWindow(bool* _open, std::vector<Object>& object)
{

}

void EpicTool::GameWindow::ShowWindowClose() // ������ �и� �ߴµ� �ʿ� ���� �� ������
{
    ImGui::End();
}
