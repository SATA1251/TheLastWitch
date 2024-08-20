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

void EpicTool::GameWindow::ShowWindowClose() // 과정을 분리 했는데 필요 없을 시 제거함
{
    ImGui::End();
}
