#include "PlayerAbility.h"
#include "InputSystem.h"
#include "GraphicsSystem.h"
#include "MeshRenderer.h"
#include "BoxCollider.h"
#include "Animator.h"
#include "Transform.h"

#include "PlayerMove.h"
#include "Boss.h"
#include "Kamen.h"
#include "EventManager.h"
#include "Navigation.h"

#include "SceneManager.h"
#include "EventManager.h"
#include "Scene.h"
#include "MeteorRange.h"

KunrealEngine::PlayerAbility::PlayerAbility()
	:_playerComp(nullptr), _meteor(nullptr), _shot(nullptr), _circle(nullptr), _laser(nullptr), _meteorRange(nullptr)
	, _beforeMeteor(false), _meteorCrator(nullptr), _laserCrator1(nullptr), _laserCrator2(nullptr), _laserCrator3(nullptr), _laserCrator4(nullptr), _laserCrator5(nullptr), _laserCrator6(nullptr)
	, _isCircleReady(true), _destroyCircle(false), _isShotReady(true), _isMeteorReady(true), _isLaserReady(true)
	, _isShotHit(false), _isLaserHit(false), _isMeteorHit(false), _isCircleHit(false)
	, _isShotDetected(false), _isCircleDetected(false), _isLaserDetected(false), _isMeteorDetected(false), _isShotEnded(false),
	_shotParticleTimer(0), _isMeteorEnded(false), _meteorParticleTimer(0), _isCircleEnded(false), _circleParticleTimer(0), _isLaserEnded(false), _laserParticleTimer(0), _isLaserStarted(false), _destroyLaser(false),
	_shotParticle2(nullptr), _shotParticle3(nullptr), _shotParticle4(nullptr), _shotParticleHit1(nullptr), _shotParticleHit2(nullptr), _shotParticleHit3(nullptr),
	_laserParticle1(nullptr), _laserParticle2(nullptr), _laserParticle3(nullptr), _laserParticle4(nullptr), _meteorParticle2(nullptr), _meteorParticle3(nullptr), _meteorParticle4(nullptr), _meteorParticleHit1(nullptr), _meteorParticleHit2(nullptr), _meteorParticleHit3(nullptr), _meteorParticleHit4(nullptr),
	_maxPotion(5), _restorePercentage(0.3f), _potionCoolDown(8.0f), _isPotionReady(true), _circleParticle(nullptr), _circleBuffParticle1(nullptr), _circleBuffParticle2(nullptr),
	_laserDestroyCount(2.5f), _potionTimer(0), _isPotionUse(false), _healthParticle1(nullptr), _healthParticle2(nullptr), _healthParticle3(nullptr), _healthParticle4(nullptr), _healthParticle5(nullptr), _healthParticle6(nullptr)

{

}

KunrealEngine::PlayerAbility::~PlayerAbility()
{

}

void KunrealEngine::PlayerAbility::Initialize()
{
	_playerComp = GetOwner()->GetComponent<Player>();
	_soundComp = GetOwner()->GetComponent<SoundPlayer>();

	CreateAbility1();
	CreateAbility2();
	CreateAbility3();
	CreateAbility4();
	InitializeHealth();

	_energyBallShot = _soundComp->CreateSoundInfo("Resources/Sound/avatar_change.mp3", true, false);
	_energyBallFlying = _soundComp->CreateSoundInfo("Resources/Sound/aura_loop.mp3", true, false);
	_energyBallExplode = _soundComp->CreateSoundInfo("Resources/Sound/wavsound.mp3", true, false);
	_circleunfolding = _soundComp->CreateSoundInfo("Resources/Sound/magic_filed.mp3", true, false);
	_soundlaser = _soundComp->CreateSoundInfo("Resources/Sound/Laser.mp3", true, false);
	_meteorExplode = _soundComp->CreateSoundInfo("Resources/Sound/Rock_Break.mp3", true, false);
	_meteorFall = _soundComp->CreateSoundInfo("Resources/Sound/Meteor.mp3", true, false);
	_soundComp->CreateSound(_energyBallShot, 1);
	_soundComp->CreateSound(_energyBallFlying, 1);
	_soundComp->CreateSound(_energyBallExplode, 1);
	_soundComp->CreateSound(_meteorExplode, 1);
	_soundComp->CreateSound(_meteorFall, 1);
	_soundComp->CreateSound(_circleunfolding, 1);
	_soundComp->CreateSound(_soundlaser, 1);
}

void KunrealEngine::PlayerAbility::Release()
{
	for (auto abils : _abilityContainer)
	{
		abils->Release();
	}
}

void KunrealEngine::PlayerAbility::FixedUpdate()
{

}

void KunrealEngine::PlayerAbility::Update()
{
	/// 함수로 나누자 나중에

	// 타이틀씬에서는 작동하지 않도록
	if (this->GetOwner()->GetObjectScene()->GetSceneName() != "Main")
	{
		return;
	}

	if (InputSystem::GetInstance()->KeyDown(KEY::Q) && this->_isShotReady && !_playerComp->_onCasting)
	{
		// 플레이어가 행동할 수 없는 상태라면 return
		if (this->_playerComp->_playerStatus == Player::Status::DEAD || this->_playerComp->_playerStatus == Player::Status::STAGGERED || this->_playerComp->_playerStatus == Player::Status::SWEEP || this->_playerComp->_playerStatus == Player::Status::PARALYSIS || this->_playerComp->_playerStatus == Player::Status::DASH)
		{
			return;
		}

		// 행동불가 상황이라면 return
		if (_playerComp->_playerBindFlag)
		{
			return;
		}

		_soundComp->Stop(_energyBallShot);
		_soundComp->Stop(_energyBallFlying);
		_soundComp->Play(_energyBallShot);
		_soundComp->Play(_energyBallFlying);

		ResetShotPos();
		Startcoroutine(shotCoolDown);
		_isShotDetected = true;
		_isShotHit = true;
		_shot->SetActive(true);
		_shot->GetComponent<Projectile>()->SetActive(true);
		_shotParticle2->GetComponent<Particle>()->SetActive(true);
		_shotParticle3->GetComponent<Particle>()->SetActive(true);
		_shotParticle4->GetComponent<Particle>()->SetActive(true);
		_shot->GetComponent<Projectile>()->ResetCondition();

		_playerComp->_playerStatus = Player::Status::ABILITY;
		_playerComp->_abilityAnimationIndex = 1;				// 구체 투척 애니메이션
	}

	if (InputSystem::GetInstance()->KeyDown(KEY::W) && this->_isCircleReady && !_playerComp->_onCasting)
	{
		// 플레이어가 행동할 수 없는 상태라면 return
		if (this->_playerComp->_playerStatus == Player::Status::DEAD || this->_playerComp->_playerStatus == Player::Status::STAGGERED || this->_playerComp->_playerStatus == Player::Status::SWEEP || this->_playerComp->_playerStatus == Player::Status::PARALYSIS || this->_playerComp->_playerStatus == Player::Status::DASH)
		{
			return;
		}

		// 행동불가 상황이라면 return
		if (_playerComp->_playerBindFlag)
		{
			return;
		}

		ResetCirclePos();											// 투사체 위치 리셋
		_isCircleDetected = true;
		_isCircleHit = true;
		_destroyCircle = false;
		_soundComp->Play(_circleunfolding);
		_circle->SetActive(true);
		Startcoroutine(circleCoolDown);

		_playerComp->_playerStatus = Player::Status::ABILITY;
		_playerComp->_abilityAnimationIndex = 2;				// 얼음 소환 애니메이션	

	}

	if (InputSystem::GetInstance()->KeyDown(KEY::E) && this->_isLaserReady && !_playerComp->_onCasting)
	{
		// 플레이어가 행동할 수 없는 상태라면 return
		if (this->_playerComp->_playerStatus == Player::Status::DEAD || this->_playerComp->_playerStatus == Player::Status::STAGGERED || this->_playerComp->_playerStatus == Player::Status::SWEEP || this->_playerComp->_playerStatus == Player::Status::PARALYSIS || this->_playerComp->_playerStatus == Player::Status::DASH)
		{
			this->_soundComp->Stop(_soundlaser);
			return;
		}

		// 행동불가 상황이라면 return
		if (_playerComp->_playerBindFlag)
		{
			return;
		}

		ResetLaserPos();
		this->_soundComp->Play(_soundlaser);
		Startcoroutine(LaserCratorStart);

		_isLaserDetected = true;
		_isLaserHit = true;
		_isLaserStarted = true;
		Startcoroutine(LaserCoolDown);
		Startcoroutine(LaserStandby);
		_playerComp->_playerStatus = Player::Status::ABILITY;
		_playerComp->_abilityAnimationIndex = 3;				// 범위 공격 애니메이션

		_CoroutineIs(laserDestroy)
		{
			auto* ability = this;
			Waitforsecond(ability->_laserDestroyCount);

			ability->_laser->GetComponent<BoxCollider>()->SetActive(false);
			ability->_laser->SetActive(false);

			float delta = 0;
			while (true)
			{
				delta += TimeManager::GetInstance().GetDeltaTime();
				ability->_laserParticle1->GetComponent<Particle>()->SetParticleSize((50 - (delta * 25)) * ToolBox::GetRandomFloat(0.8f, 1.0f), (50 - (delta * 25)) * ToolBox::GetRandomFloat(0.8f, 1.0f));
				ability->_laserParticle2->GetComponent<Particle>()->SetParticleSize((20 - (delta * 10)) * ToolBox::GetRandomFloat(0.8f, 1.0f), (20 - (delta * 10)) * ToolBox::GetRandomFloat(0.8f, 1.0f));
				ability->_laserParticle3->GetComponent<Particle>()->SetParticleSize((50 - (delta * 25)) * ToolBox::GetRandomFloat(0.8f, 1.0f), (50 - (delta * 25)) * ToolBox::GetRandomFloat(0.8f, 1.0f));
				ability->_laserParticle4->GetComponent<Particle>()->SetParticleSize((50 - (delta * 25)) * ToolBox::GetRandomFloat(0.8f, 1.0f), (50 - (delta * 25)) * ToolBox::GetRandomFloat(0.8f, 1.0f));

				if (delta > 2)
				{
					ability->_laserDestroyCount = 2.5f;
					break;
				}
				Return_null;
			}
			ability->_laserParticle1->SetActive(false);
			ability->_laserParticle2->SetActive(false);
			ability->_laserParticle3->SetActive(false);
			ability->_laserParticle4->SetActive(false);
			ability->_destroyLaser = true;
			ability->_isLaserStarted = false;

		};
		Startcoroutine(laserDestroy);

	}

	// 레이저 사용중 피격 혹은 취소되면 레이저 사라지도록
	if (this->_laser->GetActivated() && (this->_playerComp->_playerStatus == Player::Status::DEAD || this->_playerComp->_playerStatus == Player::Status::STAGGERED || this->_playerComp->_playerStatus == Player::Status::SWEEP || this->_playerComp->_playerStatus == Player::Status::PARALYSIS || this->_playerComp->_playerStatus == Player::Status::DASH || this->_playerComp->_playerBindFlag))
	{
		this->_laser->SetActive(false);
		this->_soundComp->Stop(_soundlaser);
		this->_laserParticle1->SetActive(false);
		this->_laserParticle2->SetActive(false);
		this->_laserParticle3->SetActive(false);
		this->_laserParticle4->SetActive(false);
	}

	// 운석이 떨어지는 도중 시네마틱 상태가 되면 운석이 사라지도록
	if (this->_meteor->GetActivated() && this->_playerComp->_playerStatus == Player::Status::CINEMATIC)
	{
		this->_meteor->SetActive(false);
		this->_soundComp->Stop(_meteorFall);
		this->_meteorParticle2->SetActive(false);
		this->_meteorParticle3->SetActive(false);
		this->_meteorParticle4->SetActive(false);

		this->_beforeMeteor = false;
		this->_meteorRange->GetComponent<MeteorRange>()->_onCast = false;
		this->_meteorRange->SetActive(false);
		this->_meteorRange->GetComponent<MeteorRange>()->SetActive(false);
	}

	if (InputSystem::GetInstance()->KeyDown(KEY::R) && this->_isMeteorReady && !_playerComp->_onCasting)
	{
		// 행동불가 상황이라면 return
		if (_playerComp->_playerBindFlag)
		{
			return;
		}

		this->_beforeMeteor = true;
	}

	if (this->_beforeMeteor)
	{
		// 플레이어가 행동할 수 없는 상태라면 return
		if (this->_playerComp->_playerStatus == Player::Status::DEAD || this->_playerComp->_playerStatus == Player::Status::STAGGERED || this->_playerComp->_playerStatus == Player::Status::SWEEP || this->_playerComp->_playerStatus == Player::Status::PARALYSIS || this->_playerComp->_playerStatus == Player::Status::DASH)
		{
			this->_beforeMeteor = false;

			if (!this->_meteor->GetActivated())
			{
				this->_meteorRange->GetComponent<MeteorRange>()->_onCast = false;
				this->_meteorRange->SetActive(false);
				this->_meteorRange->GetComponent<MeteorRange>()->SetActive(false);
			}
			
			return;
		}

		// 다른 스킬이 사용되면 return
		if (this->_playerComp->_playerStatus == Player::Status::ABILITY && this->_playerComp->_abilityAnimationIndex != 4)
		{
			this->_beforeMeteor = false;

			if (!this->_meteor->GetActivated())
			{
				this->_meteorRange->GetComponent<MeteorRange>()->_onCast = false;
				this->_meteorRange->SetActive(false);
				this->_meteorRange->GetComponent<MeteorRange>()->SetActive(false);
			}

			return;
		}

		// 범위 객체 활성화
		if (!this->_meteorRange->GetComponent<MeteorRange>()->GetActivated())
		{
			this->_meteorRange->SetActive(true);
			this->_meteorRange->GetComponent<MeteorRange>()->SetActive(true);
		}

			// 플레이어가 ESC키를 누를 경우 취소
			if (InputSystem::GetInstance()->KeyDown(KEY::ESCAPE))
			{
				this->_beforeMeteor = false;
				this->_meteorRange->SetActive(false);
				this->_meteorRange->GetComponent<MeteorRange>()->SetActive(false);
				return;
			}

			if (InputSystem::GetInstance()->MouseButtonDown(0))
			{
				// 사용할 수 없는 범위라면 return			// 이미 사용중이라면
				if (!CheckMeteorRange() || this->_meteorRange->GetComponent<MeteorRange>()->_onCast)
				{
					return;
				}
				else
				{
					this->_meteorRange->GetComponent<MeteorRange>()->_onCast = true;
					ResetMeteorPos();
					_isMeteorDetected = true;
					_isMeteorHit = true;
					Startcoroutine(meteorCoolDown);
					_playerComp->_playerStatus = Player::Status::ABILITY;
					_playerComp->_abilityAnimationIndex = 4;
				}
			}
		}

	if (InputSystem::GetInstance()->KeyDown(KEY::_1) && this->_maxPotion > 0 && this->_isPotionReady)
	{
		// 사용할 수 없는 상태
		if (_playerComp->_playerStatus == Player::Status::DEAD || _playerComp->_playerStatus == Player::Status::CINEMATIC)
		{
			return;
		}

		RestoreHealth();
	}

	if (_isPotionUse == true)
	{
		PlayHealParticle();
	}

	AnimateByFrame();
	UpdateAbilityLogic();

	//DebugText();
}

