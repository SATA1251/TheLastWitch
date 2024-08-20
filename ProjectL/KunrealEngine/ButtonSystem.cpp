#include "ButtonSystem.h"
#include "InputSystem.h"
#include "ImageRenderer.h"
#include "SoundPlayer.h"

KunrealEngine::ButtonSystem::ButtonSystem()
	: _image(nullptr)
{
}

KunrealEngine::ButtonSystem::~ButtonSystem()
{
}

void KunrealEngine::ButtonSystem::Initialize()
{
	// 사운드플레이어 컴포넌트 생성
	_soundComp = GetOwner()->AddComponent<SoundPlayer>();
	// SOUNDINFO를 만든다. 경로/3D사운드 여부/Loop 여부/ 음량(기본값 100)
	// 반환값 int는 그 소리가 저장된 인덱스값
	_soundclick = _soundComp->CreateSoundInfo("Resources/Sound/Click.wav", false, false);
	// 그리고 소리를 생성. 변수로 인덱스 값을 넣어준다.
	// 다음 변수는 이게 bgm(0)인지 sfx(1)인지 구분용도
	_soundComp->CreateSound(_soundclick, 1);
}

void KunrealEngine::ButtonSystem::Release()
{
}

void KunrealEngine::ButtonSystem::FixedUpdate()
{
}

void KunrealEngine::ButtonSystem::Update()
{
	bool ispick = ispick = _image->IsPicked(InputSystem::GetInstance()->GetEditorMousePos().x, InputSystem::GetInstance()->GetEditorMousePos().y);;
	//static bool ispick = false;

	if (_focusimage != nullptr)
	{
		if (ispick && !_focused)
		{
			_focused = true;
			_focusimage->GetOwner()->SetActive(true);
		}

		else if(!ispick && _focused)
		{
			_focused = false;
			_focusimage->GetOwner()->SetActive(false);
		}

		else if (!_focused && _focusimage->GetOwner()->GetActivated())
		{
			_focusimage->GetOwner()->SetActive(false);
		}
	}

	if (InputSystem::GetInstance()->MouseButtonUp(0))
	{
		if (ispick)
		{
			// 플레이 하고자 하는 위치에서 함수 실행.
			// update문에 들어가도 사운드가 종료될때까지 새로 재생하지 않음
			_soundComp->Play(_soundclick);
			_script();
		}
	}
}

void KunrealEngine::ButtonSystem::LateUpdate()
{
}

void KunrealEngine::ButtonSystem::OnTriggerEnter()
{
}

void KunrealEngine::ButtonSystem::OnTriggerStay()
{
}

void KunrealEngine::ButtonSystem::OnTriggerExit()
{
}

void KunrealEngine::ButtonSystem::SetActive(bool active)
{
}

void KunrealEngine::ButtonSystem::SetButtonFunc(std::function<void()> func)
{
	_script = func;
}

void KunrealEngine::ButtonSystem::SetImage(ImageRenderer* image)
{
	_image = image;
}

void KunrealEngine::ButtonSystem::Setfocused(ImageRenderer* focusimage)
{
	_focusimage = focusimage;
}
