#include "Player.h"
#include "Transform.h"
#include "InputSystem.h"
#include "PlayerAbility.h"
#include "PlayerMove.h"
#include "MeshRenderer.h"
#include "BoxCollider.h"
#include "Transform.h"
#include "GraphicsSystem.h"
#include "Navigation.h"
#include "ToolBox.h"

#include "CylinderCollider.h"
#include "MeshCollider.h"

KunrealEngine::Player::Player()
	:_transform(nullptr), _playerStatus(Status::IDLE), _tempStatus(Status::IDLE), _owner(nullptr)
	, _playerInfo(
		300.0f,			// hp
		300.0f,			// maxhp
		100.0f,			// stamina
		17.0f,			// movespeed
		120.0f,			// dashspeed
		40.0f,			// dashrange
		8.0f,			// dashcooldown
		1.0f,			// spellpower
		1.0f,			// damageReduce
		1.8f			// speedScale
	), _directionVector(), _abilityAnimationIndex(0),
	_isSweep(false), _sweepRange(20.0f), _movedRange(0.0f), _sweepDuration(0.5f), _sweepNode(), _sweepAnimationSpeed(35.0f), _gravity(-5.81f), _nodeCount(0)
	, _deathParticle1(nullptr), _deathParticle2(nullptr), _deathParticle3(nullptr), _deathParticle4(nullptr), _deathParticle5(nullptr), _deathParticle6(nullptr), _deathParticleVector{}, _deathAnimationSpeed(30.0f)
	, _onCasting(false), _playerStartX(0.7f), _playerStartZ(-55.0f), _playerBindFlag(false), _cinematicIndex(0), _iswakeuped(false)
{
	_sweepNode.clear();
}

KunrealEngine::Player::~Player()
{

}

