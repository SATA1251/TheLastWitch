#include "KunrealAPI.h"
#include "SceneManager.h"
#include "TimeManager.h"

KunrealEngine::EngineCore* KunrealEngine::CreateEngine()
{
	EngineCore* engineInstance = new EngineCore();

	return engineInstance;
}

KunrealEngine::Scene* KunrealEngine::CreateScene(std::string sceneName)
{
	return KunrealEngine::SceneManager::GetInstance().CreateScene(sceneName);
}

KunrealEngine::Scene* KunrealEngine::GetScene(std::string sceneName)
{
	return KunrealEngine::SceneManager::GetInstance().GetScene(sceneName);
}

KunrealEngine::Scene* KunrealEngine::GetCurrentScene()
{
	return KunrealEngine::SceneManager::GetInstance().GetCurrentScene();
}

void KunrealEngine::ChangeScene(std::string sceneName)
{
	KunrealEngine::SceneManager::GetInstance().ChangeScene(sceneName);
}

std::vector<KunrealEngine::Scene*>* KunrealEngine::GetSceneLists()
{
	return KunrealEngine::SceneManager::GetInstance().GetSceneList();
}

float KunrealEngine::GetDeltaTime()
{
	return KunrealEngine::TimeManager::GetInstance().GetDeltaTime();
}

float KunrealEngine::GetTimeScale()
{
	return KunrealEngine::TimeManager::GetInstance().GetTimeScale();
}

void KunrealEngine::SetTimeScale(float scale)
{
	return KunrealEngine::TimeManager::GetInstance().SetTimeScale(scale);
}
