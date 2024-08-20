#pragma once

#include "ToolBox.h"
#include "Component.h"
#include "GameObject.h"
#include "Player.h"
#include "Ability.h"
#include "Projectile.h"
#include "BoxCollider.h"
#include "Coroutine.h"
#include "Particle.h"
#include "TransparentMesh.h"
#include "Transform.h"
#include "TimeManager.h"
#include "SoundPlayer.h"

namespace KunrealEngine
{
	class Boss;
	class SoundPlayer;

	class _DECLSPEC PlayerAbility : public Component
	{
		friend class Player;
		friend class BattleUIManager;
	public:
		PlayerAbility();
		~PlayerAbility();

		void Initialize() override;
		void Release() override;

		void FixedUpdate() override;
		void Update() override;
		void LateUpdate() override;

		void OnTriggerEnter() override;
		void OnTriggerStay() override;
		void OnTriggerExit() override;

		void SetActive(bool active) override;

		//메테오 상태를 받아와야 해
		bool GetisMeteorBefore();

		GameObject* _shot;			// Q 스킬 투사체 객체
		GameObject* _circle;		// W 스킬 객체
		GameObject* _laser;			// E 스킬 객체
		GameObject* _meteor;		// R 스킬 운석 객체

		Ability* _shotAbil;
		Ability* _circleAbil;
		Ability* _laserAbil;
		Ability* _meteorAbil;

	private:
		std::vector<Ability*> _abilityContainer;
		Player* _playerComp;
		SoundPlayer* _soundComp;



		GameObject* _laserCrator1;	// E 스킬 그을림 객체
		GameObject* _laserCrator2;
		GameObject* _laserCrator3;
		GameObject* _laserCrator4;
		GameObject* _laserCrator5;
		GameObject* _laserCrator6;

		GameObject* _meteorRange;	// R 스킬 범위 표시 객체
		GameObject* _meteorCrator;	// 운석 떨어진 후 그을림 표현 객체
		bool _beforeMeteor;			// 운석 스킬 사용 준비

		bool _isShotHit;			// 한 번만 데미지 받도록
		bool _isCircleHit;
		bool _isLaserHit;
		bool _isMeteorHit;

		/// coroutine을 활용한 타이머 변수들
		bool _isShotReady;			// Q 쿨타임 조건

		bool _isCircleReady;			// W 쿨타임 조건
		bool _destroyCircle;			// W 소멸 조건을 위한 변수

		bool _isLaserReady;			// E 스킬 쿨타임 조건
		bool _destroyLaser;			// E 소멸 조건을 위한 변수
		float _laserDestroyCount;	// 레이저 소멸 타이머

		bool _isMeteorReady;		// R 쿨타임 조건

		/// BattleUIManager에 넘겨줄 쿨타임 체크 변수들
		bool _isShotDetected;
		bool _isCircleDetected;
		bool _isLaserDetected;
		bool _isMeteorDetected;

		/// 소리 재생 여부 체크
		bool _circlesoundplayed;

		// 이건 사운드의 인덱스를 담아둔 변수뭉치들
	private:
		int _energyBallShot;
		int _energyBallFlying;
		int _energyBallExplode;
		int _meteorFall;
		int _meteorExplode;
		int _circleunfolding;
		int _soundlaser;

		/// 파티클 배치용 멤버변수
	private:
		GameObject* _shotParticle2;
		GameObject* _shotParticle3;
		GameObject* _shotParticle4;

		GameObject* _shotParticleHit1;
		GameObject* _shotParticleHit2;
		GameObject* _shotParticleHit3;

		GameObject* _circleParticle;
		GameObject* _circleBuffParticle1;
		GameObject* _circleBuffParticle2;

		GameObject* _laserParticle1;
		GameObject* _laserParticle2;
		GameObject* _laserParticle3;
		GameObject* _laserParticle4;

		GameObject* _meteorParticle2;
		GameObject* _meteorParticle3;
		GameObject* _meteorParticle4;