void KunrealEngine::Player::Initialize()
{
	// Player ������Ʈ �ϳ������� ���õ� ������Ʈ ��� �߰��ǵ���
	this->_owner = GetOwner();

	this->_transform = _owner->GetComponent<Transform>();
	this->_transform->SetScale(0.1f, 0.1f, 0.1f);
	this->_transform->SetRotation(0.0f, 45.f, 0.0f);

	this->_owner->SetTag("Player");
	this->_owner->AddComponent<MeshRenderer>();
	this->_owner->GetComponent<MeshRenderer>()->SetMeshObject("PlayerWithCloak/PlayerWithCloak");
	this->_owner->GetComponent<MeshRenderer>()->SetActive(true);
	this->_owner->GetComponent<MeshRenderer>()->SetPickableState(true);
	this->_owner->GetComponent<MeshRenderer>()->SetNormalTexture(0, "PlayerWithCloak/M_Clothes_normal.png");
	this->_owner->GetComponent<MeshRenderer>()->SetNormalTexture(1, "PlayerWithCloak/M_Body_normal.png");
	this->_owner->GetComponent<MeshRenderer>()->SetNormalTexture(2, "PlayerWithCloak/M_Hair_normal.png");
	this->_owner->GetComponent<MeshRenderer>()->SetCartoonState(true);

	this->_owner->AddComponent<BoxCollider>();
	this->_owner->GetComponent<BoxCollider>()->SetColliderScale(7.0f, 15.0f, 7.0f);
	this->_owner->GetComponent<BoxCollider>()->SetOffset(0.0f, 8.0f, 0.0f);

	//this->_owner->AddComponent<CylinderCollider>();
	//this->_owner->GetComponent<CylinderCollider>()->SetColliderScale(30.0f, 30.0f, 30.0f);
	//this->_owner->GetComponent<CylinderCollider>()->SetOffset(0.0f, 8.0f, 0.0f);

	//this->_owner->AddComponent<MeshCollider>();
	//this->_owner->GetComponent<MeshCollider>()->CreateMeshCollider("Blade");
	//this->_owner->GetComponent<MeshCollider>()->SetColliderScale(1.0f, 1.0f, 1.0f);
	//this->_owner->GetComponent<MeshCollider>()->SetOffset(0.0f, 10.0f, 0.0f);

	auto soundcomp = this->_owner->AddComponent<SoundPlayer>();
	_knock_downSound = soundcomp->CreateSoundInfo("Resources/sound/body-fall.mp3", true, false, 100);
	_diedsound = soundcomp->CreateSoundInfo("Resources/sound/youDied.mp3", true, false, 100);
	_staandup = soundcomp->CreateSoundInfo("Resources/sound/standup_sound.mp3", true, false, 100);
	_hit = soundcomp->CreateSoundInfo("Resources/sound/rosary_hit_02.ogg", true, false, 100);
	soundcomp->CreateSound(_knock_downSound, 1);
	soundcomp->CreateSound(_diedsound, 1);
	soundcomp->CreateSound(_staandup, 1);
	soundcomp->CreateSound(_hit, 1);


	this->_owner->AddComponent<PlayerAbility>();
	this->_owner->AddComponent<PlayerMove>();

	// ��ƼŬ lifetime�� ������ 0���� ������ָ� ������ �����

	this->_deathParticle1 = this->GetOwner()->GetObjectScene()->CreateObject("playerDeathParticle1");
	this->_deathParticle1->AddComponent<Particle>();
	this->_deathParticle1->GetComponent<Particle>()->SetParticleEffect("Lightning1", "Resources/Textures/Particles/fx_Lightning1.dds", 1000);
	this->_deathParticle1->GetComponent<Particle>()->SetParticleDuration(10.0f, 5.0f);
	this->_deathParticle1->GetComponent<Particle>()->SetParticleVelocity(2.0f, true);
	this->_deathParticle1->GetComponent<Particle>()->SetParticleSize(1.0f, 1.0f);
	this->_deathParticle1->GetComponent<Particle>()->AddParticleColor(0.1f, 0.1f, 0.1f);
	this->_deathParticle1->GetComponent<Particle>()->SetParticleDirection(0.3f, 0.6f, 0.0f);
	this->_deathParticle1->GetComponent<Particle>()->SetActive(false);
	_deathParticleVector.emplace_back(_deathParticle1);

	this->_deathParticle2 = this->GetOwner()->GetObjectScene()->CreateObject("playerDeathParticle2");
	this->_deathParticle2->AddComponent<Particle>();
	this->_deathParticle2->GetComponent<Particle>()->SetParticleEffect("fx_Dust3", "Resources/Textures/Particles/fx_Dust3.dds", 1000);
	this->_deathParticle2->GetComponent<Particle>()->SetParticleDuration(10.0f, 5.0f);
	this->_deathParticle2->GetComponent<Particle>()->SetParticleVelocity(2.0f, true);
	this->_deathParticle2->GetComponent<Particle>()->SetParticleSize(3.0f, 3.0f);
	this->_deathParticle2->GetComponent<Particle>()->AddParticleColor(0.2f, 0.2f, 0.2f);
	this->_deathParticle2->GetComponent<Particle>()->SetParticleDirection(0.3f, 0.6f, 0.0f);
	this->_deathParticle2->GetComponent<Particle>()->SetActive(false);
	_deathParticleVector.emplace_back(_deathParticle2);

	this->_deathParticle3 = this->GetOwner()->GetObjectScene()->CreateObject("playerDeathParticle1");
	this->_deathParticle3->AddComponent<Particle>();
	this->_deathParticle3->GetComponent<Particle>()->SetParticleEffect("Lightning1", "Resources/Textures/Particles/fx_Lightning1.dds", 1000);
	this->_deathParticle3->GetComponent<Particle>()->SetParticleDuration(10.0f, 5.0f);
	this->_deathParticle3->GetComponent<Particle>()->SetParticleVelocity(2.0f, true);
	this->_deathParticle3->GetComponent<Particle>()->SetParticleSize(0.3f, 0.3f);
	this->_deathParticle3->GetComponent<Particle>()->AddParticleColor(0.1f, 0.1f, 0.1f);
	this->_deathParticle3->GetComponent<Particle>()->SetParticleDirection(0.3f, 0.6f, 0.0f);
	this->_deathParticle3->GetComponent<Particle>()->SetActive(false);
	_deathParticleVector.emplace_back(_deathParticle3);

	this->_deathParticle4 = this->GetOwner()->GetObjectScene()->CreateObject("playerDeathParticle2");
	this->_deathParticle4->AddComponent<Particle>();
	this->_deathParticle4->GetComponent<Particle>()->SetParticleEffect("fx_Dust3", "Resources/Textures/Particles/fx_Dust3.dds", 1000);
	this->_deathParticle4->GetComponent<Particle>()->SetParticleDuration(10.0f, 5.0f);
	this->_deathParticle4->GetComponent<Particle>()->SetParticleVelocity(2.0f, true);
	this->_deathParticle4->GetComponent<Particle>()->SetParticleSize(1.0f, 1.0f);
	this->_deathParticle4->GetComponent<Particle>()->AddParticleColor(0.2f, 0.2f, 0.2f);
	this->_deathParticle4->GetComponent<Particle>()->SetParticleDirection(0.3f, 0.6f, 0.0f);
	this->_deathParticle4->GetComponent<Particle>()->SetActive(false);
	_deathParticleVector.emplace_back(_deathParticle4);

	this->_deathParticle5 = this->GetOwner()->GetObjectScene()->CreateObject("playerDeathParticle1");
	this->_deathParticle5->AddComponent<Particle>();
	this->_deathParticle5->GetComponent<Particle>()->SetParticleEffect("Lightning1", "Resources/Textures/Particles/fx_Lightning1.dds", 1000);
	this->_deathParticle5->GetComponent<Particle>()->SetParticleDuration(10.0f, 5.0f);
	this->_deathParticle5->GetComponent<Particle>()->SetParticleVelocity(2.0f, true);
	this->_deathParticle5->GetComponent<Particle>()->SetParticleSize(0.3f, 0.3f);
	this->_deathParticle5->GetComponent<Particle>()->AddParticleColor(0.1f, 0.1f, 0.1f);
	this->_deathParticle5->GetComponent<Particle>()->SetParticleDirection(0.3f, 0.6f, 0.0f);
	this->_deathParticle5->GetComponent<Particle>()->SetActive(false);
	_deathParticleVector.emplace_back(_deathParticle5);

	this->_deathParticle6 = this->GetOwner()->GetObjectScene()->CreateObject("playerDeathParticle2");
	this->_deathParticle6->AddComponent<Particle>();
	this->_deathParticle6->GetComponent<Particle>()->SetParticleEffect("fx_Dust3", "Resources/Textures/Particles/fx_Dust3.dds", 1000);
	this->_deathParticle6->GetComponent<Particle>()->SetParticleDuration(10.0f, 5.0f);
	this->_deathParticle6->GetComponent<Particle>()->SetParticleVelocity(2.0f, true);
	this->_deathParticle6->GetComponent<Particle>()->SetParticleSize(1.0f, 1.0f);
	this->_deathParticle6->GetComponent<Particle>()->AddParticleColor(0.2f, 0.2f, 0.2f);
	this->_deathParticle6->GetComponent<Particle>()->SetParticleDirection(0.3f, 0.6f, 0.0f);
	this->_deathParticle6->GetComponent<Particle>()->SetActive(false);
	_deathParticleVector.emplace_back(_deathParticle6);
}

