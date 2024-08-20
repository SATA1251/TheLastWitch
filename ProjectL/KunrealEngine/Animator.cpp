#include "Animator.h"
#include "TimeManager.h"

KunrealEngine::Animator::Animator(GInterface::GraphicsRenderable* meshRenderable)
	:_mesh(meshRenderable)
{

}

KunrealEngine::Animator::~Animator()
{

}

void KunrealEngine::Animator::Initialize()
{
	
}

void KunrealEngine::Animator::Release()
{
	
}

void KunrealEngine::Animator::FixedUpdate()
{
	
}

void KunrealEngine::Animator::Update()
{
	
}

void KunrealEngine::Animator::LateUpdate()
{
	
}

void KunrealEngine::Animator::OnTriggerEnter()
{
	
}

void KunrealEngine::Animator::OnTriggerStay()
{
	
}

void KunrealEngine::Animator::OnTriggerExit()
{
	
}

void KunrealEngine::Animator::SetActive(bool active)
{
	
}

bool KunrealEngine::Animator::Play(int index, float speed /*= 10.0f*/, bool isLoop /*= false*/)
{
	auto isPlaying = _mesh->PlayAnimation(speed, TimeManager::GetInstance().GetDeltaTime(), index, isLoop);
	
	return isPlaying;
}

bool KunrealEngine::Animator::Play(std::string clipName, float speed /*= 10.0f*/, bool isLoop /*= false*/)
{
	auto isPlaying = _mesh->PlayAnimation(speed, TimeManager::GetInstance().GetDeltaTime(), clipName, isLoop);
	
	return isPlaying;
}

void KunrealEngine::Animator::Stop()
{
	_mesh->StopAnimation();
}

void KunrealEngine::Animator::Pause()
{
	_mesh->PauseAnimation();
}

void KunrealEngine::Animator::Replay()
{
	_mesh->ReplayAnimation();
}

float KunrealEngine::Animator::GetCurrentFrame()
{
	return _mesh->GetCurrentFrame();
}


void KunrealEngine::Animator::SetCurrentFrame(int frame)
{
	return _mesh->SetCurrentFrame(frame);
}

float KunrealEngine::Animator::GetMaxFrame()
{
	return _mesh->GetMaxFrame();
}

const std::vector<std::string>& KunrealEngine::Animator::GetClipNames()
{
	return _mesh->GetClipNames();
}

const std::string& KunrealEngine::Animator::GetNowAnimationName()
{
	return _mesh->GetNowAnimationName();
}

bool KunrealEngine::Animator::GetIsAnimationPlaying()
{
	return _mesh->GetIsPlaying();
}