		GameObject* _meteorParticleHit1;
		GameObject* _meteorParticleHit2;
		GameObject* _meteorParticleHit3;
		GameObject* _meteorParticleHit4;

		GameObject* _healthParticle1;
		GameObject* _healthParticle2;
		GameObject* _healthParticle3;
		GameObject* _healthParticle4;
		GameObject* _healthParticle5;
		GameObject* _healthParticle6;

		std::vector<GameObject*> _healthParticleList;
	private:
		// q 스킬 체크용 변수
		bool _isShotEnded;
		float _shotParticleTimer;

		// w 스킬 체크용 변수
		bool _isCircleEnded;
		float _circleParticleTimer;

		// e 스킬 체크용 변수
		bool _isLaserStarted;
		bool _isLaserEnded;
		float _laserParticleTimer;


		// r 스킬 체크용 변수
		bool _isMeteorEnded;
		float _meteorParticleTimer;

		// 포션 로직 변수
		int _maxPotion;					// 포션 사용 최대횟수
		float _restorePercentage;		// 포션 회복 비율
		float _potionCoolDown;			// 포션 재사용 대기시간
		bool _isPotionReady;			// 포션 사용 가능 여부

		float _potionTimer;
		bool _isPotionUse;
		// 비활성화 되었을 경우
	private:
		// Q스킬 전기 구체
		void ResetShotPos();
		void CreateAbility1();

		// W스킬 마법진
		void ResetCirclePos();
		void CreateAbility2();

		// E스킬 레이저
		void ResetLaserPos();
		void CreateAbility3();

		// R스킬 메테오
		void ResetMeteorPos();
		bool CheckMeteorRange();		// R스킬 사용가능 범위인지 체크
		void CreateAbility4();

		// 1번키 스킬 물약
		void InitializeHealth();
		void RestoreHealth();
		void PlayHealParticle();

		// 애니메이션 프레임과 스킬 발동 여부
		void AnimateByFrame();

		// 스킬 로직 랩핑
		void UpdateAbilityLogic();

		/// 디버깅용
		void DebugText();

	public:
		void AddToContanier(Ability* abil);

	private:
		/// 코루틴 타이머 함수들

		// Q스킬 쿨타임
		Coroutine_Func(shotCoolDown)
		{
			auto* ability = this;
			_isShotReady = false;
			Waitforsecond(ability->_abilityContainer[0]->_cooldown);
			ability->_isShotReady = true;
		};

		// W스킬 쿨타임 
		Coroutine_Func(circleCoolDown)
		{
			auto* ability = this;
			ability->_isCircleReady = false;
			_circlesoundplayed = false;
			Waitforsecond(ability->_abilityContainer[1]->_cooldown);
			ability->_isCircleReady = true;
		};

		// W스킬 발동대기
		Coroutine_Func(circleStandby)
		{
			auto* ability = this;
			Waitforsecond(0.8f);		// 2초 뒤 실행

			ability->_destroyCircle = false;	// 소멸 조건 초기화
			ability->_circle->SetActive(true);
			ability->_circle->GetComponent<Projectile>()->SetActive(true);
			ability->_circle->GetComponent<Projectile>()->ResetCondition();
		};

		// W스킬 소멸

		Coroutine_Func(CircleFadeOut)
		{
			auto* ability = this;
			Waitforsecond(6.0f);

			while (ability->_circle->GetComponent<Transform>()->GetScale().x > 0.0f)
			{
				ability->_circle->GetComponent<Transform>()->SetTotalScale(ability->_circle->GetComponent<Transform>()->GetScale().x - 0.01f);
				_circleParticle->GetComponent<Particle>()->SetActive(false);
				Return_null;
			}
			ability->_circle->SetActive(false);
			_circleBuffParticle1->GetComponent<Particle>()->SetActive(false);
			_circleBuffParticle2->GetComponent<Particle>()->SetActive(false);
		};