void KunrealEngine::Player::Release()
{

}

void KunrealEngine::Player::FixedUpdate()
{

}

void KunrealEngine::Player::Update()
{
	BeforeStart();

	AnimateByStatus();
	AfterHit();
	CheckDeath();
	PlayerSweep();

	DebugFunc();

}

void KunrealEngine::Player::LateUpdate()
{

}

void KunrealEngine::Player::OnTriggerEnter()
{

}

void KunrealEngine::Player::OnTriggerStay()
{

}

void KunrealEngine::Player::OnTriggerExit()
{

}

void KunrealEngine::Player::SetActive(bool active)
{
	this->_isActivated = active;
}

void KunrealEngine::Player::AnimateByStatus()
{
	// ���°� ���� �� �ִϸ��̼� �������� 0����
	if (_tempStatus != _playerStatus)
	{
		this->_owner->GetComponent<Animator>()->Stop();
		_tempStatus = _playerStatus;
	}

	// ���¿� ���� �ִϸ��̼� ���
	if (this->_owner->GetComponent<MeshRenderer>() != nullptr)
	{
		switch (_playerStatus)
		{
			case KunrealEngine::Player::Status::IDLE:
				this->_owner->GetComponent<Animator>()->Play("StandingIdle", 30.0f * _playerInfo._speedScale, true);
				break;
			case KunrealEngine::Player::Status::WALK:
				this->_owner->GetComponent<Animator>()->Play("FastRun", 15.0f * _playerInfo._speedScale, true);
				break;
			case KunrealEngine::Player::Status::DASH:
				this->_onCasting = false;
				this->_owner->GetComponent<Animator>()->Play("Dash", 30.0f * _playerInfo._speedScale, true);
				break;
			case KunrealEngine::Player::Status::ABILITY:
				this->_onCasting = true;

				if (this->_abilityAnimationIndex == 1)
				{
					this->_owner->GetComponent<Animator>()->Play("Shot", 40.0f * _playerInfo._speedScale, false);
				}
				else if (this->_abilityAnimationIndex == 2)
				{
					this->_owner->GetComponent<Animator>()->Play("Ice", 40.0f * _playerInfo._speedScale, false);
				}
				else if (this->_abilityAnimationIndex == 3)
				{
					this->_owner->GetComponent<Animator>()->Play("Beam_full", 40.0f * (_playerInfo._speedScale * 0.66f), false);
				}
				else if (this->_abilityAnimationIndex == 4)
				{
					this->_owner->GetComponent<Animator>()->Play("Meteor", 40.0f * _playerInfo._speedScale, false);
				}

				if (this->_owner->GetComponent<Animator>()->GetCurrentFrame() > this->_owner->GetComponent<Animator>()->GetMaxFrame())
				{
					this->_onCasting = false;
				}

				break;
			case KunrealEngine::Player::Status::STAGGERED:
				this->_onCasting = false;
				this->_owner->GetComponent<Animator>()->Play("Staggered", 20.0f * _playerInfo._speedScale, true);
				break;
			case KunrealEngine::Player::Status::PARALYSIS:
				this->_onCasting = false;
				this->_owner->GetComponent<SoundPlayer>()->Play(_hit);
				this->_owner->GetComponent<Animator>()->Play("SmallHit", 60.0f * _playerInfo._speedScale, false);
				break;
			case KunrealEngine::Player::Status::SWEEP:
				this->_onCasting = false;
				this->_owner->GetComponent<Animator>()->Play("Sweep", _sweepAnimationSpeed * _playerInfo._speedScale, false);

				if (this->_owner->GetComponent<Animator>()->GetCurrentFrame() <= 10.0f)
				{
					this->_sweepAnimationSpeed = 70.0f;
				}
				else
				{
					this->_sweepAnimationSpeed = 30.0f;
				}

				//if (this->_owner->GetComponent<Animator>()->GetCurrentFrame() >= 30.f && this->_transform->GetPosition().y > 4.0f)
				//{
				//	this->_owner->GetComponent<Animator>()->SetCurrentFrame(36);
				//}

				if (this->_owner->GetComponent<Animator>()->GetCurrentFrame() == this->_owner->GetComponent<Animator>()->GetMaxFrame())
				{
					this->_owner->GetComponent<Animator>()->Pause();
				}
				break;
			case Status::CINEMATIC:
				this->_onCasting = false;

				// �Ű������� �־��� index�� ���� �ִϸ��̼� �ݺ� ���	// ���ư��°� ����
				if (this->_cinematicIndex == 0)
				{
					// IDLE
					this->_owner->GetComponent<Animator>()->Play("StandingIdle", 30.0f * _playerInfo._speedScale, true);

				}
				else if (this->_cinematicIndex == 1)
				{
					// WALK
					this->_owner->GetComponent<Animator>()->Play("FastRun", 15.0f * _playerInfo._speedScale, true);
				}
				else if (this->_cinematicIndex == 2)
				{
					// STAGGERED
					this->_owner->GetComponent<Animator>()->Play("Staggered", 20.0f * _playerInfo._speedScale, true);
				}
				else if (this->_cinematicIndex == 3)
				{
					// LASER
					this->_owner->GetComponent<Animator>()->Play("Beam_full", 40.0f * (_playerInfo._speedScale * 0.66f), true);

					if (this->_owner->GetComponent<Animator>()->GetCurrentFrame() > 57)
					{
						this->_owner->GetComponent<Animator>()->SetCurrentFrame(49);
					}
				}
				else if (this->_cinematicIndex == 4)
				{
					// SWEEP
					this->_owner->GetComponent<Animator>()->Play("Sweep", _sweepAnimationSpeed * _playerInfo._speedScale, false);
				}		
				else if (this->_cinematicIndex == 5)
				{
					// METEOR	// ������ �����Ӹ�
					this->_owner->GetComponent<Animator>()->Play("Meteor", 40.0f * _playerInfo._speedScale, false);
					this->_owner->GetComponent<Animator>()->SetCurrentFrame(this->_owner->GetComponent<Animator>()->GetMaxFrame());
				}

				break;
			case KunrealEngine::Player::Status::DEAD:
				this->_onCasting = false;
				this->_owner->GetComponent<Animator>()->Play("Death", _deathAnimationSpeed * _playerInfo._speedScale, false);

				if (this->_owner->GetComponent<Animator>()->GetCurrentFrame() < 9)
				{
					_deathAnimationSpeed = 5.0f;
				}
				else if (this->_owner->GetComponent<Animator>()->GetCurrentFrame() >= 10 && _owner->GetComponent<Animator>()->GetCurrentFrame() < 39)
				{
					_deathAnimationSpeed = 30.0f;

					// �ٽ� �����ϴ� ��ư�� �����ϸ� �ٽ� �������

					for (auto& deathParticle : _deathParticleVector)
					{
						deathParticle->SetActive(true);
						deathParticle->GetComponent<Particle>()->SetActive(true);
					}

					for (int i = 0; i < _deathParticleVector.size(); i++)
					{
						if (i == 0 || i == 1)
						{
							_deathParticleVector[i]->GetComponent<Transform>()->SetPosition(this->_owner->GetComponent<MeshRenderer>()->GetBoneTransform("spine_02")._41,
								this->_owner->GetComponent<MeshRenderer>()->GetBoneTransform("spine_02")._42,
								this->_owner->GetComponent<MeshRenderer>()->GetBoneTransform("spine_02")._43);
						}
						else if(i == 2 || i == 3)
						{
							_deathParticleVector[i]->GetComponent<Transform>()->SetPosition(this->_owner->GetComponent<MeshRenderer>()->GetBoneTransform("calf_r")._41,
								this->_owner->GetComponent<MeshRenderer>()->GetBoneTransform("calf_r")._42,
								this->_owner->GetComponent<MeshRenderer>()->GetBoneTransform("calf_r")._43);
						}
						else
						{
							_deathParticleVector[i]->GetComponent<Transform>()->SetPosition(this->_owner->GetComponent<MeshRenderer>()->GetBoneTransform("calf_l")._41,
								this->_owner->GetComponent<MeshRenderer>()->GetBoneTransform("calf_l")._42,
								this->_owner->GetComponent<MeshRenderer>()->GetBoneTransform("calf_l")._43);
						}

					}
				}
				else if (this->_owner->GetComponent<Animator>()->GetCurrentFrame() >= 39)
				{
					_deathAnimationSpeed = 15.0f;

					for (int i = 0; i < _deathParticleVector.size(); i++)
					{
						if (i == 0 || i == 1)
						{
							_deathParticleVector[i]->GetComponent<Transform>()->SetPosition(this->_owner->GetComponent<MeshRenderer>()->GetBoneTransform("spine_02")._41,
								this->_owner->GetComponent<MeshRenderer>()->GetBoneTransform("spine_02")._42,
								this->_owner->GetComponent<MeshRenderer>()->GetBoneTransform("spine_02")._43);
						}
						else if (i == 2 || i == 3)
						{
							_deathParticleVector[i]->GetComponent<Transform>()->SetPosition(this->_owner->GetComponent<MeshRenderer>()->GetBoneTransform("calf_r")._41,
								this->_owner->GetComponent<MeshRenderer>()->GetBoneTransform("calf_r")._42,
								this->_owner->GetComponent<MeshRenderer>()->GetBoneTransform("calf_r")._43);
						}
						else
						{
							_deathParticleVector[i]->GetComponent<Transform>()->SetPosition(this->_owner->GetComponent<MeshRenderer>()->GetBoneTransform("calf_l")._41,
								this->_owner->GetComponent<MeshRenderer>()->GetBoneTransform("calf_l")._42,
								this->_owner->GetComponent<MeshRenderer>()->GetBoneTransform("calf_l")._43);
						}

					}
				}

				break;
			default:
				break;
		}
	}
}

