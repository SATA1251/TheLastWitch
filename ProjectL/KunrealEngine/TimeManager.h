#pragma once
#include "CommonHeader.h"
#include <chrono>

/// <summary>
/// 시간을 관리하는 클래스
/// 
/// QueryPerformanceCounter를 이용한 방법보다 chrono 라이브러리를 이용한 방법이 권장됨
/// -> QPC는 높은 정밀도를 제공하지만 플랫폼에 따라 사용 방법이 달라 이식성이 떨어짐
/// => chrono에서도 QPC를 사용할 수도 있으니 상위 버전이라 볼 수 있다
/// 
/// 단 무조건 상위호환은 아니고, 구식 프로그램을 다루는 경우에는 QPC가 유리할 수도 있다
/// 
/// 이건호
/// </summary>

namespace KunrealEngine
{
	class TimeManager
	{
	private:
		TimeManager();
		~TimeManager();

		// 싱글톤
	public:
		static TimeManager& GetInstance()
		{
			static TimeManager _instance;
			return _instance;
		}

	public:
		void Initialize();
		void Release();
		void Update();

		//클라이언트에 넘겨줄 함수들
	public:
		// 델타타임을 리턴해주는 함수
		float GetDeltaTime();

		// 타임스케일을 조정하는 함수	0이하는 불가능
		void SetTimeScale(float scale);

		// 이 함수를 통해 오브젝트가 각기 다른 타임스케일을 가지게 될 수 있음
		float GetTimeScale();
	private:
		std::chrono::high_resolution_clock::time_point _startTime;		// 타이머의 시작시간
		std::chrono::high_resolution_clock::time_point _prevTime;		//
		std::chrono::high_resolution_clock::time_point _currentTime;	// 델타타임을 계산하기 위해, 시간 차 계산을 위한 변수들

		float _deltaTime;			// 델타타임
		float _timeScale;			// 타임스케일 혹시 오브젝트나 씬마다 흐르는 시간을 다르게 주고 싶을 수도 있으니	
	};
}