void KunrealEngine::PlayerAbility::LateUpdate()
{

}

void KunrealEngine::PlayerAbility::OnTriggerEnter()
{

}

void KunrealEngine::PlayerAbility::OnTriggerStay()
{

}

void KunrealEngine::PlayerAbility::OnTriggerExit()
{

}

void KunrealEngine::PlayerAbility::SetActive(bool active)
{
	this->_isActivated = active;
}

bool KunrealEngine::PlayerAbility::GetisMeteorBefore()
{
	return _beforeMeteor;
}


void KunrealEngine::PlayerAbility::ResetShotPos()
{
	// 플레이어 위치에서 발사되도록
	_shot->GetComponent<Transform>()->SetPosition(this->GetOwner()->GetComponent<Transform>()->GetPosition());
	_shot->GetComponent<Transform>()->SetRotation(
		this->GetOwner()->GetComponent<Transform>()->GetRotation().x,
		this->GetOwner()->GetComponent<Transform>()->GetRotation().y,
		this->GetOwner()->GetComponent<Transform>()->GetRotation().z
	);

	_shot->GetComponent<BoxCollider>()->FixedUpdate();
	_shot->GetComponent<MeshRenderer>()->Update();
	_shot->GetComponent<Particle>()->Update();

	// 방향벡터
	DirectX::XMFLOAT3 currentPoint = _shot->GetComponent<Transform>()->GetPosition();
	DirectX::XMFLOAT3 targetPoint = GRAPHICS->ScreenToWorldPoint(InputSystem::GetInstance()->GetEditorMousePos().x, InputSystem::GetInstance()->GetEditorMousePos().y);

	DirectX::XMVECTOR direction = ToolBox::GetDirectionVec(currentPoint, targetPoint);

	DirectX::XMVECTOR currentPosVec = DirectX::XMLoadFloat3(&currentPoint);

	//플레이어 방향 돌리기
	DirectX::XMFLOAT3 playerPos = this->GetOwner()->GetComponent<Transform>()->GetPosition();
	DirectX::XMFLOAT3 playerRot = this->GetOwner()->GetComponent<Transform>()->GetRotation();
	DirectX::XMVECTOR playerPosVec = DirectX::XMLoadFloat3(&playerPos);
	DirectX::XMVECTOR rotPosVec = DirectX::XMLoadFloat3(&targetPoint);

	DirectX::XMVECTOR playerForward = DirectX::XMVectorSet(0.0f, playerRot.y, -1.0f, 0.0f);
	DirectX::XMVECTOR rotDirection = DirectX::XMVectorSubtract(rotPosVec, playerPosVec);
	rotDirection.m128_f32[1] = 0.0f;
	rotDirection = DirectX::XMVector3Normalize(rotDirection);

	DirectX::XMVECTOR dotResult = DirectX::XMVector3Dot(playerForward, rotDirection);
	float dotProduct = DirectX::XMVectorGetX(dotResult);

	float angle = acos(dotProduct);
	angle = DirectX::XMConvertToDegrees(angle);

	if (rotPosVec.m128_f32[0] > playerPosVec.m128_f32[0])
	{
		angle *= -1;
	}

	this->GetOwner()->GetComponent<Transform>()->SetRotation(0.0f, angle, 0.0f);

	_shot->GetComponent<Projectile>()->SetDirection(direction);
	_shot->GetComponent<Projectile>()->_movedRange = 0.0f;
}