void KunrealEngine::Player::SetHitState(int patternType)
{
	if (patternType == 1)
	{
		_playerStatus = Status::PARALYSIS;
	}
	else if (patternType == 2)
	{
		_playerStatus = Status::SWEEP;
	}
}


void KunrealEngine::Player::CalculateSweep(DirectX::XMVECTOR direction)
{
	// �̹� �����ְų� ���ư��� ������ �� �ȳ��ư���
	if (this->_playerStatus == Status::SWEEP)
	{
		return;
	}

	// ��带 ����ش�
	this->_sweepNode.clear();
	this->_nodeCount = 0;

	this->_directionVector = DirectX::XMVectorNegate(direction);		// �ٶ󺸴� ���� ����

	// �÷��̾� ���� ȸ��
	DirectX::XMVECTOR currentForward = DirectX::XMVectorSet(0.0f, _transform->GetRotation().y, -1.0f, 0.0f);
	DirectX::XMVECTOR dotResult = DirectX::XMVector3Dot(currentForward, this->_directionVector);
	float dotX = DirectX::XMVectorGetX(dotResult);
	float angle = acos(dotX);
	angle = DirectX::XMConvertToDegrees(angle);

	// �÷��̾� �̵�
	DirectX::XMFLOAT3 currentPoint = _transform->GetPosition();
	DirectX::XMVECTOR currentVector = DirectX::XMLoadFloat3(&currentPoint);

	// �̵� ��ǥ ����
	DirectX::XMVECTOR targetVector = DirectX::XMVectorAdd(currentVector, DirectX::XMVectorScale(direction, this->_sweepRange));
	DirectX::XMFLOAT3 targetPoint;
	DirectX::XMStoreFloat3(&targetPoint, targetVector);

	// ������ �����Ǵ� ��� ó��
	if (targetVector.m128_f32[0] < currentVector.m128_f32[0])
	{
		angle *= -1;
	}
	this->_transform->SetRotation(0.0f, angle, 0.0f);

	// �������� �̵��ϴ� Navigation�� ���� �̵��� �� �ִ� ��ġ ���
	Navigation::GetInstance().SetSEpos(0, _transform->GetPosition().x, _transform->GetPosition().y, _transform->GetPosition().z,
		targetPoint.x, this->_owner->GetComponent<PlayerMove>()->GetPlayerY(), targetPoint.z);

	targetPoint = Navigation::GetInstance().FindRaycastPath(0);

	// ������ ��ǥ ���
	ToolBox::CalculateParabolaPath(_transform->GetPosition(), targetPoint, _sweepDuration, _gravity, _sweepNode);

	// ���� �ٴ� ���̿� ���� ������ ��� y�� �缳��
	_sweepNode.back().y = this->_owner->GetComponent<PlayerMove>()->GetPlayerY();

	// ���ư� �غ� �Ϸ�
	this->_isSweep = true;
}


