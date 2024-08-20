#pragma once
#define _SILENCE_CXX20_CISO646_REMOVED_WARNING
#include <windows.h>


/// <summary>
/// 엔진의 클라이언트
/// 프로그램의 기본 정보, 게임엔진 초기화 등
/// 
/// 추후 컨텐츠 프로그래머가 사용할 내용 추가 예정
/// 
/// [이건호]
/// </summary>

///
/// 변동사항!
/// 
/// 그래픽스 테스트용으로 그래픽스 인터페이스 헤더에 추가
/// 추가종속성에 ArkEngine.lib 추가
/// _pRenderer 멤버변수 추가
/// Initialize, Update에 렌더러 관련 함수 추가
///
/// 윤종화
///

namespace KunrealEngine
{
	class EngineCore;
}

class Deserialize;

namespace GameClient 
{
	class GameClient
	{
	public:
		GameClient();
		~GameClient();

	private:
		// 윈도 관련
		HWND _hWnd;		// 핸들윈도우
		MSG _MSG;		// 메세지

		// 화면의 위치	// 초기값은 0, 0
		int _windowPosX;
		int _windowPosY;

		// 화면의 크기	// 초기값은 1920 x 1080
		int _screenWidth;
		int _screenHeight;

		// 게임엔진 코어
		KunrealEngine::EngineCore* _core;

	public:
		// 게임 클라이언트 초기화
		HRESULT Initialize(HINSTANCE hInstance);

		// 모든 업데이트를 포함, 메세지 상태에 따라 루프
		void GameLoop();

		// 게임 클라이언트 메모리 해제
		void Release();

		// 클라이언트 창과 윈도우 사이의 크기 조절
		void SetWindowSize();
		void RecalculateWindowSize();

	private:
		// 게임엔진의 라이프사이클을 돌려줌
		void UpdateAll();

		// 윈도우 프로시저 생성
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	
	private:
		Deserialize* _deserialize;
	};
}