		// E스킬 쿨타임
		Coroutine_Func(LaserCoolDown)
		{
			auto* ability = this;
			ability->_isLaserReady = false;
			Waitforsecond(ability->_abilityContainer[2]->_cooldown);
			ability->_isLaserReady = true;
		};

		// E스킬 발동 대기
		Coroutine_Func(LaserStandby)
		{
			auto* ability = this;
			Waitforsecond(0.9f);		// 0.9초 뒤 실행

			if (ability->_playerComp->_playerStatus != Player::Status::ABILITY || _playerComp->_abilityAnimationIndex != 3)
			{
				ability->_soundComp->Stop(_soundlaser);
				co_return;
			}

			ability->_laser->SetActive(true);
			ability->_laser->GetComponent<BoxCollider>()->SetActive(true);

			_laserParticle1->SetActive(true);
			_laserParticle2->SetActive(true);
			_laserParticle4->SetActive(true);

			_laserParticle1->GetComponent<Particle>()->SetActive(true);
			_laserParticle2->GetComponent<Particle>()->SetActive(true);
			_laserParticle4->GetComponent<Particle>()->SetActive(true);
		};

		// E스킬 소멸조건
		_Coroutine(laserDestroy);

		// E스킬 그을림 순차적 실행
		Coroutine_Func(LaserCratorStart)
		{
			auto* ability = this;

			Waitforsecond(1.0f);

			if (ability->_laser->GetActivated())
			{
				ability->_laserCrator1->GetComponent<TransparentMesh>()->SetActive(true);
				ability->_laserCrator1->GetComponent<TransparentMesh>()->TransformForceUpdate();
				ability->_laserCrator1->GetComponent<TransparentMesh>()->Reset();
				ability->_laserCrator1->GetComponent<TransparentMesh>()->PlayOnce();
			}

			Waitforsecond(0.1f);

			if (ability->_laser->GetActivated())
			{
				ability->_laserCrator2->GetComponent<TransparentMesh>()->SetActive(true);
				ability->_laserCrator2->GetComponent<TransparentMesh>()->TransformForceUpdate();
				ability->_laserCrator2->GetComponent<TransparentMesh>()->Reset();
				ability->_laserCrator2->GetComponent<TransparentMesh>()->PlayOnce();

			}

			Waitforsecond(0.1f);

			if (ability->_laser->GetActivated())
			{
				ability->_laserCrator3->GetComponent<TransparentMesh>()->SetActive(true);
				ability->_laserCrator3->GetComponent<TransparentMesh>()->TransformForceUpdate();
				ability->_laserCrator3->GetComponent<TransparentMesh>()->Reset();
				ability->_laserCrator3->GetComponent<TransparentMesh>()->PlayOnce();
			}

			Waitforsecond(0.1f);

			if (ability->_laser->GetActivated())
			{
				ability->_laserCrator4->GetComponent<TransparentMesh>()->SetActive(true);
				ability->_laserCrator4->GetComponent<TransparentMesh>()->TransformForceUpdate();
				ability->_laserCrator4->GetComponent<TransparentMesh>()->Reset();
				ability->_laserCrator4->GetComponent<TransparentMesh>()->PlayOnce();
			}

			Waitforsecond(0.1f);

			if (ability->_laser->GetActivated())
			{
				ability->_laserCrator5->GetComponent<TransparentMesh>()->SetActive(true);
				ability->_laserCrator5->GetComponent<TransparentMesh>()->TransformForceUpdate();
				ability->_laserCrator5->GetComponent<TransparentMesh>()->Reset();
				ability->_laserCrator5->GetComponent<TransparentMesh>()->PlayOnce();
			}

			Waitforsecond(0.1f);

			if (ability->_laser->GetActivated())
			{
				ability->_laserCrator6->GetComponent<TransparentMesh>()->SetActive(true);
				ability->_laserCrator6->GetComponent<TransparentMesh>()->TransformForceUpdate();
				ability->_laserCrator6->GetComponent<TransparentMesh>()->Reset();
				ability->_laserCrator6->GetComponent<TransparentMesh>()->PlayOnce();
			}

		};