void KunrealEngine::Player::CalculateSweep(DirectX::XMFLOAT3 direction)
{
	// �̹� �����ְų� ���ư��� ������ �� �ȳ��ư���
	if (this->_playerStatus == Status::SWEEP)
	{
		return;
	}

	// ��带 ����ش�
	this->_sweepNode.clear();
	this->_nodeCount = 0;
	DirectX::XMVECTOR directionVec = DirectX::XMLoadFloat3(&direction);

	this->_directionVector = DirectX::XMVectorNegate(directionVec);		// �ٶ󺸴� ���� ����

	// �÷��̾� ���� ȸ��
	DirectX::XMVECTOR currentForward = DirectX::XMVectorSet(0.0f, _transform->GetRotation().y, -1.0f, 0.0f);
	DirectX::XMVECTOR dotResult = DirectX::XMVector3Dot(currentForward, this->_directionVector);
	float dotX = DirectX::XMVectorGetX(dotResult);
	float angle = acos(dotX);
	angle = DirectX::XMConvertToDegrees(angle);

	// �÷��̾� �̵�
	DirectX::XMFLOAT3 currentPoint = _transform->GetPosition();
	DirectX::XMVECTOR currentVector = DirectX::XMLoadFloat3(&currentPoint);

	// �̵� ��ǥ ����
	DirectX::XMVECTOR targetVector = DirectX::XMVectorAdd(currentVector, DirectX::XMVectorScale(directionVec, this->_sweepRange));
	DirectX::XMFLOAT3 targetPoint;
	DirectX::XMStoreFloat3(&targetPoint, targetVector);

	// ������ �����Ǵ� ��� ó��
	if (targetVector.m128_f32[0] < currentVector.m128_f32[0])
	{
		angle *= -1;
	}
	this->_transform->SetRotation(0.0f, angle, 0.0f);

	// �������� �̵��ϴ� Navigation�� ���� �̵��� �� �ִ� ��ġ ���
	Navigation::GetInstance().SetSEpos(0, _transform->GetPosition().x, _transform->GetPosition().y, _transform->GetPosition().z,
		targetPoint.x, this->_owner->GetComponent<PlayerMove>()->GetPlayerY(), targetPoint.z);

	targetPoint = Navigation::GetInstance().FindRaycastPath(0);

	// ������ ��ǥ ���
	ToolBox::CalculateParabolaPath(_transform->GetPosition(), targetPoint, _sweepDuration, _gravity, _sweepNode);

	// ���� �ٴ� ���̿� ���� ������ ��� y�� �缳��
	_sweepNode.back().y = this->_owner->GetComponent<PlayerMove>()->GetPlayerY();

	// ���ư� �غ� �Ϸ�
	this->_isSweep = true;
}

