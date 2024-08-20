#pragma once
#include "CommonHeader.h"
#include <chrono>

/// <summary>
/// �ð��� �����ϴ� Ŭ����
/// 
/// QueryPerformanceCounter�� �̿��� ������� chrono ���̺귯���� �̿��� ����� �����
/// -> QPC�� ���� ���е��� ���������� �÷����� ���� ��� ����� �޶� �̽ļ��� ������
/// => chrono������ QPC�� ����� ���� ������ ���� �����̶� �� �� �ִ�
/// 
/// �� ������ ����ȣȯ�� �ƴϰ�, ���� ���α׷��� �ٷ�� ��쿡�� QPC�� ������ ���� �ִ�
/// 
/// �̰�ȣ
/// </summary>

namespace KunrealEngine
{
	class TimeManager
	{
	private:
		TimeManager();
		~TimeManager();

		// �̱���
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

		//Ŭ���̾�Ʈ�� �Ѱ��� �Լ���
	public:
		// ��ŸŸ���� �������ִ� �Լ�
		float GetDeltaTime();

		// Ÿ�ӽ������� �����ϴ� �Լ�	0���ϴ� �Ұ���
		void SetTimeScale(float scale);

		// �� �Լ��� ���� ������Ʈ�� ���� �ٸ� Ÿ�ӽ������� ������ �� �� ����
		float GetTimeScale();
	private:
		std::chrono::high_resolution_clock::time_point _startTime;		// Ÿ�̸��� ���۽ð�
		std::chrono::high_resolution_clock::time_point _prevTime;		//
		std::chrono::high_resolution_clock::time_point _currentTime;	// ��ŸŸ���� ����ϱ� ����, �ð� �� ����� ���� ������

		float _deltaTime;			// ��ŸŸ��
		float _timeScale;			// Ÿ�ӽ����� Ȥ�� ������Ʈ�� ������ �帣�� �ð��� �ٸ��� �ְ� ���� ���� ������	
	};
}