		//E스킬 취소 시 해당 크기로부터 줄어들도록
		Coroutine_Func(LaserFadeOut)
		{
			auto* ability = this;
			Particle* laserP1 = ability->_laserParticle1->GetComponent<Particle>();
			Particle* laserP2 = ability->_laserParticle2->GetComponent<Particle>();
			Particle* laserP3 = ability->_laserParticle3->GetComponent<Particle>();
			Particle* laserP4 = ability->_laserParticle4->GetComponent<Particle>();

			float half1 = laserP1->GetParticleSize().x * 0.5f;
			float half2 = laserP2->GetParticleSize().x * 0.5f;
			float half3 = laserP3->GetParticleSize().x * 0.5f;
			float half4 = laserP4->GetParticleSize().x * 0.5f;

			float delta = 0;
			while (true)
			{
				/// 이 부분 상수말고 변수 써야함
				delta += TimeManager::GetInstance().GetDeltaTime();

				if (laserP1->GetParticleSize().x > 0.0f)
				{
					laserP1->SetParticleSize((laserP1->GetParticleSize().x - (delta * half1)) * ToolBox::GetRandomFloat(0.8f, 1.0f), (laserP1->GetParticleSize().y - (delta * half1)) * ToolBox::GetRandomFloat(0.8f, 1.0f));
				}

				if (laserP2->GetParticleSize().x > 0.0f)
				{
					laserP2->SetParticleSize((laserP2->GetParticleSize().x - (delta * half2)) * ToolBox::GetRandomFloat(0.8f, 1.0f), (laserP2->GetParticleSize().y - (delta * half2)) * ToolBox::GetRandomFloat(0.8f, 1.0f));
				}

				if (laserP3->GetParticleSize().x > 0.0f)
				{
					laserP3->SetParticleSize((laserP3->GetParticleSize().x - (delta * half3)) * ToolBox::GetRandomFloat(0.8f, 1.0f), (laserP3->GetParticleSize().y - (delta * half3)) * ToolBox::GetRandomFloat(0.8f, 1.0f));
				}

				if (laserP4->GetParticleSize().x > 0.0f)
				{
					laserP4->SetParticleSize((laserP4->GetParticleSize().x - (delta * half4)) * ToolBox::GetRandomFloat(0.8f, 1.0f), (laserP4->GetParticleSize().x - (delta * half4)) * ToolBox::GetRandomFloat(0.8f, 1.0f));
				}

				if (delta > 2) break;
				Return_null;
			}
			ability->_laserParticle1->SetActive(false);
			ability->_laserParticle2->SetActive(false);
			ability->_laserParticle3->SetActive(false);
			ability->_laserParticle4->SetActive(false);
			ability->_destroyLaser = true;
			ability->_isLaserReady = false;
			ability->_isLaserStarted = false;
		};

		Coroutine_Func(laserHit)
		{
			auto* ability = this;
			Waitforsecond(0.4f);

			ability->_isLaserHit = true;

		};

		// R스킬 쿨타임
		Coroutine_Func(meteorCoolDown)
		{
			auto* ability = this;
			ability->_isMeteorReady = false;
			Waitforsecond(ability->_abilityContainer[3]->_cooldown);
			ability->_isMeteorReady = true;
		};

		// R스킬 그을림 지속시간
		Coroutine_Func(cratorDuration)
		{
			auto* ability = this;

			// 그을림 객체 활성화
			ability->_meteorCrator->GetComponent<TransparentMesh>()->PlayOnce();

			Waitforsecond(5.0f);
			//ability->_meteorCrator->GetComponent<TransparentMesh>()->SetActive(false);
			//ability->_meteorCrator->SetActive(false);
		};

		// 물약 사용 쿨타임
		Coroutine_Func(PotionCoolDown)
		{
			auto* ability = this;
			ability->_isPotionReady = false;
			Waitforsecond(ability->_potionCoolDown);
			ability->_isPotionReady = true;
		};