void KunrealEngine::Player::PlayerSweep()
{
	/// ������ ������
	/// ��� ����ŭ �����߰ڳ�
	if (this->_isSweep && this->_nodeCount < this->_sweepNode.size())
	{
		// �÷��̾ ���ư��� ���·�
		this->_playerStatus = Status::SWEEP;
		_owner->GetComponent<SoundPlayer>()->Play(_knock_downSound);

		// �� �ָ� ���ư��°� ���� ������ġ
		//if (_movedRange >= _sweepRange)
		//{
		//	this->_isSweep = false;
		//	this->_playerStatus = Status::IDLE;
		//}

		//float absx = std::abs(_transform->GetPosition().x - _sweepNode[_nodeCount].x);
		//float absy = std::abs(_transform->GetPosition().y - _sweepNode[_nodeCount].y);
		//float absz = std::abs(_transform->GetPosition().z - _sweepNode[_nodeCount].z);

		if
			(
				std::abs(_transform->GetPosition().x - _sweepNode[_nodeCount].x) > 0.3f ||
				//std::abs(_transform->GetPosition().y - _sweepNode[_nodeCount].y) > 0.3f ||
				std::abs(_transform->GetPosition().z - _sweepNode[_nodeCount].z) > 0.3f
				)
		{
			this->_transform->SetPosition(_sweepNode[_nodeCount].x, _sweepNode[_nodeCount].y, _sweepNode[_nodeCount].z);
		}
		else
		{
			this->_nodeCount++;
		}

	}
	// ��ǥ�������� �����ϸ� 
	else if (this->_isSweep && this->_nodeCount == this->_sweepNode.size())
	{
		// 		this->_playerStatus = Status::IDLE;
		// 		this->_isSweep = false;
		Startcoroutine(afterSweep);
	}
}