void KunrealEngine::PlayerAbility::CreateAbility1()
{
	Ability* shot = new Ability();
	shot->Initialize("Shot");
	_shotAbil = shot;

	shot->SetTotalData(
		"Shot",			// 이름
		13.0f,			// 데미지
		10.0f,			// 마나
		5.0f,			// 무력화 피해량
		1.5f,			// 쿨타임
		80.0f			// 사거리
	);

	// 객체 저장
	_shot = shot->_projectile;

	// 크기 조정
	_shot->GetComponent<Transform>()->SetScale(0.3f, 0.3, 0.3f);

	// 투사체 컴포넌트 추가
	_shot->AddComponent<Projectile>();
	Projectile* shotProj = _shot->GetComponent<Projectile>();

	// 파티클 추가	// 번개공
	Particle* shotParticle = _shot->AddComponent<Particle>();
	shotParticle->SetParticleEffect("BlastWave2", "Resources/Textures/Particles/fx_BlastWave2.dds", 1000);
	shotParticle->SetParticleSize(7.f, 7.0f);
	shotParticle->SetParticleVelocity(3.0f, true);
	shotParticle->SetParticleDirection(0.0f, 0.0f, 0.0f);
	shotParticle->AddParticleColor(0.5f, 0.5f, 2.0f);
	shotParticle->SetParticleDuration(0.7f, 0.05f);
	shotParticle->SetActive(true);

	_shotParticle2 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("PlayerQ2");
	_shotParticle2->_autoAwake = true;
	_shotParticle2->AddComponent<Particle>();
	_shotParticle2->GetComponent<Particle>()->SetParticleEffect("BlastWave3", "Resources/Textures/Particles/fx_BlastWave3.dds", 1000);
	_shotParticle2->GetComponent<Particle>()->SetParticleDuration(1.0f, 0.05f);
	_shotParticle2->GetComponent<Particle>()->SetParticleVelocity(7.0f, true);
	_shotParticle2->GetComponent<Particle>()->SetParticleSize(10.f, 10.0f);
	_shotParticle2->GetComponent<Particle>()->AddParticleColor(0.0f, 1.5f, 10.f);
	_shotParticle2->GetComponent<Particle>()->SetParticleDirection(0.0f, 0.0f, 0.0f);
	_shotParticle2->GetComponent<Particle>()->SetActive(true);
	_shotParticle2->SetActive(true);
	_shotParticle2->SetParent(_shot);

	_shotParticle3 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("PlayerQ3");
	_shotParticle3->_autoAwake = true;
	_shotParticle3->AddComponent<Particle>();
	_shotParticle3->GetComponent<Particle>()->SetParticleEffect("Cracks1", "Resources/Textures/Particles/fx_Cracks1.dds", 1000);
	_shotParticle3->GetComponent<Particle>()->SetParticleDuration(1.0f, 0.05f);
	_shotParticle3->GetComponent<Particle>()->SetParticleVelocity(200.0f, true);
	_shotParticle3->GetComponent<Particle>()->SetParticleSize(2.0f, 2.0f);
	_shotParticle3->GetComponent<Particle>()->AddParticleColor(0.0f, 1.5f, 10.f);
	_shotParticle3->GetComponent<Particle>()->SetParticleDirection(0.0f, 0.0f, 0.0f);
	_shotParticle3->GetComponent<Particle>()->SetActive(true);
	_shotParticle3->SetActive(true);
	_shotParticle3->SetParent(_shot);

	_shotParticle4 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("PlayerQ4");
	_shotParticle4->_autoAwake = true;
	_shotParticle4->AddComponent<Particle>();
	_shotParticle4->GetComponent<Particle>()->SetParticleEffect("EnergyBolt1", "Resources/Textures/Particles/fx_EnergyBolt1.dds", 1000);
	_shotParticle4->GetComponent<Particle>()->SetParticleDuration(1.0f, 0.05f);
	_shotParticle4->GetComponent<Particle>()->SetParticleVelocity(0.0f, true);
	_shotParticle4->GetComponent<Particle>()->SetParticleSize(1.0f, 1.0f);
	_shotParticle4->GetComponent<Particle>()->AddParticleColor(0.0f, 1.5f, 10.f);
	_shotParticle4->GetComponent<Particle>()->SetParticleDirection(0.0f, 0.0f, 0.0f);
	_shotParticle4->GetComponent<Particle>()->SetActive(true);
	_shotParticle4->SetActive(true);
	_shotParticle4->SetParent(_shot);

	_shotParticleHit1 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("ShotParticleHit1");
	_shotParticleHit1->_autoAwake = true;
	_shotParticleHit1->AddComponent<Particle>();
	_shotParticleHit1->GetComponent<Particle>()->SetParticleEffect("BlastWave1", "Resources/Textures/Particles/fx_BlastWave1.dds", 1000);
	_shotParticleHit1->GetComponent<Particle>()->SetParticleDuration(0.1f, 0.1f);
	_shotParticleHit1->GetComponent<Particle>()->SetParticleVelocity(100.0f, true);
	_shotParticleHit1->GetComponent<Particle>()->SetParticleSize(5.f, 5.0f);
	_shotParticleHit1->GetComponent<Particle>()->AddParticleColor(0.0f, 0.5f, 1.0f);
	_shotParticleHit1->GetComponent<Particle>()->SetParticleDirection(0.0f, 0.0f, 0.0f);
	_shotParticleHit1->GetComponent<Particle>()->SetActive(false);
	_shotParticleHit1->SetActive(true);

	_shotParticleHit2 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("ShotParticleHit2");
	_shotParticleHit2->_autoAwake = true;
	_shotParticleHit2->AddComponent<Particle>();
	_shotParticleHit2->GetComponent<Particle>()->SetParticleEffect("Cracks1", "Resources/Textures/Particles/fx_Cracks1.dds", 1000);
	_shotParticleHit2->GetComponent<Particle>()->SetParticleDuration(1.0f, 0.1f);
	_shotParticleHit2->GetComponent<Particle>()->SetParticleVelocity(150.0f, true);
	_shotParticleHit2->GetComponent<Particle>()->SetParticleSize(3.0f, 3.0f);
	_shotParticleHit2->GetComponent<Particle>()->AddParticleColor(0.0f, 0.5f, 2.0f);
	_shotParticleHit2->GetComponent<Particle>()->SetParticleDirection(0.0f, 0.0f, 0.0f);
	_shotParticleHit2->GetComponent<Particle>()->SetActive(false);
	_shotParticleHit2->SetActive(true);

	_shotParticleHit3 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("ShotParticleHit3");
	_shotParticleHit3->_autoAwake = true;
	_shotParticleHit3->AddComponent<Particle>();
	_shotParticleHit3->GetComponent<Particle>()->SetParticleEffect("Halo2", "Resources/Textures/Particles/fx_Halo2.dds", 1000);
	_shotParticleHit3->GetComponent<Particle>()->SetParticleDuration(0.1f, 0.1f);
	_shotParticleHit3->GetComponent<Particle>()->SetParticleVelocity(100.0f, true);
	_shotParticleHit3->GetComponent<Particle>()->SetParticleSize(15.f, 15.0f);
	_shotParticleHit3->GetComponent<Particle>()->AddParticleColor(0.0f, 1.0f, 5.0f);
	_shotParticleHit3->GetComponent<Particle>()->SetParticleDirection(0.0f, 0.0f, 0.0f);
	_shotParticleHit3->GetComponent<Particle>()->SetActive(false);
	_shotParticleHit3->SetActive(true);


	shotProj->SetMeshObject("Meteor/Meteor");

	// Q 스킬은 메쉬가 안 보이게
	_shot->GetComponent<MeshRenderer>()->SetAlpha(0.0f);

	shotProj->GetCollider()->SetColliderScale(5.0f, 5.0f, 5.0f);
	shotProj->SetDestoryCondition([shot, shotProj, this]()->bool
		{
			if (shotProj->GetCollider()->IsCollided() && shotProj->GetCollider()->GetTargetObject()->GetTag() == "Boss")
			{
				if (_isShotHit)
				{
					//_soundComp->Stop(_energyBallShot);
					_soundComp->Stop(_energyBallFlying);
					_soundComp->Stop(_energyBallExplode);
					_soundComp->Play(_energyBallExplode);
					EventManager::GetInstance().CalculateDamageToBoss(shot);

					// 소멸조건 활성화 시 파티클이 살아있다면 제거
					if (_shotParticleHit1->GetComponent<Particle>()->GetActivated())
					{
						_shotParticleHit1->GetComponent<Particle>()->SetActive(false);
						_shotParticleHit2->GetComponent<Particle>()->SetActive(false);
						_shotParticleHit3->GetComponent<Particle>()->SetActive(false);
						_shotParticleTimer = 0.0f;
						_isShotEnded = false;
					}

					_isShotHit = false;
				}

				return true;
			}
			else if (shotProj->_movedRange > shot->GetRange())
			{
				// 소멸조건 활성화 시 파티클이 살아있다면 제거
				if (_shotParticleHit1->GetComponent<Particle>()->GetActivated())
				{
					_shotParticleHit1->GetComponent<Particle>()->SetActive(false);
					_shotParticleHit2->GetComponent<Particle>()->SetActive(false);
					_shotParticleHit3->GetComponent<Particle>()->SetActive(false);
					_shotParticleTimer = 0.0f;
					_isShotEnded = false;
				}

				return true;
			}
			else
			{
				return false;
			}
		});

	_shot->SetActive(false);

	shot->SetLogic([shot, shotProj, this]()  // 부모가 비활성화 되기전에 파티클을 켜줘야 위치를 받을 수 있음
		{
			if (_shot->GetActivated())
			{
				DirectX::XMFLOAT3 currentPoint = _shot->GetComponent<Transform>()->GetPosition();

				DirectX::XMVECTOR currentPosVec = DirectX::XMLoadFloat3(&currentPoint);
				DirectX::XMVECTOR newPosition = DirectX::XMVectorAdd(currentPosVec, DirectX::XMVectorScale(shotProj->GetDirection(), 50.0f * TimeManager::GetInstance().GetDeltaTime()));

				_shot->GetComponent<Transform>()->SetPosition(newPosition.m128_f32[0], 5.0f, newPosition.m128_f32[2]);
				shotProj->_movedRange += DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(newPosition, currentPosVec)));

				if (shotProj->GetCollider()->IsCollided() && shotProj->GetCollider()->GetTargetObject()->GetTag() == "Boss")
				{
					_isShotEnded = true;
				}
			} 

			if (!(_shot->GetActivated()) && _isShotEnded == true)
			{
				_shotParticleTimer += TimeManager::GetInstance().GetDeltaTime();
				_shotParticleHit1->GetComponent<Particle>()->SetActive(true);
				_shotParticleHit2->GetComponent<Particle>()->SetActive(true);
				_shotParticleHit3->GetComponent<Particle>()->SetActive(true);
				_shotParticleHit1->GetComponent<Transform>()->SetPosition(_shot->GetComponent<Transform>()->GetPosition());
				_shotParticleHit2->GetComponent<Transform>()->SetPosition(_shot->GetComponent<Transform>()->GetPosition());
				_shotParticleHit3->GetComponent<Transform>()->SetPosition(_shot->GetComponent<Transform>()->GetPosition());

				_shotParticleHit1->GetComponent<Particle>()->SetParticleSize(_shotParticleTimer * 40, _shotParticleTimer * 40);
				_shotParticleHit3->GetComponent<Particle>()->SetParticleSize(_shotParticleTimer * 120, _shotParticleTimer * 120);
				EventManager::GetInstance().CamShake(2.0f);

				if (_shotParticleTimer > 0.2f)
				{
					_shotParticleHit1->GetComponent<Particle>()->SetActive(false);
					_shotParticleHit2->GetComponent<Particle>()->SetActive(false);
					_shotParticleHit3->GetComponent<Particle>()->SetActive(false);
					_shotParticleTimer = 0.0f;
					_isShotEnded = false;
				}
			}
		});

	AddToContanier(shot);
}


void KunrealEngine::PlayerAbility::ResetCirclePos()
{
	// 플레이어 위치에 생성되도록
	_circle->GetComponent<Transform>()->SetPosition(this->GetOwner()->GetComponent<Transform>()->GetPosition().x, this->GetOwner()->GetComponent<Transform>()->GetPosition().y - 0.8f, this->GetOwner()->GetComponent<Transform>()->GetPosition().z);

	// 크기 초기화
	_circle->GetComponent<Transform>()->SetTotalScale(0.0f);

	// 위치 조정
	//_ice->GetComponent<BoxCollider>()->FixedUpdate();
	_circle->GetComponent<MeshRenderer>()->Update();
	//_ice->GetComponent<Particle>()->Update();
}

