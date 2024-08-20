#pragma once
#include <functional>
#include "CommonHeader.h"

namespace KunrealEngine
{
	class GameObject;
	class MeshRenderer;
	class ImageRenderer;
	class Projectile;

	class Ability
	{
		friend class EventManager;
		friend class PlayerAbility;
		friend class BattleUIManager;

	public:
		Ability();
		~Ability();

		void Initialize(std::string objName);
		void Release();
		void Update();

		void SetActive(bool active);

	private:
		std::string _abilityName;	// 스킬 이름

		float _damage;				// 피해량	// 플레이어의 damageScale과 곱연산
		float _stamina;				// 기력 소모량
		float _staggerDamage;		// 무력화 피해량
		float _cooldown;			// 재사용 대기시간
		float _range;				// 사거리

		bool _isActivated;			// 사용가능한가	// 스킬 창에 올라가 있는지
		bool _isUsable;				// 쿨이 돌았는가
		bool _isHit;				// 보스가 피격당했는지

		ImageRenderer* _icon;		// 스킬 아이콘
		GameObject* _projectile;	// 투사체를 담을 오브젝트

		std::function<void()> _abilityLogic;	// 스킬 로직

	public:
		// 스킬 정보 통합 입력
		void SetTotalData(std::string name, float damage, float stamina, float sDamage, float cooldown, float range);

		// 스킬 정보를 개별 입력
		void SetAbilityName(std::string name);
		void SetDamage(float damage);
		void SetRequireStamina(float stamina);
		void SetStaggerDamage(float stag);
		void SetCoolDown(float cd);
		void SetRange(float range);

		// 스킬이 어떻게 작동할 것인가 설정
		void SetLogic(std::function<void()> logic);

		// 스킬 정보 반환
		float GetDamage();
		float GetStamina();
		float GetStaggerDmg();
		float GetCooldown();
		float GetRange();

	};
}