void KunrealEngine::Player::MoveToScene(std::string sceneName)
{
	this->_owner->MoveToScene(sceneName);
	this->_owner->GetComponent<PlayerAbility>()->_shot->SetActive(false);
	this->_owner->GetComponent<PlayerAbility>()->_shot->MoveToScene(sceneName);
	this->_owner->GetComponent<PlayerAbility>()->_circle->SetActive(false);
	this->_owner->GetComponent<PlayerAbility>()->_circle->MoveToScene(sceneName);
	this->_owner->GetComponent<PlayerAbility>()->_meteor->SetActive(false);
	this->_owner->GetComponent<PlayerAbility>()->_meteor->MoveToScene(sceneName);
}


void KunrealEngine::Player::ResetPlayerStatus()
{
	// ���� �ʱ�ȭ
	this->_playerInfo._hp = this->_playerInfo._maxhp;
	this->_playerInfo._spellPower = 1.0f;

	// ���� �ʱ�ȭ
	this->_transform->SetRotation(0.0f, -180.0f, 0.0f);

	// ���� ����
	this->GetOwner()->GetComponent<PlayerAbility>()->_maxPotion = 5;

	// ��ġ �ʱ�ȭ
	this->_transform->SetPosition(this->_playerStartX, 2.0f, this->_playerStartZ);

	// ���� �ʱ�ȭ
	this->_playerStatus = Status::IDLE;
}


void KunrealEngine::Player::SetPlayerBindFlag(bool flag, int state /*= 0*/)
{
	/// flag�� true�϶� �ൿ�Ұ� false�϶� IDLE�� �ٲ���
	/// �ݺ����� ������ �Ű����� ���� �ݺ�ȣ���ϸ� ��� IDLE �ȴ�
	/// 
	/// int �Ű������� �÷��̾ �ݺ������� �ִϸ��̼�		DEFAULT�� 0
	// 0 IDLE
	// 1 WALK
	// 2 STAGGERED	// ���� �ִϸ��̼�
	// 3 LASER		// �� �� ��� �ִ�ü�� �ݺ�
	// 4 SWEEP		// �̰� �ݺ�����ȵ�

	this->_playerBindFlag = flag;

	//if (flag)
	//{
	//	this->_playerStatus = Status::CINEMATIC;
	//	this->_cinematicIndex = state;
	//}
	//else
	//{
	//	this->_playerStatus = Status::IDLE;
	//}
}