void KunrealEngine::PlayerAbility::CreateAbility2()
{
	Ability* circle = new Ability();
	circle->Initialize("Circle");
	_circleAbil = circle;

	circle->SetTotalData(
		"Circle",		// 이름
		10.0f,			// 데미지
		15.0f,			// 마나
		0.0f,			// 무력화 피해량
		12.0f,			// 쿨타임
		12.0f			// 사거리
	);


	_circle = circle->_projectile;

	// 크기 조정
	// 시작은 작게
	_circle->GetComponent<Transform>()->SetTotalScale(0.01f);
	_circle->AddComponent<MeshRenderer>()->SetMeshObject("MagicCircle/MagicCircle");
	_circle->AddComponent<CylinderCollider>();
	_circle->GetCollider()->SetColliderScale(75.0f, 20.0f, 75.0f);
	_circle->GetCollider()->SetOffset(0.0f, 10.0f, 0.0f);

	_circle->SetActive(false);

	_circleParticle = SceneManager::GetInstance().GetCurrentScene()->CreateObject("CircleParticle");
	_circleParticle->_autoAwake = true;
	_circleParticle->AddComponent<Particle>();
	_circleParticle->GetComponent<Particle>()->SetParticleEffect("fx_BlastWave5", "Resources/Textures/Particles/fx_BlastWave5.dds", 1000);
	_circleParticle->GetComponent<Particle>()->SetParticleDuration(1.0f, 0.6f);
	_circleParticle->GetComponent<Particle>()->SetParticleVelocity(0.4f, true);
	_circleParticle->GetComponent<Particle>()->SetParticleSize(100.0f, 100.0f);
	_circleParticle->GetComponent<Particle>()->AddParticleColor(0.0f, 0.5f, 2.0f);
	_circleParticle->GetComponent<Particle>()->SetParticleDirection(0.0f, 50.0f, 0.0f);
	_circleParticle->GetComponent<Particle>()->SetParticleCameraApply(true);
	_circleParticle->GetComponent<Particle>()->SetActive(false);
	_circleParticle->SetParent(_circle);

	_circleBuffParticle1 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("CircleBuffParticle1");
	_circleBuffParticle1->_autoAwake = true;
	_circleBuffParticle1->AddComponent<Particle>();
	_circleBuffParticle1->GetComponent<Particle>()->SetParticleEffect("fx_BlastWave1", "Resources/Textures/Particles/fx_BlastWave1.dds", 1000);
	_circleBuffParticle1->GetComponent<Particle>()->SetParticleDuration(0.9f, 1.0f);
	_circleBuffParticle1->GetComponent<Particle>()->SetParticleVelocity(0.4f, true);
	_circleBuffParticle1->GetComponent<Particle>()->SetParticleSize(9.0f, 9.0f);
	_circleBuffParticle1->GetComponent<Particle>()->AddParticleColor(0.0f, 0.05f, 4.0f);
	_circleBuffParticle1->GetComponent<Particle>()->SetParticleDirection(0.0f, 0.0f, 0.0f);
	_circleBuffParticle1->GetComponent<Particle>()->SetParticleCameraApply(true);
	_circleBuffParticle1->GetComponent<Particle>()->SetActive(false);

	_circleBuffParticle2 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("CircleBuffParticle2");
	_circleBuffParticle2->_autoAwake = true;
	_circleBuffParticle2->AddComponent<Particle>();
	_circleBuffParticle2->GetComponent<Particle>()->SetParticleEffect("Lightning2", "Resources/Textures/Particles/fx_Lightning2.dds", 1000);
	_circleBuffParticle2->GetComponent<Particle>()->SetParticleDuration(0.8f, 2.0f);
	_circleBuffParticle2->GetComponent<Particle>()->SetParticleVelocity(10.f, true);
	_circleBuffParticle2->GetComponent<Particle>()->SetParticleSize(15.0f, 15.0f);
	_circleBuffParticle2->GetComponent<Particle>()->AddParticleColor(0.0f, 0.5f, 5.0f);
	_circleBuffParticle2->GetComponent<Particle>()->SetParticleDirection(0.0f, 40.0f, 0.0f);
	//_circleBuffParticle2->GetComponent<Particle>()->SetOffSet(0.0f, 8.0f, 0.0f);
	//_circleBuffParticle2->GetComponent<Particle>()->SetParticleCameraApply(true);
	_circleBuffParticle2->GetComponent<Particle>()->SetActive(false);

	circle->SetLogic([circle, this]()
		{
			if (_circle->GetActivated())
			{
				if (_circle->GetCollider()->IsCollided() && _circle->GetCollider()->GetTargetObject() == this->GetOwner())
				{
					this->_playerComp->_playerInfo._spellPower = 1.5f;
					_circleBuffParticle1->GetComponent<Particle>()->SetActive(true);
					_circleBuffParticle2->GetComponent<Particle>()->SetActive(true);

					_circleBuffParticle1->GetComponent<Transform>()->SetPosition(this->GetOwner()->GetComponent<Transform>()->GetPosition());
					_circleBuffParticle2->GetComponent<Transform>()->SetPosition(this->GetOwner()->GetComponent<MeshRenderer>()->GetBoneTransform("spine_02")._41,
						this->GetOwner()->GetComponent<MeshRenderer>()->GetBoneTransform("spine_02")._42,
						this->GetOwner()->GetComponent<MeshRenderer>()->GetBoneTransform("spine_02")._43);
				}
				else
				{
					this->_playerComp->_playerInfo._spellPower = 1.0f;
					_circleBuffParticle1->GetComponent<Particle>()->SetActive(false);
					_circleBuffParticle2->GetComponent<Particle>()->SetActive(false);
				}

				if (this->_isCircleHit && _circle->GetCollider()->IsCollided() && _circle->GetCollider()->GetTargetObject()->GetTag() == "Boss")
				{
					EventManager::GetInstance().CalculateDamageToBoss(circle);
					this->_isCircleHit = false;
				}

				// 회전시키기
				if (_circle->GetComponent<Transform>()->GetRotation().y < 360.0f)
				{
					_circle->GetComponent<Transform>()->SetRotation(_circle->GetComponent<Transform>()->GetRotation().x, _circle->GetComponent<Transform>()->GetRotation().y + 0.1f, _circle->GetComponent<Transform>()->GetRotation().z);
				}
				else
				{
					_circle->GetComponent<Transform>()->SetRotation(0.0f, 0.0f, 0.0f);
				}

				if (_circle->GetComponent<Transform>()->GetScale().x < 0.5f && !_destroyCircle)
				{
					_circle->GetComponent<Transform>()->SetTotalScale(_circle->GetComponent<Transform>()->GetScale().x + 0.01f);
				}
				else
				{
					_destroyCircle = true;
					_circleParticle->GetComponent<Particle>()->SetParticleSize(100.0f, 100.0f);
					_circleParticle->GetComponent<Particle>()->SetActive(true);

					Startcoroutine(CircleFadeOut);
				}
			}

		});

	AddToContanier(circle);
}


void KunrealEngine::PlayerAbility::ResetLaserPos()
{
	this->_laser->GetComponent<Transform>()->SetPosition(this->GetOwner()->GetComponent<Transform>()->GetPosition());

	// 플레이어 방향조절
	DirectX::XMFLOAT3 newPos = GRAPHICS->ScreenToWorldPoint(InputSystem::GetInstance()->GetEditorMousePos().x, InputSystem::GetInstance()->GetEditorMousePos().y);

	//플레이어 방향 돌리기
	DirectX::XMFLOAT3 playerPos = this->GetOwner()->GetComponent<Transform>()->GetPosition();
	DirectX::XMFLOAT3 playerRot = this->GetOwner()->GetComponent<Transform>()->GetRotation();
	DirectX::XMVECTOR playerPosVec = DirectX::XMLoadFloat3(&playerPos);
	DirectX::XMVECTOR rotPosVec = DirectX::XMLoadFloat3(&newPos);

	DirectX::XMVECTOR playerForward = DirectX::XMVectorSet(0.0f, playerRot.y, -1.0f, 0.0f);
	DirectX::XMVECTOR rotDirection = DirectX::XMVectorSubtract(rotPosVec, playerPosVec);
	rotDirection.m128_f32[1] = 0.0f;
	rotDirection = DirectX::XMVector3Normalize(rotDirection);

	DirectX::XMVECTOR dotResult = DirectX::XMVector3Dot(playerForward, rotDirection);
	float dotProduct = DirectX::XMVectorGetX(dotResult);

	float angle = acos(dotProduct);
	angle = DirectX::XMConvertToDegrees(angle);

	if (rotPosVec.m128_f32[0] > playerPosVec.m128_f32[0])
	{
		angle *= -1;
	}

	this->GetOwner()->GetComponent<Transform>()->SetRotation(0.0f, angle, 0.0f);
	this->_laser->GetComponent<Transform>()->SetRotation(this->GetOwner()->GetComponent<Transform>()->GetRotation());

	DirectX::XMVECTOR newLaserPos = DirectX::XMVectorAdd(playerPosVec, DirectX::XMVectorScale(rotDirection, this->_laser->GetComponent<BoxCollider>()->GetColliderScale().z * 0.5f + 10.0f));

	this->_laser->GetComponent<Transform>()->SetPosition(newLaserPos.m128_f32[0], 5.0f, newLaserPos.m128_f32[2]);

	// 그을림 객체 위치값
	DirectX::XMVECTOR cratorPos1 = DirectX::XMVectorAdd(playerPosVec, DirectX::XMVectorScale(rotDirection, 25.0f));
	DirectX::XMVECTOR cratorPos2 = DirectX::XMVectorAdd(playerPosVec, DirectX::XMVectorScale(rotDirection, 50.0f));
	DirectX::XMVECTOR cratorPos3 = DirectX::XMVectorAdd(playerPosVec, DirectX::XMVectorScale(rotDirection, 75.0f));
	DirectX::XMVECTOR cratorPos4 = DirectX::XMVectorAdd(playerPosVec, DirectX::XMVectorScale(rotDirection, 100.0f));
	DirectX::XMVECTOR cratorPos5 = DirectX::XMVectorAdd(playerPosVec, DirectX::XMVectorScale(rotDirection, 125.0f));
	DirectX::XMVECTOR cratorPos6 = DirectX::XMVectorAdd(playerPosVec, DirectX::XMVectorScale(rotDirection, 150.0f));

	this->_laserCrator1->GetComponent<Transform>()->SetPosition(cratorPos1.m128_f32[0], 20.0f, cratorPos1.m128_f32[2]);
	this->_laserCrator2->GetComponent<Transform>()->SetPosition(cratorPos2.m128_f32[0], 20.0f, cratorPos2.m128_f32[2]);
	this->_laserCrator3->GetComponent<Transform>()->SetPosition(cratorPos3.m128_f32[0], 20.0f, cratorPos3.m128_f32[2]);
	this->_laserCrator4->GetComponent<Transform>()->SetPosition(cratorPos4.m128_f32[0], 20.0f, cratorPos4.m128_f32[2]);
	this->_laserCrator5->GetComponent<Transform>()->SetPosition(cratorPos5.m128_f32[0], 20.0f, cratorPos5.m128_f32[2]);
	this->_laserCrator6->GetComponent<Transform>()->SetPosition(cratorPos6.m128_f32[0], 20.0f, cratorPos6.m128_f32[2]);

	// 파티클 위치조정 부분

	float laserPosOffset = 15.0f;
	float laserScaleOffset = 80.0f;

	DirectX::XMFLOAT3 direction = ToolBox::RotateVector(DirectX::XMFLOAT3(0, 0, -1.0f), _playerComp->_transform->GetRotation().y);

	DirectX::XMVECTOR dirVec = DirectX::XMLoadFloat3(&direction);

	DirectX::XMVECTOR laserScaleDir = DirectX::XMVectorScale(dirVec, laserPosOffset);
	_laserParticle1->GetComponent<Transform>()->SetPosition(_playerComp->_transform->GetPosition().x + laserScaleDir.m128_f32[0], 16.0f, _playerComp->_transform->GetPosition().z + laserScaleDir.m128_f32[2]);
	_laserParticle2->GetComponent<Transform>()->SetPosition(_playerComp->_transform->GetPosition().x + laserScaleDir.m128_f32[0], 16.0f, _playerComp->_transform->GetPosition().z + laserScaleDir.m128_f32[2]);
	_laserParticle3->GetComponent<Transform>()->SetPosition(_playerComp->_transform->GetPosition().x + laserScaleDir.m128_f32[0], 16.0f, _playerComp->_transform->GetPosition().z + laserScaleDir.m128_f32[2]);
	_laserParticle4->GetComponent<Transform>()->SetPosition(_playerComp->_transform->GetPosition().x + laserScaleDir.m128_f32[0], 16.0f, _playerComp->_transform->GetPosition().z + laserScaleDir.m128_f32[2]);

	_laserParticle1->GetComponent<Particle>()->SetParticleRotation(90.0f, _playerComp->_transform->GetRotation().y, 0.0f);
	_laserParticle2->GetComponent<Particle>()->SetParticleRotation(90.0f, _playerComp->_transform->GetRotation().y, 0.0f);
	_laserParticle3->GetComponent<Particle>()->SetParticleRotation(90.0f, _playerComp->_transform->GetRotation().y, 0.0f);
	_laserParticle4->GetComponent<Particle>()->SetParticleRotation(0.0f, _playerComp->_transform->GetRotation().y, 0.0f);

	_laserParticle1->GetComponent<Particle>()->Update();
	_laserParticle2->GetComponent<Particle>()->Update();
	_laserParticle3->GetComponent<Particle>()->Update();
	_laserParticle4->GetComponent<Particle>()->Update();
}

