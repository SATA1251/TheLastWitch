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

//지속적으로 델타타임 등 필요한 부분들을 업데이트
void KunrealEngine::TimeManager::Update()
{
	///업데이트가 돌아가면서 currentTime을 계속 업데이트
	///prevTime과 새로 업데이트한 시간의 차이를 계산해서 델타타임 도출
	///그리고 prevTime을 currentTime로 업데이트
	_currentTime = std::chrono::high_resolution_clock::now();
	_deltaTime = std::chrono::duration<float>(_currentTime - _prevTime).count() * _timeScale;
	_prevTime = _currentTime;
}

float KunrealEngine::TimeManager::GetDeltaTime()
{
	// 처음에 데이터들이 전부 로드되지 않은 동안에는 델타타임이 빠르게 돈다
	// 캐릭터가 지구를 벗어나는 일이 없도록 조정
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