bool KunrealEngine::Player::GetPlayerBindFlag()
{
	return this->_playerBindFlag;
}

void KunrealEngine::Player::AfterHit()
{
	if (_playerStatus == Status::PARALYSIS)
	{
		if (this->_owner->GetComponent<Animator>()->GetCurrentFrame() >= this->_owner->GetComponent<Animator>()->GetMaxFrame())
		{
			_playerStatus = Status::IDLE;
		}

	}
	else if (_playerStatus == Status::SWEEP)
	{
		if (GetOwner()->GetComponent<Animator>()->GetCurrentFrame() >= GetOwner()->GetComponent<Animator>()->GetMaxFrame())
		{
			Startcoroutine(afterSweep);
		}
	}
}


void KunrealEngine::Player::BeforeStart()
{
	if (SceneManager::GetInstance().GetCurrentScene()->GetSceneName() == "Title")
	{
		if (SceneManager::GetInstance().GetCurrentScene()->GetGameObject("titleuibox")->GetActivated())
		{
			this->_playerStatus = Status::BEFORESTART;
			this->_owner->GetComponent<Animator>()->Play("StandingUp", 30.0f * _playerInfo._speedScale, false);
			this->_owner->GetComponent<Animator>()->SetCurrentFrame(0);
		}
		else
		{
			if (this->_playerStatus == Status::BEFORESTART)
			{
				this->_owner->GetComponent<SoundPlayer>()->Play(_staandup);
				this->_owner->GetComponent<Animator>()->Play("StandingUp", 30.0f * _playerInfo._speedScale, false);
			}
		}

		if (this->_owner->GetComponent<Animator>()->GetCurrentFrame() >= this->_owner->GetComponent<Animator>()->GetMaxFrame())
		{
			if (!_iswakeuped)
			{
				_iswakeuped = true;
				auto& scene = SceneManager::GetInstance();
				this->_owner->GetComponent<SoundPlayer>()->Stop(_staandup);

				scene.GetScene("Title")->GetGameObject("tutorialFade")->GetComponent<ImageRenderer>()->Update();
				scene.GetScene("Title")->GetGameObject("tutorialFade")->SetActive(true);

				scene.GetScene("Title")->GetGameObject("tutorialImage")->GetComponent<ImageRenderer>()->Update();
				scene.GetScene("Title")->GetGameObject("tutorialImage")->SetActive(true);
			}
		}
	}
}

void KunrealEngine::Player::SetPlayerIdle()
{
	this->_playerStatus = Status::IDLE;
}

const DirectX::XMVECTOR KunrealEngine::Player::GetDirectionVector()
{
	return this->_directionVector;
}

const KunrealEngine::Player::Status KunrealEngine::Player::GetPlayerStatus()
{
	return this->_playerStatus;
}

KunrealEngine::Player::PlayerInfo& KunrealEngine::Player::GetPlayerData()
{
	return this->_playerInfo;
}

void KunrealEngine::Player::CheckDeath()
{
	if (this->_playerInfo._hp <= 0.0f)
	{
		this->_playerStatus = Status::DEAD;
	}
}

void KunrealEngine::Player::DebugFunc()
{
	// O ������ ������ü�� ��Ȱ��ȭ ���������� �ƹ��� ��
	if (InputSystem::GetInstance()->KeyDown(KEY::O))
	{
		this->_owner->GetComponent<PlayerAbility>()->_shot->SetActive(false);
		this->_owner->GetComponent<PlayerAbility>()->_circle->SetActive(false);
		this->_owner->GetComponent<PlayerAbility>()->_meteor->SetActive(false);
	}

	// P������ ��Ȱ
	if (this->_playerStatus == Status::DEAD && InputSystem::GetInstance()->KeyDown(KEY::P))
	{
		this->_playerInfo._hp = 500.0f;
		this->_playerStatus = Status::IDLE;
	}

	GRAPHICS->DrawDebugText(100, 400, 40, "%.3f", this->_playerInfo._hp);

	if (this->GetOwner()->GetComponent<BoxCollider>()->GetTargetObject() == nullptr)
	{
		GRAPHICS->DrawDebugText(100, 600, 40, "NULL");
	}
	else
	{
		GRAPHICS->DrawDebugText(100, 600, 40, this->GetOwner()->GetComponent<BoxCollider>()->GetTargetObject()->GetObjectName().c_str());
	}
	//GRAPHICS->DrawColorText()

}