void KunrealEngine::PlayerAbility::CreateAbility3()
{
	Ability* laser = new Ability();
	laser->Initialize("Laser");
	_laserAbil = laser;

	laser->SetTotalData(
		"Laser",			// 이름
		10.0f,			// 데미지
		20.0f,			// 마나
		15.0f,			// 무력화 피해량
		10.0f,			// 쿨타임
		15.0f			// 사거리
	);

	_laser = laser->_projectile;

	// 크기 조정			/// 정밀 조정 필요
	//_laser->GetComponent<Transform>()->SetScale(20.0f, 20.0f, 20.0f);
	_laser->GetComponent<Transform>()->SetRotation(90.0f, 0.0f, 0.0f);

	// 투사체 컴포넌트 추가
	_laser->AddComponent<BoxCollider>();
	BoxCollider* laserCollider = _laser->GetComponent<BoxCollider>();
	laserCollider->SetColliderScale(20.0f, 20.0f, 160.0f);

	_laserParticle1 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("PlayerE1");
	_laserParticle1->_autoAwake = true;
	_laserParticle1->AddComponent<Particle>();
	_laserParticle1->GetComponent<Particle>()->SetParticleEffect("fx_SmokeyHalo1", "Resources/Textures/Particles/fx_SmokeyHalo1.dds", 1000);
	_laserParticle1->GetComponent<Particle>()->SetParticleDuration(3.0f, 2.0f);
	_laserParticle1->GetComponent<Particle>()->SetParticleVelocity(84.f, false);
	_laserParticle1->GetComponent<Particle>()->SetParticleSize(50.f, 50.0f);
	_laserParticle1->GetComponent<Particle>()->AddParticleColor(0.0f, 0.5f, 1.0f);
	_laserParticle1->GetComponent<Particle>()->SetParticleRotation(90.0f, _playerComp->_transform->GetRotation().y, 0.0f);
	_laserParticle1->GetComponent<Particle>()->SetParticleCameraApply(true);
	_laserParticle1->GetComponent<Particle>()->SetParticleAngle(339, 0.0f, 30);
	_laserParticle1->SetTotalComponentState(false);
	_laserParticle1->SetActive(false);

	_laserParticle2 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("PlayerE2");
	_laserParticle2->_autoAwake = true;
	_laserParticle2->AddComponent<Particle>();
	_laserParticle2->GetComponent<Particle>()->SetParticleEffect("Laser", "Resources/Textures/Particles/fx_Dust2.dds", 1000);
	_laserParticle2->GetComponent<Particle>()->SetParticleDuration(3.0f, 2.0f);
	_laserParticle2->GetComponent<Particle>()->SetParticleVelocity(84.f, false);
	_laserParticle2->GetComponent<Particle>()->SetParticleSize(20.f, 20.f);
	_laserParticle2->GetComponent<Particle>()->AddParticleColor(1.0f, 1.0f, 0.0f);
	_laserParticle2->GetComponent<Particle>()->SetParticleRotation(90.0f, _playerComp->_transform->GetRotation().y, 0.0f);
	_laserParticle2->GetComponent<Particle>()->SetParticleCameraApply(true);
	_laserParticle2->GetComponent<Particle>()->SetParticleAngle(339, 0.0f, 30);
	_laserParticle2->SetTotalComponentState(false);
	_laserParticle2->SetActive(false);

	_laserParticle3 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("PlayerE3");
	_laserParticle3->_autoAwake = true;
	_laserParticle3->AddComponent<Particle>();
	_laserParticle3->GetComponent<Particle>()->SetParticleEffect("fx_BlastWave5", "Resources/Textures/Particles/fx_BlastWave5.dds", 1000);
	_laserParticle3->GetComponent<Particle>()->SetParticleDuration(3.0f, 2.0f);
	_laserParticle3->GetComponent<Particle>()->SetParticleVelocity(84.f, false);
	_laserParticle3->GetComponent<Particle>()->SetParticleSize(30.f, 30.f);
	_laserParticle3->GetComponent<Particle>()->AddParticleColor(1.0f, 1.0f, 0.3f);
	_laserParticle3->GetComponent<Particle>()->SetParticleRotation(90.0f, _playerComp->_transform->GetRotation().y, 0.0f);
	_laserParticle3->GetComponent<Particle>()->SetParticleCameraApply(true);
	_laserParticle3->GetComponent<Particle>()->SetParticleAngle(339, 0.0f, 30);
	_laserParticle3->SetTotalComponentState(false);
	_laserParticle3->SetActive(false);

	_laserParticle4 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("PlayerE4");
	_laserParticle4->_autoAwake = true;
	_laserParticle4->AddComponent<Particle>();
	_laserParticle4->GetComponent<Particle>()->SetParticleEffect("fx_Flare7", "Resources/Textures/Particles/fx_Flare7.dds", 1000);
	_laserParticle4->GetComponent<Particle>()->SetParticleDuration(3.0f, 0.1f);
	_laserParticle4->GetComponent<Particle>()->SetParticleVelocity(0.f, true);
	_laserParticle4->GetComponent<Particle>()->SetParticleSize(30.f, 30.f);
	_laserParticle4->GetComponent<Particle>()->AddParticleColor(0.0f, 0.5f, 1.0f);
	_laserParticle4->GetComponent<Particle>()->SetParticleRotation(90.0f, _playerComp->_transform->GetRotation().y, 0.0f);
	_laserParticle4->GetComponent<Particle>()->SetParticleCameraApply(true);
	_laserParticle4->GetComponent<Particle>()->SetParticleAngle(339, 0.0f, 30);
	_laserParticle4->SetTotalComponentState(false);
	_laserParticle4->SetActive(false);

	_laser->SetActive(false);

	_laserCrator1 = this->GetOwner()->GetObjectScene()->CreateObject("LaserCrator1");
	_laserCrator2 = this->GetOwner()->GetObjectScene()->CreateObject("LaserCrator2");
	_laserCrator3 = this->GetOwner()->GetObjectScene()->CreateObject("LaserCrator3");
	_laserCrator4 = this->GetOwner()->GetObjectScene()->CreateObject("LaserCrator4");
	_laserCrator5 = this->GetOwner()->GetObjectScene()->CreateObject("LaserCrator5");
	_laserCrator6 = this->GetOwner()->GetObjectScene()->CreateObject("LaserCrator6");

	_laserCrator1->_autoAwake = true;
	_laserCrator2->_autoAwake = true;
	_laserCrator3->_autoAwake = true;
	_laserCrator4->_autoAwake = true;
	_laserCrator5->_autoAwake = true;
	_laserCrator6->_autoAwake = true;

	_laserCrator1->AddComponent<TransparentMesh>();
	_laserCrator2->AddComponent<TransparentMesh>();
	_laserCrator3->AddComponent<TransparentMesh>();
	_laserCrator4->AddComponent<TransparentMesh>();
	_laserCrator5->AddComponent<TransparentMesh>();
	_laserCrator6->AddComponent<TransparentMesh>();

	_laserCrator1->GetComponent<TransparentMesh>()->CreateTMesh("LaserCrator1", "Resources/Textures/Decal/Decal.png", 0.6f);
	_laserCrator2->GetComponent<TransparentMesh>()->CreateTMesh("LaserCrator2", "Resources/Textures/Decal/Decal.png", 0.6f);
	_laserCrator3->GetComponent<TransparentMesh>()->CreateTMesh("LaserCrator3", "Resources/Textures/Decal/Decal.png", 0.6f);
	_laserCrator4->GetComponent<TransparentMesh>()->CreateTMesh("LaserCrator4", "Resources/Textures/Decal/Decal.png", 0.6f);
	_laserCrator5->GetComponent<TransparentMesh>()->CreateTMesh("LaserCrator5", "Resources/Textures/Decal/Decal.png", 0.6f);
	_laserCrator6->GetComponent<TransparentMesh>()->CreateTMesh("LaserCrator6", "Resources/Textures/Decal/Decal.png", 0.6f);

	_laserCrator1->GetComponent<TransparentMesh>()->SetTimer(5.0f);
	_laserCrator2->GetComponent<TransparentMesh>()->SetTimer(5.0f);
	_laserCrator3->GetComponent<TransparentMesh>()->SetTimer(5.0f);
	_laserCrator4->GetComponent<TransparentMesh>()->SetTimer(5.0f);
	_laserCrator5->GetComponent<TransparentMesh>()->SetTimer(5.0f);
	_laserCrator6->GetComponent<TransparentMesh>()->SetTimer(5.0f);

	_laserCrator1->GetComponent<TransparentMesh>()->SetDecal(true);
	_laserCrator2->GetComponent<TransparentMesh>()->SetDecal(true);
	_laserCrator3->GetComponent<TransparentMesh>()->SetDecal(true);
	_laserCrator4->GetComponent<TransparentMesh>()->SetDecal(true);
	_laserCrator5->GetComponent<TransparentMesh>()->SetDecal(true);
	_laserCrator6->GetComponent<TransparentMesh>()->SetDecal(true);

	_laserCrator1->GetComponent<TransparentMesh>()->SetActive(true);
	_laserCrator2->GetComponent<TransparentMesh>()->SetActive(true);
	_laserCrator3->GetComponent<TransparentMesh>()->SetActive(true);
	_laserCrator4->GetComponent<TransparentMesh>()->SetActive(true);
	_laserCrator5->GetComponent<TransparentMesh>()->SetActive(true);
	_laserCrator6->GetComponent<TransparentMesh>()->SetActive(true);

	_laserCrator1->GetComponent<TransparentMesh>()->Reset();
	_laserCrator2->GetComponent<TransparentMesh>()->Reset();
	_laserCrator3->GetComponent<TransparentMesh>()->Reset();
	_laserCrator4->GetComponent<TransparentMesh>()->Reset();
	_laserCrator5->GetComponent<TransparentMesh>()->Reset();
	_laserCrator6->GetComponent<TransparentMesh>()->Reset();

	_laserCrator1->GetComponent<Transform>()->SetScale(40.0f, 200.0f, 40.0f);
	_laserCrator2->GetComponent<Transform>()->SetScale(40.0f, 200.0f, 40.0f);
	_laserCrator3->GetComponent<Transform>()->SetScale(40.0f, 200.0f, 40.0f);
	_laserCrator4->GetComponent<Transform>()->SetScale(40.0f, 200.0f, 40.0f);
	_laserCrator5->GetComponent<Transform>()->SetScale(40.0f, 200.0f, 40.0f);
	_laserCrator6->GetComponent<Transform>()->SetScale(40.0f, 200.0f, 40.0f);

	laser->SetLogic([laser, laserCollider, this]()
		{
			if (_isLaserStarted == true)
			{
				_laserParticle1->GetComponent<Particle>()->SetParticleSize(50.f * ToolBox::GetRandomFloat(0.8f, 1.0f), 50.f * ToolBox::GetRandomFloat(0.8f, 1.0f));
				_laserParticle2->GetComponent<Particle>()->SetParticleSize(30.f * ToolBox::GetRandomFloat(0.8f, 1.0f), 30.f * ToolBox::GetRandomFloat(0.8f, 1.0f));
				_laserParticle3->GetComponent<Particle>()->SetParticleSize(60.f * ToolBox::GetRandomFloat(0.8f, 1.0f), 60.f * ToolBox::GetRandomFloat(0.8f, 1.0f));
				_laserParticle4->GetComponent<Particle>()->SetParticleSize(50.f * ToolBox::GetRandomFloat(0.8f, 1.0f), 50.f * ToolBox::GetRandomFloat(0.8f, 1.0f));
			}

			if (laserCollider->GetTargetObject() != nullptr && laserCollider->IsCollided() && laserCollider->GetTargetObject()->GetTag() == "Boss" && this->_isLaserHit)
			{
				EventManager::GetInstance().CalculateDamageToBoss(laser);
				this->_isLaserHit = false;
			}

			if (!this->_isLaserHit && this->_laser->GetActivated())
			{
				EventManager::GetInstance().CamShake(0.5f);
				Startcoroutine(laserHit);
			}
		});

	AddToContanier(laser);
}

