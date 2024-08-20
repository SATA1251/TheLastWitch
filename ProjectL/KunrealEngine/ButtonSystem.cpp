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
	// �����÷��̾� ������Ʈ ����
	_soundComp = GetOwner()->AddComponent<SoundPlayer>();
	// SOUNDINFO�� �����. ���/3D���� ����/Loop ����/ ����(�⺻�� 100)
	// ��ȯ�� int�� �� �Ҹ��� ����� �ε�����
	_soundclick = _soundComp->CreateSoundInfo("Resources/Sound/Click.wav", false, false);
	// �׸��� �Ҹ��� ����. ������ �ε��� ���� �־��ش�.
	// ���� ������ �̰� bgm(0)���� sfx(1)���� ���п뵵
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
			// �÷��� �ϰ��� �ϴ� ��ġ���� �Լ� ����.
			// update���� ���� ���尡 ����ɶ����� ���� ������� ����
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
