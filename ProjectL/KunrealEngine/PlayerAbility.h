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

		//���׿� ���¸� �޾ƿ;� ��
		bool GetisMeteorBefore();

		GameObject* _shot;			// Q ��ų ����ü ��ü
		GameObject* _circle;		// W ��ų ��ü
		GameObject* _laser;			// E ��ų ��ü
		GameObject* _meteor;		// R ��ų � ��ü

		Ability* _shotAbil;
		Ability* _circleAbil;
		Ability* _laserAbil;
		Ability* _meteorAbil;

	private:
		std::vector<Ability*> _abilityContainer;
		Player* _playerComp;
		SoundPlayer* _soundComp;



		GameObject* _laserCrator1;	// E ��ų ������ ��ü
		GameObject* _laserCrator2;
		GameObject* _laserCrator3;
		GameObject* _laserCrator4;
		GameObject* _laserCrator5;
		GameObject* _laserCrator6;

		GameObject* _meteorRange;	// R ��ų ���� ǥ�� ��ü
		GameObject* _meteorCrator;	// � ������ �� ������ ǥ�� ��ü
		bool _beforeMeteor;			// � ��ų ��� �غ�

		bool _isShotHit;			// �� ���� ������ �޵���
		bool _isCircleHit;
		bool _isLaserHit;
		bool _isMeteorHit;

		/// coroutine�� Ȱ���� Ÿ�̸� ������
		bool _isShotReady;			// Q ��Ÿ�� ����

		bool _isCircleReady;			// W ��Ÿ�� ����
		bool _destroyCircle;			// W �Ҹ� ������ ���� ����

		bool _isLaserReady;			// E ��ų ��Ÿ�� ����
		bool _destroyLaser;			// E �Ҹ� ������ ���� ����
		float _laserDestroyCount;	// ������ �Ҹ� Ÿ�̸�

		bool _isMeteorReady;		// R ��Ÿ�� ����

		/// BattleUIManager�� �Ѱ��� ��Ÿ�� üũ ������
		bool _isShotDetected;
		bool _isCircleDetected;
		bool _isLaserDetected;
		bool _isMeteorDetected;

		/// �Ҹ� ��� ���� üũ
		bool _circlesoundplayed;

		// �̰� ������ �ε����� ��Ƶ� ������ġ��
	private:
		int _energyBallShot;
		int _energyBallFlying;
		int _energyBallExplode;
		int _meteorFall;
		int _meteorExplode;
		int _circleunfolding;
		int _soundlaser;

		/// ��ƼŬ ��ġ�� �������
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
		// q ��ų üũ�� ����
		bool _isShotEnded;
		float _shotParticleTimer;

		// w ��ų üũ�� ����
		bool _isCircleEnded;
		float _circleParticleTimer;

		// e ��ų üũ�� ����
		bool _isLaserStarted;
		bool _isLaserEnded;
		float _laserParticleTimer;


		// r ��ų üũ�� ����
		bool _isMeteorEnded;
		float _meteorParticleTimer;

		// ���� ���� ����
		int _maxPotion;					// ���� ��� �ִ�Ƚ��
		float _restorePercentage;		// ���� ȸ�� ����
		float _potionCoolDown;			// ���� ���� ���ð�
		bool _isPotionReady;			// ���� ��� ���� ����

		float _potionTimer;
		bool _isPotionUse;
		// ��Ȱ��ȭ �Ǿ��� ���
	private:
		// Q��ų ���� ��ü
		void ResetShotPos();
		void CreateAbility1();

		// W��ų ������
		void ResetCirclePos();
		void CreateAbility2();

		// E��ų ������
		void ResetLaserPos();
		void CreateAbility3();

		// R��ų ���׿�
		void ResetMeteorPos();
		bool CheckMeteorRange();		// R��ų ��밡�� �������� üũ
		void CreateAbility4();

		// 1��Ű ��ų ����
		void InitializeHealth();
		void RestoreHealth();
		void PlayHealParticle();

		// �ִϸ��̼� �����Ӱ� ��ų �ߵ� ����
		void AnimateByFrame();

		// ��ų ���� ����
		void UpdateAbilityLogic();

		/// ������
		void DebugText();

	public:
		void AddToContanier(Ability* abil);

	private:
		/// �ڷ�ƾ Ÿ�̸� �Լ���

		// Q��ų ��Ÿ��
		Coroutine_Func(shotCoolDown)
		{
			auto* ability = this;
			_isShotReady = false;
			Waitforsecond(ability->_abilityContainer[0]->_cooldown);
			ability->_isShotReady = true;
		};

		// W��ų ��Ÿ�� 
		Coroutine_Func(circleCoolDown)
		{
			auto* ability = this;
			ability->_isCircleReady = false;
			_circlesoundplayed = false;
			Waitforsecond(ability->_abilityContainer[1]->_cooldown);
			ability->_isCircleReady = true;
		};

		// W��ų �ߵ����
		Coroutine_Func(circleStandby)
		{
			auto* ability = this;
			Waitforsecond(0.8f);		// 2�� �� ����

			ability->_destroyCircle = false;	// �Ҹ� ���� �ʱ�ȭ
			ability->_circle->SetActive(true);
			ability->_circle->GetComponent<Projectile>()->SetActive(true);
			ability->_circle->GetComponent<Projectile>()->ResetCondition();
		};

		// W��ų �Ҹ�

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

		// E��ų ��Ÿ��
		Coroutine_Func(LaserCoolDown)
		{
			auto* ability = this;
			ability->_isLaserReady = false;
			Waitforsecond(ability->_abilityContainer[2]->_cooldown);
			ability->_isLaserReady = true;
		};

		// E��ų �ߵ� ���
		Coroutine_Func(LaserStandby)
		{
			auto* ability = this;
			Waitforsecond(0.9f);		// 0.9�� �� ����

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

		// E��ų �Ҹ�����
		_Coroutine(laserDestroy);

		// E��ų ������ ������ ����
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

		//E��ų ��� �� �ش� ũ��κ��� �پ�鵵��
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
				/// �� �κ� ������� ���� �����
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

		// R��ų ��Ÿ��
		Coroutine_Func(meteorCoolDown)
		{
			auto* ability = this;
			ability->_isMeteorReady = false;
			Waitforsecond(ability->_abilityContainer[3]->_cooldown);
			ability->_isMeteorReady = true;
		};

		// R��ų ������ ���ӽð�
		Coroutine_Func(cratorDuration)
		{
			auto* ability = this;

			// ������ ��ü Ȱ��ȭ
			ability->_meteorCrator->GetComponent<TransparentMesh>()->PlayOnce();

			Waitforsecond(5.0f);
			//ability->_meteorCrator->GetComponent<TransparentMesh>()->SetActive(false);
			//ability->_meteorCrator->SetActive(false);
		};

		// ���� ��� ��Ÿ��
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