void KunrealEngine::PlayerAbility::ResetMeteorPos()
{
	// 마우스 3D좌표로부터 80.0f 위에 위치시키도록
	DirectX::XMFLOAT3 newPos = GRAPHICS->ScreenToWorldPoint(InputSystem::GetInstance()->GetEditorMousePos().x, InputSystem::GetInstance()->GetEditorMousePos().y);

	_meteor->GetComponent<Transform>()->SetPosition(newPos);
	_meteor->GetComponent<Transform>()->SetPosition
	(
		_meteor->GetComponent<Transform>()->GetPosition().x,
		_meteor->GetComponent<Transform>()->GetPosition().y + 80.0f,
		_meteor->GetComponent<Transform>()->GetPosition().z
	);

	// 그래픽스 부분들 Transform 업데이트
	_meteor->GetComponent<BoxCollider>()->FixedUpdate();
	_meteor->GetComponent<Light>()->Update();
	_meteor->GetComponent<MeshRenderer>()->Update();

	//플레이어 방향 돌리기
	DirectX::XMFLOAT3 playerPos = this->GetOwner()->GetComponent<Transform>()->GetPosition();
	DirectX::XMFLOAT3 playerRot = this->GetOwner()->GetComponent<Transform>()->GetRotation();
	DirectX::XMVECTOR playerPosVec = DirectX::XMLoadFloat3(&playerPos);
	DirectX::XMVECTOR rotPosVec = DirectX::XMLoadFloat3(&newPos);

	DirectX::XMVECTOR playerForward = DirectX::XMVectorSet(0.0f, playerRot.y, -1.0f, 0.0f);
	DirectX::XMVECTOR rotDirection = DirectX::XMVectorSubtract(rotPosVec, playerPosVec);
	rotDirection.m128_f32[1] = 0.0f;
	rotDirection = DirectX::XMVector3Normalize(rotDirection);

	DirectX::XMVECTOR dotResult = DirectX::XMVector3Dot(playerForward, rotDirection);
	float dotProduct = DirectX::XMVectorGetX(dotResult);

	float angle = acos(dotProduct);
	angle = DirectX::XMConvertToDegrees(angle);

	if (rotPosVec.m128_f32[0] > playerPosVec.m128_f32[0])
	{
		angle *= -1;
	}

	this->GetOwner()->GetComponent<Transform>()->SetRotation(0.0f, angle, 0.0f);

	// 그을림 객체 위치값
	DirectX::XMFLOAT3 cratorPos = this->_meteor->GetComponent<Transform>()->GetPosition();
	cratorPos.y = 20.0f;

	this->_meteorCrator->GetComponent<Transform>()->SetPosition(cratorPos);
}


bool KunrealEngine::PlayerAbility::CheckMeteorRange()
{
	DirectX::XMFLOAT3 mousePos = GRAPHICS->ScreenToWorldPoint(InputSystem::GetInstance()->GetEditorMousePos().x, InputSystem::GetInstance()->GetEditorMousePos().y);

	bool isOnmap = Navigation::GetInstance().GetPositionOnMap(0, mousePos.x, mousePos.y, mousePos.z);

	return isOnmap;
}