		Coroutine_Func(PotionEffectDraw)
		{
			_healthParticle1->GetComponent<Particle>()->SetActive(true);
			_healthParticle1->GetComponent<Transform>()->SetPosition(this->GetOwner()->GetComponent<Transform>()->GetPosition());
			_healthParticle1->GetComponent<Particle>()->SetParticleSize(2,3);

			_healthParticle6->GetComponent<Particle>()->SetActive(true);
			_healthParticle6->GetComponent<Transform>()->SetPosition(this->GetOwner()->GetComponent<Transform>()->GetPosition());
			_healthParticle6->GetComponent<Particle>()->SetParticleSize(2,3);

			Waitforsecond(0.1f);

			_healthParticle3->GetComponent<Particle>()->SetActive(true);
			_healthParticle3->GetComponent<Transform>()->SetPosition(this->GetOwner()->GetComponent<Transform>()->GetPosition());
			_healthParticle3->GetComponent<Particle>()->SetParticleSize(2, 3);

			_healthParticle2->GetComponent<Particle>()->SetActive(true);
			_healthParticle2->GetComponent<Transform>()->SetPosition(this->GetOwner()->GetComponent<Transform>()->GetPosition());
			_healthParticle2->GetComponent<Particle>()->SetParticleSize(2, 3);

			Waitforsecond(0.1f);

			_healthParticle5->GetComponent<Particle>()->SetActive(true);
			_healthParticle5->GetComponent<Transform>()->SetPosition(this->GetOwner()->GetComponent<Transform>()->GetPosition());
			_healthParticle5->GetComponent<Particle>()->SetParticleSize(2, 3);

			_healthParticle4->GetComponent<Particle>()->SetActive(true);
			_healthParticle4->GetComponent<Transform>()->SetPosition(this->GetOwner()->GetComponent<Transform>()->GetPosition());
			_healthParticle4->GetComponent<Particle>()->SetParticleSize(2, 3);

			Waitforsecond(0.1f);

			_healthParticle1->GetComponent<Particle>()->SetActive(true);
			_healthParticle1->GetComponent<Transform>()->SetPosition(this->GetOwner()->GetComponent<Transform>()->GetPosition());
			_healthParticle1->GetComponent<Particle>()->SetParticleSize(0, 0);

			_healthParticle5->GetComponent<Particle>()->SetActive(true);
			_healthParticle5->GetComponent<Transform>()->SetPosition(this->GetOwner()->GetComponent<Transform>()->GetPosition());
			_healthParticle5->GetComponent<Particle>()->SetParticleSize(0, 0);
			
			Waitforsecond(0.1f);

			_healthParticle6->GetComponent<Particle>()->SetActive(true);
			_healthParticle6->GetComponent<Transform>()->SetPosition(this->GetOwner()->GetComponent<Transform>()->GetPosition());
			_healthParticle6->GetComponent<Particle>()->SetParticleSize(0, 0);

			_healthParticle4->GetComponent<Particle>()->SetActive(true);
			_healthParticle4->GetComponent<Transform>()->SetPosition(this->GetOwner()->GetComponent<Transform>()->GetPosition());
			_healthParticle4->GetComponent<Particle>()->SetParticleSize(0, 0);

			Waitforsecond(0.1f);

			_healthParticle3->GetComponent<Particle>()->SetActive(true);
			_healthParticle3->GetComponent<Transform>()->SetPosition(this->GetOwner()->GetComponent<Transform>()->GetPosition());
			_healthParticle3->GetComponent<Particle>()->SetParticleSize(0, 0);

			_healthParticle2->GetComponent<Particle>()->SetActive(true);
			_healthParticle2->GetComponent<Transform>()->SetPosition(this->GetOwner()->GetComponent<Transform>()->GetPosition());
			_healthParticle2->GetComponent<Particle>()->SetParticleSize(0, 0);
		};
	};
}

