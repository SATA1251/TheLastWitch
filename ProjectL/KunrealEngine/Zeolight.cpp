#include "Zeolight.h"
#include "ImageRenderer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "InputSystem.h"
#include "ButtonSystem.h"
#include "Transform.h"

KunrealEngine::Zeolight::Zeolight()
{
}

KunrealEngine::Zeolight::~Zeolight()
{
}

void KunrealEngine::Zeolight::Initialize()
{
	NpcCore::Initialize();
	image = SceneManager::GetInstance().GetCurrentScene()->GetGameObject("zeolight_Image");

	image->GetChilds()[0]->GetComponent<ButtonSystem>()->SetButtonFunc(std::bind(&Zeolight::stage1, this));
	image->GetChilds()[1]->GetComponent<ButtonSystem>()->SetButtonFunc(std::bind(&Zeolight::stage2, this));
	image->GetChilds()[2]->GetComponent<ButtonSystem>()->SetButtonFunc(std::bind(&Zeolight::stage3, this));
	image->GetChilds()[3]->GetComponent<ButtonSystem>()->SetButtonFunc(std::bind(&Zeolight::stage4, this));
}

void KunrealEngine::Zeolight::Release()
{
}

void KunrealEngine::Zeolight::FixedUpdate()
{
}

void KunrealEngine::Zeolight::Update()
{
	NpcCore::UpdateNpc();
	if (KunrealEngine::InputSystem::GetInstance()->KeyDown(KEY::G) && _enoughDistance && !image->GetActivated())
	{
		image->SetActive(true);
	}

	else if (KunrealEngine::InputSystem::GetInstance()->KeyDown(KEY::G) && image->GetActivated())
	{
		image->SetActive(false);
	}
	else if(!_enoughDistance)
	{
		image->SetActive(false);
	}
}

void KunrealEngine::Zeolight::LateUpdate()
{
}

void KunrealEngine::Zeolight::OnTriggerEnter()
{
}

void KunrealEngine::Zeolight::OnTriggerStay()
{
}

void KunrealEngine::Zeolight::OnTriggerExit()
{
}

void KunrealEngine::Zeolight::SetActive(bool active)
{

}

void KunrealEngine::Zeolight::SetNpcName(std::string npcName)
{

}

void KunrealEngine::Zeolight::SetInteractionRange(int Range)
{
	_interactionRange = Range;
}

void KunrealEngine::Zeolight::SetInterationStep(int step)
{

}

bool KunrealEngine::Zeolight::GetInteration()
{
	return _enoughDistance;
}

void KunrealEngine::Zeolight::stage1()
{
	_playerTransform->SetPosition(400.0f,400.0f,400.0f);
	//_playerTransform->GetOwner()->GetComponent<Player>(); 
}

void KunrealEngine::Zeolight::stage2()
{
	_playerTransform->SetPosition(400.0f,00.0f,400.0f);
}

void KunrealEngine::Zeolight::stage3()
{
	_playerTransform->SetPosition(-400.0f,00.0f,400.0f);
}

void KunrealEngine::Zeolight::stage4()
{
	_playerTransform->SetPosition(-400.0f,-400.0f,400.0f);
}