void KunrealEngine::PlayerAbility::CreateAbility4()
{
	Ability* meteor = new Ability();
	meteor->Initialize("Meteor");
	_meteorAbil = meteor;

	meteor->SetTotalData(
		"Meteor",			// 이름
		70.0f,			// 데미지
		10.0f,			// 마나
		15.0f,			// 무력화 피해량
		15.0f,			// 쿨타임
		15.0f			// 사거리
	);

	// 객체 저장
	_meteor = meteor->_projectile;

	// 우리 에셋으론 세워야 이쁘다
	_meteor->GetComponent<Transform>()->SetRotation(0.0f, 0.0f, 90.0f);

	// 투사체 컴포넌트 추가
	_meteor->AddComponent<Projectile>();
	Projectile* meteorProj = _meteor->GetComponent<Projectile>();

	// Mesh 및 크기 설정
	_meteor->GetComponent<Transform>()->SetScale(1.5f, 1.5f, 1.5f);

	meteorProj->SetMeshObject("Meteor/Meteor");
	meteorProj->GetCollider()->SetColliderScale(15.0f, 60.0f, 60.0f);

	// 운석 범위 표시 객체
	this->_meteorRange = this->GetOwner()->GetObjectScene()->CreateObject("MeteorRange");
	this->_meteorRange->AddComponent<MeteorRange>();
	this->_meteorRange->GetComponent<MeteorRange>()->SetActive(false);
	this->_meteorRange->SetActive(false);
	this->_meteorRange->_autoAwake = true;

	// 낙하 후 그을림 표현 객체
	this->_meteorCrator = this->GetOwner()->GetObjectScene()->CreateObject("MeteorCrater");
	this->_meteorCrator->AddComponent<TransparentMesh>();
	this->_meteorCrator->GetComponent<TransparentMesh>()->CreateTMesh("MeteorCrater", "Resources/Textures/Decal/Decal.png", 0.6f);
	this->_meteorCrator->GetComponent<TransparentMesh>()->SetTimer(5.0f);
	this->_meteorCrator->GetComponent<TransparentMesh>()->SetDecal(true);
	this->_meteorCrator->GetComponent<Transform>()->SetScale(100.0f, 100.0f, 100.0f);
	this->_meteorCrator->GetComponent<TransparentMesh>()->Reset();
	this->_meteorCrator->SetActive(false);

	// 파티클 추가	// 불타오르게
	Particle* meteorParticle = _meteor->AddComponent<Particle>();
	//Particle* meteorParticle = _meteor->GetComponent<Particle>();
	meteorParticle->SetParticleEffect("BlastWave2", "Resources/Textures/Particles/fx_BlastWave2.dds", 1000);
	//meteorParticle->SetParticleSize(17.f, 20.0f);
	meteorParticle->SetParticleSize(25.0f, 30.0f);
	meteorParticle->SetParticleVelocity(3.0f, true);
	meteorParticle->SetParticleDuration(1.5f, 1.5f);
	meteorParticle->AddParticleColor(6.0f, 0.2f, 0.1f);
	meteorParticle->SetParticleDirection(0.0f, 50.0f, 0.0f);
	meteorParticle->SetActive(true);

	_meteorParticle2 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("PlayerR2");
	_meteorParticle2->_autoAwake = true;
	_meteorParticle2->AddComponent<Particle>();
	_meteorParticle2->GetComponent<Particle>()->SetParticleEffect("BlastWave3", "Resources/Textures/Particles/fx_BlastWave3.dds", 1000);
	_meteorParticle2->GetComponent<Particle>()->SetParticleDuration(2.0f, 1.4f);
	_meteorParticle2->GetComponent<Particle>()->SetParticleVelocity(6.5f, true);
	//_meteorParticle2->GetComponent<Particle>()->SetParticleSize(17.f, 20.0f);
	_meteorParticle2->GetComponent<Particle>()->SetParticleSize(25.0f, 30.0f);
	_meteorParticle2->GetComponent<Particle>()->AddParticleColor(2.0f, 1.0f, 0.0f);
	_meteorParticle2->GetComponent<Particle>()->SetParticleDirection(0.0f, 50.0f, 0.0f);
	_meteorParticle2->SetParent(_meteor);


	_meteorParticle3 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("PlayerR3");
	_meteorParticle3->_autoAwake = true;
	_meteorParticle3->GetComponent<Transform>()->SetPosition(0, 10.0f, 0);
	_meteorParticle3->AddComponent<Particle>();
	_meteorParticle3->GetComponent<Particle>()->SetParticleEffect("Fire1", "Resources/Textures/Particles/fx_Fire1.dds", 1000);
	_meteorParticle3->GetComponent<Particle>()->SetParticleDuration(1.0f, 0.6f);
	_meteorParticle3->GetComponent<Particle>()->SetParticleVelocity(6.0f, true);
	//_meteorParticle3->GetComponent<Particle>()->SetParticleSize(13.f, 13.0f);
	_meteorParticle3->GetComponent<Particle>()->SetParticleSize(19.0f, 19.0f);
	_meteorParticle3->GetComponent<Particle>()->AddParticleColor(1.0f, 0.1f, 0.1f);
	_meteorParticle3->GetComponent<Particle>()->SetParticleDirection(0.0f, 100.0f, 0.0f);
	_meteorParticle3->SetParent(_meteor);

	_meteorParticle4 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("PlayerR4");
	_meteorParticle4->_autoAwake = true;
	_meteorParticle4->GetComponent<Transform>()->SetPosition(0, 10.f, 0.f);
	_meteorParticle4->AddComponent<Particle>();
	_meteorParticle4->GetComponent<Particle>()->SetParticleEffect("Fire1", "Resources/Textures/Particles/fx_Fire1.dds", 1000);
	_meteorParticle4->GetComponent<Particle>()->SetParticleDuration(1.0f, 0.6f);
	_meteorParticle4->GetComponent<Particle>()->SetParticleVelocity(6.0f, true);
	//_meteorParticle4->GetComponent<Particle>()->SetParticleSize(1.f, 1.0f);
	_meteorParticle4->GetComponent<Particle>()->SetParticleSize(1.5f, 1.5f);
	_meteorParticle4->GetComponent<Particle>()->AddParticleColor(1.0f, 0.1f, 0.1f);
	_meteorParticle4->GetComponent<Particle>()->SetParticleDirection(0.0f, 100.0f, 0.0f);
	_meteorParticle4->SetParent(_meteor);

	// 착탄 파티클

	_meteorParticleHit1 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("MeteorParticleHit1");
	_meteorParticleHit1->_autoAwake = true;
	_meteorParticleHit1->AddComponent<Particle>();
	_meteorParticleHit1->GetComponent<Particle>()->SetParticleEffect("Flare6", "Resources/Textures/Particles/fx_Twister3.dds", 1000);
	_meteorParticleHit1->GetComponent<Particle>()->SetParticleDuration(2.4f, 2.0f);
	_meteorParticleHit1->GetComponent<Particle>()->SetParticleVelocity(1.0f, true);
	_meteorParticleHit1->GetComponent<Particle>()->SetParticleSize(1.0f, 1.f);
	_meteorParticleHit1->GetComponent<Particle>()->AddParticleColor(1.f, 1.f, 1.f);
	_meteorParticleHit1->GetComponent<Particle>()->SetParticleDirection(0.0f, 400.0f, 0.0f);
	_meteorParticleHit1->GetComponent<Particle>()->SetActive(false);
	_meteorParticleHit1->SetActive(true);

	_meteorParticleHit2 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("MeteorParticleHit2");
	_meteorParticleHit2->_autoAwake = true;
	_meteorParticleHit2->AddComponent<Particle>();
	_meteorParticleHit2->GetComponent<Particle>()->SetParticleEffect("Halo1", "Resources/Textures/Particles/fx_Halo1.dds", 1000);
	_meteorParticleHit2->GetComponent<Particle>()->SetParticleDuration(8.0f, 0.5f);
	_meteorParticleHit2->GetComponent<Particle>()->SetParticleVelocity(10.0f, true);
	_meteorParticleHit2->GetComponent<Particle>()->SetParticleSize(1.0f, 1.0f);
	_meteorParticleHit2->GetComponent<Particle>()->AddParticleColor(2.0f, 0.5f, 0.0f);
	_meteorParticleHit2->GetComponent<Particle>()->SetParticleDirection(0.0f, 0.0f, 0.0f);
	_meteorParticleHit2->GetComponent<Particle>()->SetParticleCameraApply(true);
	_meteorParticleHit2->GetComponent<Particle>()->SetActive(false);
	_meteorParticleHit2->SetActive(true);

	_meteorParticleHit3 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("MeteorParticleHit3");
	_meteorParticleHit3->_autoAwake = true;
	_meteorParticleHit3->AddComponent<Particle>();
	_meteorParticleHit3->GetComponent<Particle>()->SetParticleEffect("Flare6", "Resources/Textures/Particles/fx_Twister3.dds", 1000);
	_meteorParticleHit3->GetComponent<Particle>()->SetParticleDuration(2.4f, 2.0f);
	_meteorParticleHit3->GetComponent<Particle>()->SetParticleVelocity(1.0f, true);
	_meteorParticleHit3->GetComponent<Particle>()->SetParticleSize(1.0f, 1.0f);
	_meteorParticleHit3->GetComponent<Particle>()->AddParticleColor(2.0f, 0.3f, 0.1f);
	_meteorParticleHit3->GetComponent<Particle>()->SetParticleDirection(0.0f, 600.0f, 0.0f);
	_meteorParticleHit3->GetComponent<Particle>()->SetParticleCameraApply(true);
	_meteorParticleHit3->GetComponent<Particle>()->SetActive(false);
	_meteorParticleHit3->SetActive(true);

	_meteorParticleHit4 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("MeteorParticleHit4");
	_meteorParticleHit4->_autoAwake = true;
	_meteorParticleHit4->AddComponent<Particle>();
	_meteorParticleHit4->GetComponent<Particle>()->SetParticleEffect("BlastWave2", "Resources/Textures/Particles/fx_BlastWave2.dds", 1000);
	_meteorParticleHit4->GetComponent<Particle>()->SetParticleDuration(4.0f, 0.5f);
	_meteorParticleHit4->GetComponent<Particle>()->SetParticleVelocity(0.0f, true);
	_meteorParticleHit4->GetComponent<Particle>()->SetParticleSize(1.0f, 1.0f);
	_meteorParticleHit4->GetComponent<Particle>()->AddParticleColor(1.0f, 0.0f, 0.0f);
	_meteorParticleHit4->GetComponent<Particle>()->SetParticleDirection(0.0f, 0.0f, 0.0f);
	_meteorParticleHit4->GetComponent<Particle>()->SetParticleCameraApply(true);
	_meteorParticleHit4->GetComponent<Particle>()->SetActive(false);
	_meteorParticleHit4->SetActive(true);

	// 운석에 point light 추가
	DirectX::XMFLOAT4 ambient = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 diffuse = { 1.0f, 0.0f, 0.0f, 1.0f };
	DirectX::XMFLOAT4 specular = { 1.0f, 1.0f, 1.0f, 1.0f };

	_meteor->AddComponent<Light>();
	_meteor->GetComponent<Light>()->CreatePointLight(ambient, diffuse, specular, 500.0f, 100.0f);

	// 비활성화 조건		// 인게임에서는 소멸처럼
	meteorProj->SetDestoryCondition([meteor, meteorProj, this]()->bool
		{
			if (_meteor->GetComponent<Transform>()->GetPosition().y <= 2.0f)
			{
				_soundComp->Stop(_meteorFall);
				_soundComp->Play(_meteorExplode);

				if (meteorProj->GetCollider()->GetTargetObject() != nullptr && meteorProj->GetCollider()->IsCollided() && meteorProj->GetCollider()->GetTargetObject()->GetTag() == "Boss" && this->_isMeteorHit)
				{
					EventManager::GetInstance().CalculateDamageToBoss(meteor);
					this->_isMeteorHit = false;
				}

				// 범위 표시 부분을 비활성화
				this->_meteorRange->GetComponent<MeteorRange>()->_onCast = false;
				this->_meteorRange->SetActive(false);
				this->_meteorRange->GetComponent<MeteorRange>()->SetActive(false);
				this->_beforeMeteor = false;

				// 그을림 객체 활성화
				this->_meteorCrator->SetActive(true);
				this->_meteorCrator->GetComponent<TransparentMesh>()->SetActive(true);
				this->_meteorCrator->GetComponent<TransparentMesh>()->Reset();
				Startcoroutine(cratorDuration);

				_isMeteorEnded = true;
				return true;
			}
			else
			{
				return false;
			}
		});

	// 비활성화 된 상태로 시작
	_meteor->SetActive(false);

	// 전투 로직
	meteor->SetLogic([meteor, meteorProj, this]()
		{
			if (_meteor->GetActivated())
			{
				_soundComp->Play(_meteorFall);

				_meteor->GetComponent<Transform>()->SetPosition
				(
					_meteor->GetComponent<Transform>()->GetPosition().x,
					_meteor->GetComponent<Transform>()->GetPosition().y - 30.0f * TimeManager::GetInstance().GetDeltaTime(),
					_meteor->GetComponent<Transform>()->GetPosition().z
				);

				// 실제로 끝나지 않았지만 로직상 문제 X
				if (meteorProj->GetCollider()->IsCollided() && meteorProj->GetCollider()->GetTargetObject()->GetTag() == "Boss")
				{
					_isMeteorEnded = true;
				}
			}

			if (!(_meteor->GetActivated()) && _isMeteorEnded == true)
			{
				_meteorParticleTimer += TimeManager::GetInstance().GetDeltaTime();

				_meteorParticleHit1->GetComponent<Particle>()->SetActive(true);
				_meteorParticleHit2->GetComponent<Particle>()->SetActive(true);
				_meteorParticleHit3->GetComponent<Particle>()->SetActive(true);
				_meteorParticleHit4->GetComponent<Particle>()->SetActive(true);

				_meteorParticleHit1->GetComponent<Transform>()->SetPosition(_meteor->GetComponent<Transform>()->GetPosition());
				_meteorParticleHit2->GetComponent<Transform>()->SetPosition(_meteor->GetComponent<Transform>()->GetPosition());
				_meteorParticleHit3->GetComponent<Transform>()->SetPosition(_meteor->GetComponent<Transform>()->GetPosition());
				_meteorParticleHit4->GetComponent<Transform>()->SetPosition(_meteor->GetComponent<Transform>()->GetPosition());

				_meteorParticleHit1->GetComponent<Particle>()->SetParticleSize(30 - (_meteorParticleTimer * 40), 30 - (_meteorParticleTimer * 40));

				_meteorParticleHit3->GetComponent<Particle>()->SetParticleSize(120 - (_meteorParticleTimer * 150), 120 - (_meteorParticleTimer * 150));


				if (_meteorParticleTimer < 0.2f)
				{
					_meteorParticleHit2->GetComponent<Particle>()->SetParticleSize(_meteorParticleTimer * 300, _meteorParticleTimer * 300);
					_meteorParticleHit4->GetComponent<Particle>()->SetParticleSize(_meteorParticleTimer * 300, _meteorParticleTimer * 300);
				}
				else
				{
					_meteorParticleHit2->GetComponent<Particle>()->SetParticleSize(120 - (_meteorParticleTimer * 120), 120 - (_meteorParticleTimer * 120));
					_meteorParticleHit4->GetComponent<Particle>()->SetParticleSize(120 - (_meteorParticleTimer * 120), 120 - (_meteorParticleTimer * 120));
				}


				if (_meteorParticleTimer > 1.0f)
				{
					_meteorParticleTimer = 0.0f;
					_meteorParticleHit1->GetComponent<Particle>()->SetActive(false);
					_meteorParticleHit2->GetComponent<Particle>()->SetActive(false);
					_meteorParticleHit3->GetComponent<Particle>()->SetActive(false);
					_meteorParticleHit4->GetComponent<Particle>()->SetActive(false);
					_isMeteorEnded = false;
				}

				if (_meteorParticleTimer < 0.3f)
				{
					EventManager::GetInstance().CamShake(3.0f);
				}
			}
		});

	// 다 만들었으면 컨테이너에 추가
	AddToContanier(meteor);
}

