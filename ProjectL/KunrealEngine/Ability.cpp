#include "Ability.h"
#include "SceneManager.h"
#include "Scene.h"
#include "MeshRenderer.h"
#include "ImageRenderer.h"
#include "Projectile.h"

KunrealEngine::Ability::Ability()
	:_damage(0), _stamina(0), _staggerDamage(0), _cooldown(0), _range(0), _isActivated(false), _isUsable(true), _isHit(false)
	, _icon(nullptr), _projectile(nullptr)
{

}

KunrealEngine::Ability::~Ability()
{

}

void KunrealEngine::Ability::Initialize(std::string objName)
{
	_projectile = SceneManager::GetInstance().GetCurrentScene()->CreateObject(objName);
	_projectile->SetTag("playerSub");

	// 스킬을 사용해야 활성화 되어야하니
	_projectile->SetActive(false);
}

void KunrealEngine::Ability::Release()
{
	if (_icon != nullptr)
	{
		SceneManager::GetInstance().GetCurrentScene()->DeleteGameObject(_icon->GetOwner());
	}
}

void KunrealEngine::Ability::Update()
{

}

void KunrealEngine::Ability::SetActive(bool active)
{
	this->_isActivated = active;
}

void KunrealEngine::Ability::SetTotalData(std::string name, float damage, float stamina, float sDamage, float cooldown, float range)
{
	this->_abilityName = name;
	this->_damage = damage;
	this->_stamina = stamina;
	this->_staggerDamage = sDamage;
	this->_cooldown = cooldown;
	this->_range = range;
}

void KunrealEngine::Ability::SetAbilityName(std::string name)
{
	this->_abilityName = name;
}

void KunrealEngine::Ability::SetDamage(float damage)
{
	this->_damage = damage;
}

void KunrealEngine::Ability::SetRequireStamina(float stamina)
{
	this->_stamina = stamina;
}

void KunrealEngine::Ability::SetStaggerDamage(float stag)
{
	this->_staggerDamage = stag;
}

void KunrealEngine::Ability::SetCoolDown(float cd)
{
	this->_cooldown = cd;
}

void KunrealEngine::Ability::SetRange(float range)
{
	this->_range = range;
}

void KunrealEngine::Ability::SetLogic(std::function<void()> logic)
{
	this->_abilityLogic = logic;
}

float KunrealEngine::Ability::GetDamage()
{
	return this->_damage;
}

float KunrealEngine::Ability::GetStamina()
{
	return this->_stamina;
}

float KunrealEngine::Ability::GetStaggerDmg()
{
	return this->_staggerDamage;
}

float KunrealEngine::Ability::GetCooldown()
{
	return this->_cooldown;
}

float KunrealEngine::Ability::GetRange()
{
	return this->_range;
}
