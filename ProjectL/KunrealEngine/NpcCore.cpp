#include "NpcCore.h"
#include "ToolBox.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Transform.h"
#include "Component.h"


KunrealEngine::NpcCore::NpcCore()
	: _npcName("No Named"), _transform(nullptr), _enoughDistance(false), _playerTransform(nullptr)
	, _interactionRange(0), _interactionStep(0)
{

}

KunrealEngine::NpcCore::~NpcCore()
{

}

void KunrealEngine::NpcCore::Initialize()
{
	_transform = SceneManager::GetInstance().GetCurrentScene()->GetGameObject("NPC_zeolight")->GetComponent<Transform>();
	_playerTransform = SceneManager::GetInstance().GetCurrentScene()->GetGameObject("Player")->GetComponent<Transform>();
}

void KunrealEngine::NpcCore::UpdateNpc()
{
	DirectX::XMFLOAT3 mypos(_transform->GetPosition().x, _transform->GetPosition().y, _transform->GetPosition().z);
	DirectX::XMFLOAT3 playerpos(_playerTransform->GetPosition().x, _playerTransform->GetPosition().y, _playerTransform->GetPosition().z);

	if (_interactionRange > ToolBox::GetDistance(mypos, playerpos))
	{
		_enoughDistance = true;
	}

	else if (_interactionRange < ToolBox::GetDistance(mypos, playerpos))
	{
		_enoughDistance = false;
	}
}

void KunrealEngine::NpcCore::SetNpcName(std::string npcName)
{
	_npcName = npcName;
}

void KunrealEngine::NpcCore::SetInteractionRange(int Range)
{
	_interactionRange = Range;
}

void KunrealEngine::NpcCore::SetInterationStep(int step)
{
	_interactionStep = step;
}

void KunrealEngine::NpcCore::SetPlayer(Transform* playerTransform)
{
	_playerTransform = playerTransform;
}
