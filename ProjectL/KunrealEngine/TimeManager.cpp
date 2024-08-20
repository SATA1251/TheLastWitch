#include "TimeManager.h"

KunrealEngine::TimeManager::TimeManager()
	:_startTime(std::chrono::high_resolution_clock::now()),
	_prevTime(std::chrono::high_resolution_clock::now()),
	_deltaTime(0.0),
	_timeScale(1.0f)
{

}

KunrealEngine::TimeManager::~TimeManager()
{

}

void KunrealEngine::TimeManager::Initialize()
{

}

void KunrealEngine::TimeManager::Release()
{

}

//���������� ��ŸŸ�� �� �ʿ��� �κе��� ������Ʈ
void KunrealEngine::TimeManager::Update()
{
	///������Ʈ�� ���ư��鼭 currentTime�� ��� ������Ʈ
	///prevTime�� ���� ������Ʈ�� �ð��� ���̸� ����ؼ� ��ŸŸ�� ����
	///�׸��� prevTime�� currentTime�� ������Ʈ
	_currentTime = std::chrono::high_resolution_clock::now();
	_deltaTime = std::chrono::duration<float>(_currentTime - _prevTime).count() * _timeScale;
	_prevTime = _currentTime;
}

float KunrealEngine::TimeManager::GetDeltaTime()
{
	// ó���� �����͵��� ���� �ε���� ���� ���ȿ��� ��ŸŸ���� ������ ����
	// ĳ���Ͱ� ������ ����� ���� ������ ����
	if (_deltaTime > 0.1f)
	{
		_deltaTime = 0.001f;
	}

	return _deltaTime;
}

void KunrealEngine::TimeManager::SetTimeScale(float scale)
{
	if (scale <= 0)
	{
		return;
	}

	_timeScale = scale;
}

float KunrealEngine::TimeManager::GetTimeScale()
{
	return _timeScale;
}
