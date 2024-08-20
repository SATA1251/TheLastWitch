#include <windows.h>
#include "GameClient.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	///여기에 전체 업데이트 부분
	GameClient::GameClient* gameClient = new GameClient::GameClient();
	gameClient->Initialize(hInstance);
	gameClient->GameLoop();
	gameClient->Release();

	return 0;
}