void KunrealEngine::PlayerAbility::InitializeHealth()
{
	_healthParticle1 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("healthParticle1");
	_healthParticle1->_autoAwake = true;
	_healthParticle1->AddComponent<Particle>();
	_healthParticle1->GetComponent<Particle>()->SetParticleEffect("fx_Blaster1", "Resources/Textures/Particles/fx_Blaster1.dds", 1000);
	_healthParticle1->GetComponent<Particle>()->SetParticleDuration(0.1f, 0.1f);
	_healthParticle1->GetComponent<Particle>()->SetParticleVelocity(1.0f, true);
	_healthParticle1->GetComponent<Particle>()->SetParticleSize(5.f, 5.0f);
	_healthParticle1->GetComponent<Particle>()->AddParticleColor(0.3f, 10.0f, 0.3f);
	_healthParticle1->GetComponent<Particle>()->SetParticleDirection(0.0f, 0.0f, 0.0f);
	_healthParticle1->GetComponent<Particle>()->SetOffSet(2, 16, -3);
	_healthParticle1->GetComponent<Particle>()->SetActive(false);
	_healthParticleList.emplace_back(_healthParticle1);

	_healthParticle2 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("healthParticle2");
	_healthParticle2->_autoAwake = true;
	_healthParticle2->AddComponent<Particle>();
	_healthParticle2->GetComponent<Particle>()->SetParticleEffect("fx_Blaster1", "Resources/Textures/Particles/fx_Blaster1.dds", 1000);
	_healthParticle2->GetComponent<Particle>()->SetParticleDuration(0.1f, 0.1f);
	_healthParticle2->GetComponent<Particle>()->SetParticleVelocity(1.0f, true);
	_healthParticle2->GetComponent<Particle>()->SetParticleSize(5.f, 5.0f);
	_healthParticle2->GetComponent<Particle>()->AddParticleColor(0.3f, 10.0f, 0.3f);
	_healthParticle2->GetComponent<Particle>()->SetParticleDirection(0.0f, 0.0f, 0.0f);
	_healthParticle2->GetComponent<Particle>()->SetOffSet(-2, 14, -3);
	_healthParticle2->GetComponent<Particle>()->SetActive(false);
	_healthParticleList.emplace_back(_healthParticle2);

	_healthParticle3 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("healthParticle3");
	_healthParticle3->_autoAwake = true;
	_healthParticle3->AddComponent<Particle>();
	_healthParticle3->GetComponent<Particle>()->SetParticleEffect("fx_Blaster1", "Resources/Textures/Particles/fx_Blaster1.dds", 1000);
	_healthParticle3->GetComponent<Particle>()->SetParticleDuration(0.1f, 0.1f);
	_healthParticle3->GetComponent<Particle>()->SetParticleVelocity(1.0f, true);
	_healthParticle3->GetComponent<Particle>()->SetParticleSize(5.f, 5.0f);
	_healthParticle3->GetComponent<Particle>()->AddParticleColor(0.3f, 10.0f, 0.3f);
	_healthParticle3->GetComponent<Particle>()->SetParticleDirection(0.0f, 0.0f, 0.0f);
	_healthParticle3->GetComponent<Particle>()->SetOffSet(2, 12, -3);
	_healthParticle3->GetComponent<Particle>()->SetActive(false);
	_healthParticleList.emplace_back(_healthParticle3);

	_healthParticle4 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("healthParticle4");
	_healthParticle4->_autoAwake = true;
	_healthParticle4->AddComponent<Particle>();
	_healthParticle4->GetComponent<Particle>()->SetParticleEffect("fx_Blaster1", "Resources/Textures/Particles/fx_Blaster1.dds", 1000);
	_healthParticle4->GetComponent<Particle>()->SetParticleDuration(0.1f, 0.1f);
	_healthParticle4->GetComponent<Particle>()->SetParticleVelocity(1.0f, true);
	_healthParticle4->GetComponent<Particle>()->SetParticleSize(5.f, 5.0f);
	_healthParticle4->GetComponent<Particle>()->AddParticleColor(0.3f, 10.0f, 0.3f);
	_healthParticle4->GetComponent<Particle>()->SetParticleDirection(0.0f, 0.0f, 0.0f);
	_healthParticle4->GetComponent<Particle>()->SetOffSet(1, 10, -3);
	_healthParticle4->GetComponent<Particle>()->SetActive(false);
	_healthParticleList.emplace_back(_healthParticle4);

	_healthParticle5 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("healthParticle5");
	_healthParticle5->_autoAwake = true;
	_healthParticle5->AddComponent<Particle>();
	_healthParticle5->GetComponent<Particle>()->SetParticleEffect("fx_Blaster1", "Resources/Textures/Particles/fx_Blaster1.dds", 1000);
	_healthParticle5->GetComponent<Particle>()->SetParticleDuration(0.1f, 0.1f);
	_healthParticle5->GetComponent<Particle>()->SetParticleVelocity(1.0f, true);
	_healthParticle5->GetComponent<Particle>()->SetParticleSize(5.f, 5.0f);
	_healthParticle5->GetComponent<Particle>()->AddParticleColor(0.3f, 10.0f, 0.3f);
	_healthParticle5->GetComponent<Particle>()->SetParticleDirection(0.0f, 0.0f, 0.0f);
	_healthParticle5->GetComponent<Particle>()->SetOffSet(-2, 8, -3);
	_healthParticle5->GetComponent<Particle>()->SetActive(false);
	_healthParticleList.emplace_back(_healthParticle5);

	_healthParticle6 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("healthParticle6");
	_healthParticle6->_autoAwake = true;
	_healthParticle6->AddComponent<Particle>();
	_healthParticle6->GetComponent<Particle>()->SetParticleEffect("fx_Blaster1", "Resources/Textures/Particles/fx_Blaster1.dds", 1000);
	_healthParticle6->GetComponent<Particle>()->SetParticleDuration(0.1f, 0.1f);
	_healthParticle6->GetComponent<Particle>()->SetParticleVelocity(1.0f, true);
	_healthParticle6->GetComponent<Particle>()->SetParticleSize(5.f, 5.0f);
	_healthParticle6->GetComponent<Particle>()->AddParticleColor(0.3f, 10.0f, 0.3f);
	_healthParticle6->GetComponent<Particle>()->SetParticleDirection(0.0f, 0.0f, 0.0f);
	_healthParticle6->GetComponent<Particle>()->SetOffSet(2, 6, -3);
	_healthParticle6->GetComponent<Particle>()->SetActive(false);
	_healthParticleList.emplace_back(_healthParticle6);
}

void KunrealEngine::PlayerAbility::RestoreHealth()
{
	// 회복량 계산 소수점 반올림
	float restoreAmount = round(this->_playerComp->_playerInfo._maxhp * this->_restorePercentage);

	// 플레이어가 회복 가능한 체력이 회복율보다 낮을 경우	// 체력이 70%보다 높을 경우
	if (this->_playerComp->_playerInfo._hp + restoreAmount > this->_playerComp->_playerInfo._maxhp)
	{
		this->_playerComp->_playerInfo._hp = this->_playerComp->_playerInfo._maxhp;
	}
	else
	{
		this->_playerComp->_playerInfo._hp += restoreAmount;
	}

	this->_maxPotion--;
	_isPotionUse = true;

	Startcoroutine(PotionCoolDown);

}


void KunrealEngine::PlayerAbility::PlayHealParticle()
{
	_potionTimer += TimeManager::GetInstance().GetDeltaTime();

	for (auto& healthParticleList : _healthParticleList)
	{
		healthParticleList->GetComponent<Transform>()->SetPosition(this->GetOwner()->GetComponent<Transform>()->GetPosition());
	}

	Startcoroutine(PotionEffectDraw);

	if (_potionTimer > 1.0f)
	{
		_potionTimer = 0.0f;
		_isPotionUse = false;
		for (auto& healthParticleList : _healthParticleList)
		{
			healthParticleList->GetComponent<Particle>()->SetActive(false);
		}
	}
}

void KunrealEngine::PlayerAbility::AnimateByFrame()
{
	if (_playerComp->_playerStatus == Player::Status::ABILITY && _playerComp->_abilityAnimationIndex == 1 && GetOwner()->GetComponent<Animator>()->GetCurrentFrame() >= GetOwner()->GetComponent<Animator>()->GetMaxFrame())
	{
		_playerComp->_playerStatus = Player::Status::IDLE;
	}

	if (_playerComp->_playerStatus == Player::Status::ABILITY && _playerComp->_abilityAnimationIndex == 2 && GetOwner()->GetComponent<Animator>()->GetCurrentFrame() >= GetOwner()->GetComponent<Animator>()->GetMaxFrame())
	{
		_playerComp->_playerStatus = Player::Status::IDLE;
	}

	if (_playerComp->_playerStatus == Player::Status::ABILITY && _playerComp->_abilityAnimationIndex == 3 && GetOwner()->GetComponent<Animator>()->GetCurrentFrame() >= GetOwner()->GetComponent<Animator>()->GetMaxFrame())
	{
		_playerComp->_playerStatus = Player::Status::IDLE;
		//this->_isLaserReady = false;
	}

	if (_playerComp->_playerStatus == Player::Status::ABILITY && _playerComp->_abilityAnimationIndex == 4 && GetOwner()->GetComponent<Animator>()->GetCurrentFrame() >= GetOwner()->GetComponent<Animator>()->GetMaxFrame())
	{
		_meteor->SetActive(true);
		_meteor->GetComponent<Particle>()->SetActive(true);
		_meteorParticle2->GetComponent<Particle>()->SetActive(true);
		_meteorParticle3->GetComponent<Particle>()->SetActive(true);
		_meteorParticle4->GetComponent<Particle>()->SetActive(true);
		_playerComp->_playerStatus = Player::Status::IDLE;
	}
}


void KunrealEngine::PlayerAbility::UpdateAbilityLogic()
{
	this->_abilityContainer[0]->_abilityLogic();
	this->_abilityContainer[1]->_abilityLogic();
	this->_abilityContainer[2]->_abilityLogic();
	this->_abilityContainer[3]->_abilityLogic();
}

void KunrealEngine::PlayerAbility::DebugText()
{
	//if (this->_isShotReady)
	//{
	//	GRAPHICS->DrawDebugText(100, 500, 40, "Q Ready");
	//}
	//else
	//{
	//	GRAPHICS->DrawDebugText(100, 500, 40, "Q On CoolDown");
	//}
	//
	//if (this->_isIceReady)
	//{
	//	GRAPHICS->DrawDebugText(100, 600, 40, "W Ready");
	//}
	//else
	//{
	//	GRAPHICS->DrawDebugText(100, 600, 40, "W On CoolDown");
	//}
	//
	//if (this->_isLaserReady)
	//{
	//	GRAPHICS->DrawDebugText(100, 700, 40, "E Ready");
	//}
	//else
	//{
	//	GRAPHICS->DrawDebugText(100, 700, 40, "E On CoolDown");
	//}
	//
	//if (this->_isMeteorReady)
	//{
	//	GRAPHICS->DrawDebugText(100, 800, 40, "R Ready");
	//}
	//else
	//{
	//	GRAPHICS->DrawDebugText(100, 800, 40, "R On CoolDown");
	//}

	//if (this->_laser->GetCollider()->GetTargetObject() != nullptr)
	//{
	//	GRAPHICS->DrawDebugText(100, 800, 40, this->_laser->GetCollider()->GetTargetObject()->GetObjectName//().c_str());
	//}
	//else
	//{
	//	GRAPHICS->DrawDebugText(100, 800, 40, "Laser Not Collided");
	//}

	if (this->_isPotionReady)
	{
		GRAPHICS->DrawDebugText(100, 800, 40, "Potion Ready");
	}
	else
	{
		GRAPHICS->DrawDebugText(100, 800, 40, "Potion on cooldown");
	}

	//GRAPHICS->DrawDebugText(100, 850, 40, "%f", g_);
}

void KunrealEngine::PlayerAbility::AddToContanier(Ability* abil)
{
	_abilityContainer.emplace_back(abil);
}

