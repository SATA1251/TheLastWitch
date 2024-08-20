#include <random>
#include <algorithm>
#include <DirectXMath.h>
#include <math.h>
#include "GameObject.h"
#include "Animator.h"
#include "TimeManager.h"
#include "Transform.h"
#include "SceneManager.h"
#include "MeshRenderer.h"
#include "Scene.h"
#include "Player.h"
#include "ToolBox.h"
#include "Particle.h"
#include "BoxCollider.h"
#include "InputSystem.h"
#include "Navigation.h"
#include "EventManager.h"
#include "Boss.h"

#include "PlayerAbility.h"

KunrealEngine::Boss::Boss()
	: _info(), _status(BossStatus::ENTER), _boss(nullptr), _player(nullptr), _patternIndex(-1), _exPatternIndex(-1),
	_distance(0.0f), _startTime(1.0f), _isCorePattern(false),
	_basicPattern(), _corePattern(), _corePatternOrigin(), _nowTitlePattern(nullptr), _nowPlayingPattern(nullptr),
	_isStart(false), _isHit(false), _isRotateFinish(false), _isAngleCheck(false),
	_bossTransform(nullptr), _playerTransform(nullptr),
	_stopover(), _nodeCount(0), _direction(), _prevPos(), _backStepPos(),
	_isMoving(false), _isRotate(false), _backStepReady(false), _isHideFinish(false),
	_rotAngle(0.0f), _sumRot(0.0f), _prevRot(),
	_isSpecialPatternPlaying(false), _specialPatternTimer(0.0f), _specialPatternIndex(-1), _canPlaySpecialPattern(false),
	_specialPatternEndLogicPlay(false), _nowSpecialPattern(nullptr), _specialPatternPlayPhase(0), _goalPhase(1), _stopSpecialPattern(false),
	_isEnterInitialize(false), _deathTimer(0.5f), _finalPattern(nullptr), _updateTimer(0.0f), _isFinalPatternInit(false), _teltimer(0)
{
}

KunrealEngine::Boss::~Boss()
{
	for (auto& index : _basicPattern)
	{	
		for (auto& pattern : index)
		{
			if (pattern != nullptr)
			{
				delete pattern;
			}
		}
	}

	//auto collider = _boss->GetComponent<BoxCollider>();
	//if (collider != nullptr)
	//{
	//	_boss->DeleteComponent(collider);
	//}
	//
	//auto meshRenderer = _boss->GetComponent<MeshRenderer>();
	//if (meshRenderer != nullptr)
	//{
	//	_boss->DeleteComponent(meshRenderer);
	//}
}


void KunrealEngine::Boss::DebugTest()
{
	// �ִϸ��̼� Ȯ�ο�
	GRAPHICS->DrawDebugText(500, 0, 50, "%s", _boss->GetComponent<Animator>()->GetNowAnimationName().c_str());
	GRAPHICS->DrawDebugText(800, 0, 50, "%f", this->_info._hp);

	// ���� Ȯ�ο�
	switch (_status)
	{
		case BossStatus::ENTER:
		{
			GRAPHICS->DrawDebugText(200, 350, 20, "Boss Status : ENTER");
			break;
		}
		case BossStatus::IDLE:
		{
			GRAPHICS->DrawDebugText(200, 350, 20, "Boss Status : IDLE");
			GRAPHICS->DeleteAllLine();
			break;
		}
		case BossStatus::CHASE:
		{
			GRAPHICS->DrawDebugText(200, 350, 20, "Boss Status : CHASE");
			if (_stopover.size() > 0)
			{
				for (const auto& path : _stopover)
				{
					GRAPHICS->CreateDebugLine(path.first, path.second, DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f));
				}
			}
			break;
		}
		case BossStatus::STAGGERED:
		{
			GRAPHICS->DrawDebugText(200, 350, 20, "Boss Status : ON_STAGGERED");
			break;
		}
		case BossStatus::OFF_STAGGRED:
		{
			GRAPHICS->DrawDebugText(200, 350, 20, "Boss Status : OFF_STAGGERED");
			break;
		}
		case BossStatus::DEAD:
		{
			GRAPHICS->DrawDebugText(200, 350, 20, "Boss Status : DEAD");
			break;
		}
		case BossStatus::PATTERN_READY:
		{
			GRAPHICS->DrawDebugText(200, 350, 20, "Boss Status : PATTERN_READY");
			break;
		}
		case BossStatus::BASIC_ATTACK:
		{
			GRAPHICS->DrawDebugText(200, 350, 20, "Boss Status : BASIC_ATTACK");
			break;
		}
		case BossStatus::CORE_ATTACK:
		{
			GRAPHICS->DrawDebugText(200, 350, 20, "Boss Status : CORE_ATTACK");
			break;
		}
		case BossStatus::PATTERN_END:
		{
			GRAPHICS->DrawDebugText(200, 350, 20, "Boss Status : PATTERN_END");
			break;
		}
		default:
			break;
	}
}

void KunrealEngine::Boss::Initialize(GameObject* boss)
{
	_player = SceneManager::GetInstance().GetScene("Main")->GetGameObject("Player");

	_boss = boss;

	_boss->SetTag("Boss");

	_basicPattern.resize(_info.GetMaxPhase());

	_bossTransform = _boss->GetComponent<Transform>();
	_playerTransform = _player->GetComponent<Transform>();

	SetMesh();

	SetTexture();

	SetBossCollider();

	SetBossTransform();

	CreatePattern();

	SetSubObject(false);

	SortCorePattern();

	SetSubObjectScene();

	_particleTeleport = SceneManager::GetInstance().GetCurrentScene()->CreateObject("ParticleTeleport");
	_particleTeleport->_autoAwake = true;
	_particleTeleport->AddComponent<Particle>();
	_particleTeleport->GetComponent<Particle>()->SetParticleEffect("fx_SmokeyHalo1", "Resources/Textures/Particles/fx_SmokeyHalo1.dds", 1000);
	_particleTeleport->GetComponent<Particle>()->SetParticleDuration(2.0f, 0.2f);
	_particleTeleport->GetComponent<Particle>()->SetParticleVelocity(1.0f, true);
	_particleTeleport->GetComponent<Particle>()->SetParticleSize(60.0f, 60.0f);
	_particleTeleport->GetComponent<Particle>()->AddParticleColor(0.1f, 5.0f, 0.1f);
	_particleTeleport->GetComponent<Particle>()->SetParticleDirection(0.0f, 0.0f, 0.0f);
	_particleTeleport->GetComponent<Particle>()->SetOffSet(0.0f, 20.0f, 0.0f);
	//_particleTeleport->GetComponent<Particle>()->SetParticleCameraApply(true);
	//_particleTeleport->GetComponent<Particle>()->SetParticleAngle(270.0f, 0.0f, 0.0f);
	_particleTeleport->GetComponent<Particle>()->SetActive(false);

}

void KunrealEngine::Boss::Update()
{
	// ����� Ȯ�ο�
	DebugTest();

	// ĳ���Ͱ� �̵���
	if (InputSystem::GetInstance()->MouseButtonUp(1) || InputSystem::GetInstance()->KeyDown(KEY::SPACE))
	{
		// �̵�, ȸ�� ���õ� �͵� �ʱ�ȭ����
		_isRotateFinish = false;
		_isMoving = false;
		_nodeCount = 0;

		_isAngleCheck = false;
	}

	_updateTimer += TimeManager::GetInstance().GetDeltaTime();

	if (_updateTimer >= 2.0f)
	{
		// �̵�, ȸ�� ���õ� �͵� �ʱ�ȭ����
		_isRotateFinish = false;
		_isMoving = false;
		_nodeCount = 0;

		_isAngleCheck = false;

		_updateTimer = 0.0f;
	}

	if (_info._hp <= 0)
	{
		if (_status != BossStatus::DEAD)
		{
			PatternForceEnd();
			_status = BossStatus::DEAD;
		}
	}

	if (_player->GetComponent<Player>()->GetPlayerData()._hp <= 0)
	{
		if (_status != BossStatus::WIN)
		{
			PatternForceEnd();
			_status = BossStatus::WIN;
		}
	}

	if (_info._staggeredGauge <= 0)
	{
		PatternForceEnd();

		_status = BossStatus::STAGGERED;
	}

	SpecialAttack();

	SpecialAttack2();

	switch (_status)
	{
		case BossStatus::ENTER:
		{
			Enter();
			break;
		}
		case BossStatus::IDLE:
		{
			_info._phase = _goalPhase;
			Idle();
			break;
		}
		case BossStatus::CHASE:
		{
			Chase();
			break;
		}
		case BossStatus::STAGGERED:
		{
			Staggred();
			break;
		}
		case BossStatus::OFF_STAGGRED:
		{
			OffStaggred();
			break;
		}
		case BossStatus::DEAD:
		{
			Dead();
			break;
		}
		case BossStatus::PATTERN_READY:
		{
			PatternReady();
			break;
		}
		case BossStatus::BASIC_ATTACK:
		{
			BasicAttack();
			break;
		}
		case BossStatus::CORE_ATTACK:
		{
			CoreAttack();
			break;
		}
		case BossStatus::PATTERN_END:
		{
			PatternEnd();
			break;
		}
		case BossStatus::WIN:
		{
			Win();
			break;
		}
		default:
		{
			break;
		}
	}
}

void KunrealEngine::Boss::Enter()
{
	if (!_isEnterInitialize)
	{
		_bossOriginPos = _boss->GetComponent<Transform>()->GetPosition();

		auto playerPosition = _player->GetComponent<Transform>()->GetPosition();

		_boss->GetComponent<BoxCollider>()->SetActive(false);
		_boss->GetComponent<MeshRenderer>()->SetActive(false);
		
		_boss->GetComponent<BoxCollider>()->FixedUpdate();
		
		InitializeEnterCameraMove();
		
		_isEnterInitialize = true;
		
		SceneManager::GetInstance().GetCurrentScene()->GetGameObject("Player")->GetComponent<Player>()->SetPlayerBindFlag(true, 0);
	}


	auto isCameraMoveFinsh = EnterCameraMove();
	
	if (isCameraMoveFinsh)
	{
		EventManager::GetInstance().BattleUIOpen(500);

		_startTime -= TimeManager::GetInstance().GetDeltaTime();

		static int testCnt = 0;

		if (_startTime < 0.0f)
		{

			_isStart = true;
			_boss->GetComponent<Transform>()->SetPosition(_bossOriginPos);
			_boss->GetComponent<MeshRenderer>()->SetActive(true);
			_boss->GetComponent<BoxCollider>()->SetActive(true);

			_boss->GetComponent<Transform>()->Update();
			_boss->GetComponent<MeshRenderer>()->Update();
			_boss->GetComponent<BoxCollider>()->FixedUpdate();

			SceneManager::GetInstance().GetCurrentScene()->GetGameObject("Player")->GetComponent<Player>()->SetPlayerBindFlag(false);

			testCnt++;

			if (testCnt > 1)
			{
				int a = 10;
			}
		}

		// �����ϸ� 
		if (_isStart)
		{
			// ���� ���� �ִϸ��̼� ����
			_boss->GetComponent<Animator>()->Play("Emergence", _info._baseAnimSpeed, false);

			// ���� ���� �ִϸ��̼� ������ �����ٸ�
			if (_boss->GetComponent<Animator>()->GetIsAnimationPlaying() == false)
			{
				// ���� �ִϸ��̼� ������ ���� �ִϸ��̼� �������� 0���� �ǵ���
				_boss->GetComponent<Animator>()->Stop();
				// IDLE�� ���� ����

				_status = BossStatus::IDLE;
			}

			_canPlaySpecialPattern = true;
		}
	}
}

void KunrealEngine::Boss::Idle()
{
	_boss->GetComponent<Animator>()->Play("Idle", _info._baseAnimSpeed, true);

	// �ٽ� ������ �����Ǿ��ִٸ�
	if (!(_corePattern).empty())
	{
		// �ٽ� ������ back�������� hp�� ���� ������ �Ǿ��ֱ⿡
		// ������ ü���� �ٽ� ������ �ߵ� ü�� ���ϰ� �ȴٸ�
		if (_info._hp <= (_corePattern).back()->_triggerHp)
		{
			// �ھ��������� ���� Ȯ��
			_isCorePattern = true;

			_nowTitlePattern = _corePattern.back();

			// ���� �غ� ���·� ����
			_status = BossStatus::PATTERN_READY;

			// �Լ� ����
			return;
		}
	}

	// �ٽ� ������ �ƴ� �⺻ ������ ��쿡 ���� �⺻ ������ ����
	if (_patternIndex == -1 && !_isCorePattern)
	{
		// ���� ������ ���� ���� �ε����� ������
		_patternIndex = ToolBox::GetRandomNum(0, _basicPattern[_info._phase-1].size() - 1);

		if (_basicPattern[_info._phase-1].size() > 1)
		{
			while (_patternIndex == _exPatternIndex)
			{
				// ���� ������ ���� ���� �ε����� ������
				_patternIndex = ToolBox::GetRandomNum(0, _basicPattern[_info._phase-1].size() - 1);
			}

			// ������ ���� ������ Ȱ��ȭ�Ǿ����� �ʴٸ�
			while ((_basicPattern[_info._phase-1])[_patternIndex]->_isActive == false)
			{
				// ������ ���� ������ Ȱ��ȭ �Ǿ� ���������� �ٽ� ���� �ε����� ����
				_patternIndex = ToolBox::GetRandomNum(0, _basicPattern[_info._phase-1].size() - 1);
			}
		}
	}

	_nowTitlePattern = _basicPattern[_info._phase-1][_patternIndex];

	// Chase�� ���� ��ȯ
	_status = BossStatus::CHASE;

	if (_nowTitlePattern->_skipChase)
	{
		_status = BossStatus::PATTERN_READY;
	}
}

void KunrealEngine::Boss::Chase()
{
	// ���� ���� ���� �ִϸ��̼��� Idle �̶��
	if (_boss->GetComponent<Animator>()->GetNowAnimationName() == "Run")
	{
		_boss->GetComponent<Animator>()->Play("Run", _info._baseAnimSpeed, true);
	}
	else
	{
		_boss->GetComponent<Animator>()->Play("Idle", _info._baseAnimSpeed, true);
	}

	auto patternRange = _nowTitlePattern->_range;

	auto bossPosition = _bossTransform->GetPosition();

	auto playerPosition = _playerTransform->GetPosition();

	// ������ ��Ÿ��� �޾ƿ�
	if (!_isRotateFinish)
	{
		// ���� ���Խ� ���� üũ�� �Ǿ����� �ʴٸ�
		if (!_isAngleCheck)
		{
			// ������ ����
			_rotAngle = ToolBox::GetAngleWithDirection(bossPosition, playerPosition, _bossTransform->GetRotation().y);

			if (isnan(_rotAngle))
			{
				_rotAngle = 0.0f;
			}

			// ���� ������ ���
			_prevRot = _bossTransform->GetRotation();

			// ���� ������
			_sumRot = 0.0f;

			// �ι�° ���Խú��� �ȵ����� �ϵ���
			_isAngleCheck = true;
		}

		// ������ �÷��̾ �ٶ󺸰� ��
		_isRotateFinish = LookAtPlayer(_rotAngle, _info._rotationSpeed);
	}

	// ������ �÷��̾ �ٶ󺸰� �Ǿ��� ��
	if (_isRotateFinish)
	{
		if (_nowTitlePattern->_skipMove)
		{
			_status = BossStatus::PATTERN_READY;;
		}

		// ������ �÷��̾� ������ �Ÿ��� ����
		if (!_isMoving)
		{
			_distance = ToolBox::GetDistance(bossPosition, playerPosition);
		}

		// ���� ��Ÿ����� �÷��̾���� �Ÿ��� ����� ��� ���� ����
		if (_distance <= patternRange + _nowTitlePattern->_rangeOffset)
		{
			// ���� �غ� ���·� ����
			_status = BossStatus::PATTERN_READY;

			return;
		}

		// ���� ��Ÿ����� �÷��̾���� �Ÿ��� �� ���
		else
		{
			// ���� ��Ÿ����� �÷��̾��� �Ÿ��� ������ ���� �Ÿ� �̳��� ��� �ɾ �̵�
			if (_distance - patternRange <= 50.0f)
			{
				// ������ �÷��̾��� �� �� �ִ� ���� ���� ���
				if (_isMoving == false)
				{
					UpdateMoveNode();

					// ��� ����� �������Ƿ�
					_isMoving = true;

					_prevPos = _stopover[0].first;
				}

				// ����� ��� ���Ͱ� �����Ѵٸ�
				if (_stopover.size() > 0)
				{
					// �÷��̾ ���Ͽ� �̵�
 					auto isChasing = MoveToPlayer(_prevPos, _stopover[_nodeCount].second, _info._moveSpeed);

					// ù ��常ŭ �Ѿư��ٸ� ���� ��带 ������� �̵�
					if (!isChasing)
					{
						// ���� ��� ���� ���Ϳ� �°� ȸ��
						if (_nodeCount + 1 < _stopover.size())
						{
							_isRotate = false;
							_isRotate = RotateToTarget(_stopover[_nodeCount + 1].second);
						}
						else
						{
							_nodeCount++;
						}

						// ȸ���� �����ٸ� ���� ���� �̵�
						if (_isRotate)
						{
							_nodeCount++;
						}
					}
				}

				// ��� ��带 �о� ������ �÷��̾ �� �Ѿư��ٸ�
				if (_nodeCount >= _stopover.size())
				{
					// ���� �غ� ���·� ����
					_status = BossStatus::PATTERN_READY;

					return;
				}
			}

			// ���� ��Ÿ����� �÷����� �Ÿ��� �ְ� ���� �Ÿ��� ��� ��� �ڷ���Ʈ�� �̵�
			else
			{
				TeleportToPlayer();

				// ���� �غ� ���·� ����
				_status = BossStatus::PATTERN_READY;

				return;
			}
		}
	}
}

void KunrealEngine::Boss::Staggred()
{
	// ����ȭ �ִϸ��̼� ����
	_boss->GetComponent<Animator>()->Play("Staggred", _info._baseAnimSpeed, false);

	// ����ȭ �ִϸ��̼��� �����ٸ�
	if (_boss->GetComponent<Animator>()->GetIsAnimationPlaying() == false)
	{
		_boss->GetComponent<Animator>()->Stop();
		_status = BossStatus::OFF_STAGGRED;
	}
}

void KunrealEngine::Boss::OffStaggred()
{
	// ����ȭ ���� �ִϸ��̼� ����
	_boss->GetComponent<Animator>()->Play("OffStaggred", _info._baseAnimSpeed, false);

	// ����ȭ ���� �ִϸ��̼��� �����ٸ�
	if (_boss->GetComponent<Animator>()->GetIsAnimationPlaying() == false)
	{
		_boss->GetComponent<Animator>()->Stop();

		_info._staggeredGauge = _info.GetMaxStaggeredGauage();
		_status = BossStatus::IDLE;
	}
}

void KunrealEngine::Boss::Dead()
{
	if (!_isFinalPatternInit)
	{
		_canPlaySpecialPattern = false;

		_finalPattern->Initialize();

		_isFinalPatternInit = true;
	}

	if (_finalPattern->_index == 0)
	{
		_finalPattern->Play();
	}
	
	// ������ ������ �ִϸ��̼� ����
	//auto isPlaying = _boss->GetComponent<Animator>()->Play("Dead", _info._baseAnimSpeed, false);
	//
	//if (!isPlaying)
	//{
	//	if (_deathTimer >= 0.0f)
	//	{
	//		_boss->GetComponent<MeshRenderer>()->SetIsDissolve(true);
	//
	//		_boss->GetComponent<MeshRenderer>()->SetDissolve(_deathTimer);
	//	}
	//	else
	//	{
	//		_boss->GetComponent<MeshRenderer>()->SetActive(false);
	//		_boss->GetComponent<BoxCollider>()->SetActive(false);
	//	}
	//
	//	if (_boss->GetComponent<MeshRenderer>()->GetActivated())
	//	{
	//		_deathTimer -= TimeManager::GetInstance().GetDeltaTime() * 0.2f;
	//	}
	//}
}


void KunrealEngine::Boss::PatternReady()
{
	// ��ã�� �������Ƿ� ���Ͽ����� ��ã�⸦ ���� �ʱ�ȭ
	_nodeCount = 0;

	// ���� ���� ���� Idle �ִϸ��̼��� ����
	_boss->GetComponent<Animator>()->Stop();

	// ���� ���� ù ������ ���� ���� ������Ʈ���� ��� ��
	for (const auto& object : _nowTitlePattern->_subObject)
	{
		// ��� ������Ʈ�� ��������, ���� ���ο��� �˾Ƽ� ó�� �ؾ� ��
		object->SetActive(true);

		// ��� ������Ʈ�� ��
		object->SetTotalComponentState(false);
	}

	// ���ϳ��� ù ���� �ʱ�ȭ������� �͵� �ʱ�ȭ
	_nowTitlePattern->Initialize();

	// �ھ� ������ ���
	if (_isCorePattern)
	{
		// Core Attack���� ���� ����
		_status = BossStatus::CORE_ATTACK;
	}
	else
	{
		// Basic Attack���� ���� ����
		_status = BossStatus::BASIC_ATTACK;
	}

	_isAngleCheck = false;
}

void KunrealEngine::Boss::BasicAttack()
{
	auto index = _basicPattern[_info._phase-1][_patternIndex]->_index;
	_nowPlayingPattern = _basicPattern[_info._phase-1][_patternIndex]->_patternList[index];

	// ������ ����
	auto isPatternPlaying = _nowTitlePattern->Play();

	// ���� ������ �����ٸ�
	if (isPatternPlaying == false)
	{
		_boss->GetComponent<Animator>()->Stop();
		_status = BossStatus::PATTERN_END;
	}
}

void KunrealEngine::Boss::CoreAttack()
{
	auto index = _corePattern.back()->_index;
	_nowPlayingPattern = _nowTitlePattern->_patternList[index];

	// ������ ����
	auto isPatternPlaying = _nowTitlePattern->Play();

	// ���� ������ �����ٸ�
	if (isPatternPlaying == false)
	{
		_boss->GetComponent<Animator>()->Stop();
		_status = BossStatus::PATTERN_END;
	}
}


void KunrealEngine::Boss::SpecialAttack()
{
	if (_info._phase != _specialPatternPlayPhase || _stopSpecialPattern)
	{
		if (_isSpecialPatternPlaying)
		{
			for (const auto& pattern : _specialPattern[_specialPatternIndex]->_patternList)
			{
				for (int i = 0; i < pattern->_isColliderActive.size(); i++)
				{
					pattern->_isColliderHit[i] = false;
					pattern->_isColliderActive[i] = false;
				}

				for (const auto& object : pattern->_subObject)
				{
					object->SetTotalComponentState(false);

					object->SetActive(false);

					_isSpecialPatternPlaying = false;

					_specialPatternIndex = -1;
					_specialPatternTimer = 0.0f;

					_specialPatternEndLogicPlay = false;
				}
			}
		}
		return;
	}

	if (_specialPattern.empty())
	{
		return;
	}

	if (!_canPlaySpecialPattern)
	{
		return;
	}

	if (!_isSpecialPatternPlaying)
	{
		_specialPatternIndex = ToolBox::GetRandomNum(_specialPattern.size()-1);

		_isSpecialPatternPlaying = true;

		_specialPattern[_specialPatternIndex]->Initialize();


		for (auto& object : _specialPattern[_specialPatternIndex]->_subObject)
		{
			// ��� ������Ʈ�� ��������, ���� ���ο��� �˾Ƽ� ó�� �ؾ� ��
			object->SetActive(true);

			// ��� ������Ʈ�� ��
			object->SetTotalComponentState(false);
		}

	}

	auto isPlaying = _specialPattern[_specialPatternIndex]->SpecialPatternPlay();

	if (isPlaying)
	{
		auto index = _specialPattern[_specialPatternIndex]->_index;
		_nowSpecialPattern = _specialPattern[_specialPatternIndex]->_patternList[index];
	}

	// ���� ������ �����ٸ�
	if (isPlaying == false)
	{
		if (!_specialPatternEndLogicPlay)
		{
			for (const auto& pattern : _specialPattern[_specialPatternIndex]->_patternList)
			{
				for (int i = 0; i < pattern->_isColliderActive.size(); i++)
				{
					pattern->_isColliderHit[i] = false;
					pattern->_isColliderActive[i] = false;
				}

				for (const auto& object : pattern->_subObject)
				{
					object->SetTotalComponentState(false);
				
					object->SetActive(false);
				}
			}

			_specialPatternEndLogicPlay = true;
		}


		_specialPatternTimer += TimeManager::GetInstance().GetDeltaTime();

		if (_specialPatternTimer >= 2.0f)
		{
			_isSpecialPatternPlaying = false;
			
			_specialPatternIndex = -1;
			_specialPatternTimer = 0.0f;

			_specialPatternEndLogicPlay = false;
		}
	}
}


void KunrealEngine::Boss::SpecialAttack2()
{

}

void KunrealEngine::Boss::PatternEnd()
{
	// ���� ������ ���� �ʱ�ȭ
	_isAngleCheck = false;

	_isHideFinish = false;
	_isMoving = false;
	_isRotateFinish = false;

	// ���� ���� �ε��� ����
	_exPatternIndex = _patternIndex;

	// Idle �ִϸ��̼� ����
	_boss->GetComponent<Animator>()->Play("Idle", _info._baseAnimSpeed, true);

	// ��� ���� ������Ʈ�� ��
	if (_patternIndex == -1)
	{
		for (const auto& pattern : _corePattern.back()->_patternList)
		{
			for (int i = 0; i < pattern->_isColliderActive.size(); i++)
			{
				pattern->_isColliderHit[i] = false;
				pattern->_isColliderActive[i] = false;
			}

			for (const auto& object : pattern->_subObject)
			{
				object->SetTotalComponentState(false);
				
				object->SetActive(false);
			}
		}
	}

	else
	{
		for (const auto& pattern : _basicPattern[_info._phase-1][_patternIndex]->_patternList)
		{
			for (int i = 0; i < pattern->_isColliderActive.size(); i++)
			{
				pattern->_isColliderHit[i] = false;
				pattern->_isColliderActive[i] = false;
			}

			for (const auto& object : pattern->_subObject)
			{
				object->SetTotalComponentState(false);

				object->SetActive(false);
			}
		}
	}

	// 1�ʵ��� ����� �� Idle ���·� ��ȯ
	Startcoroutine(patternEnd);
}


void KunrealEngine::Boss::Win()
{
	auto isPlaying = _boss->GetComponent<Animator>()->Play("BattleCry", _info._baseAnimSpeed, true);
}

void KunrealEngine::Boss::InitializeEnterCameraMove()
{

}

bool KunrealEngine::Boss::EnterCameraMove()
{
	return true;
}

void KunrealEngine::Boss::PatternForceEnd()
{
	// ���� ������ ���� �ʱ�ȭ
	_isAngleCheck = false;

	_isHideFinish = false;
	_isMoving = false;
	_isRotateFinish = false;

	// ���� ���� �ε��� ����
	_exPatternIndex = _patternIndex;

	// Idle �ִϸ��̼� ����
	_boss->GetComponent<Animator>()->Stop();

	// ��� ���� ������Ʈ�� ��
	if (_patternIndex == -1)
	{
		for (const auto& object : _corePattern.back()->_subObject)
		{
			object->SetActive(false);
		}
	}

	else
	{
		for (const auto& object : _basicPattern[_info._phase-1][_patternIndex]->_subObject)
		{
			object->SetActive(false);
		}
	}
}


void KunrealEngine::Boss::ChangePhase(unsigned int phase)
{
	_goalPhase = phase;
}

void KunrealEngine::Boss::SetInfo(BossBasicInfo& info)
{
	_info = info;
}

void KunrealEngine::Boss::SetBasicPatternList(std::vector<BossPattern*>* basicPatternList)
{
}

void KunrealEngine::Boss::SetCorePatternList(std::vector<BossPattern*>* corePatternList)
{
	SortCorePattern();
}

const BossStatus& KunrealEngine::Boss::GetStatus()
{
	return _status;
}


const float KunrealEngine::Boss::GetDamage()
{
	if (_nowTitlePattern)
	{
		return _nowPlayingPattern->_damage;
	}
	else
	{
		return 0.0f;
	}
}


BossPattern* KunrealEngine::Boss::GetNowPattern()
{
	return _nowTitlePattern;
}


BossPattern* KunrealEngine::Boss::GetNowPlayingPattern()
{
	return _nowPlayingPattern;
}


BossPattern* KunrealEngine::Boss::GetNowSpecialPattern()
{
	return _nowSpecialPattern;
}

BossBasicInfo& KunrealEngine::Boss::GetBossInfo()
{
	return _info;
}


bool KunrealEngine::Boss::isDead()
{
	if (_info._hp <= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void KunrealEngine::Boss::SetStartTime(float time)
{
	_startTime = time;
}

bool KunrealEngine::Boss::CompareCorePattern(const BossPattern* pattern1, const BossPattern* pattern2)
{
	return pattern1->_triggerHp < pattern2->_triggerHp;
}

void KunrealEngine::Boss::SortCorePattern()
{
	std::sort((_corePattern).begin(), (_corePattern).end(), CompareCorePattern);

	_corePatternOrigin = _corePattern;
}


void KunrealEngine::Boss::SetSubObjectScene()
{
	//auto bossScene = _boss->GetObjectScene();
	//
	//for (const auto& index : _basicPattern)
	//{
	//	for (const auto& object : index->_subObject)
	//	{
	//		object->changescene()?
	//	}
	//}
}

float KunrealEngine::Boss::CalculateAngle(const DirectX::XMFLOAT3& bossPosition, const DirectX::XMFLOAT3& playerPosition)
{
	// ������ �÷��̾� ������ ������ ���
	auto angle = ToolBox::GetAngle(bossPosition, playerPosition);

	auto bossAngle = _boss->GetComponent<Transform>()->GetRotation().y;

	// ���纸���� ������ ����� �÷��̾���� �������� ���̰� 180 �����ϰ��
	// angle��ŭ �״�� �����̸� �� (�̷��� ���ϸ� 90, 270�� �� 270���� ���ƹ��� ��ȿ����)
	if (std::abs(bossAngle - angle) < 180.0f)
	{
		return angle;
	}
	// �������� ���̰� 180���� �������
	else
	{
		// angle�� ������ ���
		if (angle < 0)
		{
			return 360.0f + angle;
		}
		else
		{
			return angle - 360.0f;
		}
	}
}


void KunrealEngine::Boss::SetSpecialPatternPlayPhase(unsigned int phase)
{
	_specialPatternPlayPhase = phase;
}


void KunrealEngine::Boss::StopSpecialPattern()
{
	_stopSpecialPattern = true;

	if (_isSpecialPatternPlaying)
	{
		for (const auto& pattern : _specialPattern[_specialPatternIndex]->_patternList)
		{
			for (int i = 0; i < pattern->_isColliderActive.size(); i++)
			{
				pattern->_isColliderHit[i] = false;
				pattern->_isColliderActive[i] = false;
			}

			for (const auto& object : pattern->_subObject)
			{
				object->SetTotalComponentState(false);

				object->SetActive(false);

				_isSpecialPatternPlaying = false;

				_specialPatternIndex = -1;
				_specialPatternTimer = 0.0f;

				_specialPatternEndLogicPlay = false;
			}
		}
	}
}


void KunrealEngine::Boss::ResetBoss()
{
	_status = BossStatus::ENTER;

	_distance = 0.0f;
	_startTime = 1.0f;
	_isCorePattern = false;
	_corePattern = _corePatternOrigin;
	_nowTitlePattern = nullptr;
	_nowPlayingPattern = nullptr;
	_isStart = false;
	_isHit = false;
	_isRotateFinish = false;
	_isAngleCheck = false;
	_stopover = {};
	_nodeCount = 0;
	_direction = {};
	_prevPos = {};
	_backStepPos = {};
	_isMoving = false;
	_isRotate = false;
	_backStepReady = false;
	_isHideFinish = false;
	_rotAngle = 0.0f;
	_sumRot = 0.0f;
	_prevRot = {};
	_isSpecialPatternPlaying = false;
	_specialPatternTimer = 0.0f;
	_specialPatternIndex = -1;
	_canPlaySpecialPattern = false;
	_specialPatternEndLogicPlay = false;
	_nowSpecialPattern = nullptr;
	_specialPatternPlayPhase = 0;
	_goalPhase = 1;
	_stopSpecialPattern = false;
	_isEnterInitialize = false;
	_deathTimer = 0.5f;
	_updateTimer = 0.0f;
	_isFinalPatternInit = false;

	SetSubObject(false);
}

bool KunrealEngine::Boss::MoveToPlayer(DirectX::XMFLOAT3& startPos, DirectX::XMFLOAT3& targetPos, float speed)
{
	float moveSpeed = speed * TimeManager::GetInstance().GetDeltaTime();

	// ���������� �Ÿ� ���
	auto dist = ToolBox::GetDistance(startPos, targetPos);
	if (dist > 0.5f)
	{
		// �÷��̾� �̵�
		_boss->GetComponent<Animator>()->Play("Run", speed, true);

		DirectX::XMVECTOR currentPosVec = DirectX::XMLoadFloat3(&startPos);

		DirectX::XMVECTOR direction = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&targetPos), currentPosVec);

		direction = DirectX::XMVector3Normalize(direction);

		DirectX::XMVECTOR newPosition = DirectX::XMVectorAdd(currentPosVec, DirectX::XMVectorScale(direction, moveSpeed));
		_bossTransform->SetPosition(newPosition.m128_f32[0], _bossTransform->GetPosition().y, newPosition.m128_f32[2]);

		_prevPos = _bossTransform->GetPosition();

		return true;
	}

	else
	{
		return false;
	}
}

void KunrealEngine::Boss::TeleportToPlayer()
{
	auto playerDirection = _player->GetComponent<Player>()->GetDirectionVector();
	auto originPlayerPos = _playerTransform->GetPosition();
	auto currentPosVec = DirectX::XMLoadFloat3(&originPlayerPos);

	auto patternRange = _nowTitlePattern->_range;
	DirectX::XMVECTOR newPlayerPosition = DirectX::XMVectorAdd(currentPosVec, DirectX::XMVectorScale(playerDirection, 10.0f + patternRange));

	// ������ �÷��̾� ������ ��� ���
	Navigation::GetInstance().SetSEpos(1, _bossTransform->GetPosition().x, _bossTransform->GetPosition().y, _bossTransform->GetPosition().z,
		newPlayerPosition.m128_f32[0], _bossTransform->GetPosition().y, newPlayerPosition.m128_f32[2]);

	_stopover = Navigation::GetInstance().FindStraightPath(1);

	for (auto& index : _stopover)
	{
		index.first.y = _bossTransform->GetPosition().y;
		index.second.y = _bossTransform->GetPosition().y;
	}

	if (_stopover.empty())
	{
		return;
	}

	auto bossRotation = _bossTransform->GetRotation();
	auto playerRotation = _playerTransform->GetRotation();

	// ���⿡ �÷��̾��� ���� ���� * ���� �Ÿ��� ���� �÷��̾�� ��¦ ���������� �ؾ� ��
	// �÷��̾��� ���� ���͸� �����;���

	auto target = _stopover.back().second;

	_prevPos = _bossTransform->GetPosition();

	_bossTransform->SetPosition(target.x, target.y, target.z);

	// �÷��̾ �ٶ󺸵��� 
	auto angle = CalculateAngle(_bossTransform->GetPosition(), _playerTransform->GetPosition());

	_bossTransform->SetRotation(bossRotation.x, angle, bossRotation.z);

	_prevRot = _bossTransform->GetRotation();
}


bool KunrealEngine::Boss::Teleport(const DirectX::XMFLOAT3& targetPos, bool lookAtPlayer, float hideTime)
{
	if (_isHideFinish == false)
	{
		_boss->GetComponent<Animator>()->Stop();

		_boss->GetComponent<MeshRenderer>()->SetActive(false);
		_boss->GetComponent<MeshRenderer>()->Update();
		_boss->GetComponent<BoxCollider>()->SetActive(false);
		_boss->GetComponent<BoxCollider>()->FixedUpdate();

		// ����������� ����Ʈ

		_teltimer += TimeManager::GetInstance().GetDeltaTime();

		if (_teltimer < 1.0f)
		{
			_particleTeleport->GetComponent<Particle>()->SetActive(true);
			_particleTeleport->GetComponent<Particle>()->SetParticleSize(60 - (_teltimer * 60), 60 - (_teltimer * 60));
			_particleTeleport->GetComponent<Transform>()->SetPosition(this->_bossTransform->GetPosition());
		}
		else
		{
			_particleTeleport->GetComponent<Particle>()->SetActive(false);
		}

		Startcoroutine(TeleportWithHide);
	}

	if (_isHideFinish == true)
	{
		_teltimer = 0;

		// Ÿ�� ������������ ��� ���
		DirectX::XMVECTOR targetVec = DirectX::XMLoadFloat3(&targetPos);

		Navigation::GetInstance().SetSEpos(1, _bossTransform->GetPosition().x, _bossTransform->GetPosition().y, _bossTransform->GetPosition().z,
			targetVec.m128_f32[0], _bossTransform->GetPosition().y, targetVec.m128_f32[2]);

		_stopover = Navigation::GetInstance().FindStraightPath(1);

		for (auto& index : _stopover)
		{
			index.first.y = _bossTransform->GetPosition().y;
			index.second.y = _bossTransform->GetPosition().y;
		}

		if (_stopover.empty())
		{
			return true;
		}

		auto bossRotation = _bossTransform->GetRotation();
		auto playerRotation = _playerTransform->GetRotation();

		auto target = _stopover.back().second;
		_bossTransform->SetPosition(target.x, target.y, target.z);
		_prevPos = _bossTransform->GetPosition();

		// �÷��̾ �ٶ󺸵��� 
		if (lookAtPlayer)
		{
			auto angle = CalculateAngle(_bossTransform->GetPosition(), _playerTransform->GetPosition());
			_bossTransform->SetRotation(bossRotation.x, angle, bossRotation.z);
		}

		_boss->GetComponent<MeshRenderer>()->SetActive(true);
		_boss->GetComponent<MeshRenderer>()->Update();
		_boss->GetComponent<BoxCollider>()->SetActive(true);
		_boss->GetComponent<BoxCollider>()->FixedUpdate();


		_prevRot = _bossTransform->GetRotation();

		return true;
	}

	return false;
}


void KunrealEngine::Boss::ForceMove(const DirectX::XMFLOAT3& targetPos)
{
	_prevPos = _bossTransform->GetPosition();
	_bossTransform->SetPosition(targetPos);
}

bool KunrealEngine::Boss::LookAtPlayer(float angle, float rotateSpeed)
{
	// ȸ�� �ӵ�
	float speed = TimeManager::GetInstance().GetDeltaTime() * rotateSpeed;

	// ���� ����
	float errorRange = speed * 2.0f;

	_sumRot += speed;

	// ���� ������ ��ǥ�� �ϴ� �������� ���� ���
	if (angle > 0)
	{
		if (_sumRot < angle)
		{
			// ȸ�� �ӵ���ŭ ȸ��
			_bossTransform->SetRotation(_prevRot.x, _prevRot.y + _sumRot, _prevRot.z);
		}
	}
	else
	{
		if (_sumRot < std::abs(angle))
		{
			// ȸ�� �ӵ���ŭ ȸ��
			_bossTransform->SetRotation(_prevRot.x, _prevRot.y - _sumRot, _prevRot.z);
		}
	}

	if (_sumRot > std::abs(angle))
	{
		// ���� ���� ���� �ִϸ��̼��� Idle �̶��
		if (_boss->GetComponent<Animator>()->GetNowAnimationName() == "Idle")
		{
			// Idle �ִϸ��̼��� ���߰�
			_boss->GetComponent<Animator>()->Stop();
		}

		// ȸ���� �Ϸ�Ǿ��ٰ� ��ȯ
		_isAngleCheck = false;
		_sumRot = 0.0f;
		_prevRot = _bossTransform->GetRotation();

		return true;
	}
	else
	{
		return false;
	}
}


bool KunrealEngine::Boss::Move(DirectX::XMFLOAT3& targetPos, float speed, bool roateToTarget, bool rayCast)
{
	// Ÿ�� �������� ȸ���� ���Ҷ�
	bool finshRoate = true;
	if (roateToTarget == true)
	{
		finshRoate = RotateToTarget(targetPos);
	}

	// ȸ���� �����ٸ�
	if (finshRoate == true)
	{
		// �� �Լ��� ó�� ���ͼ� �ѹ��� ���������� �ʾҴٸ� ��ã�� (�ѹ��� ��������)
		if (_isMoving == false)
		{
			// �տ� ��ü�� ������ �ű������ �̵���Ű�� �ʹٸ�
			if (rayCast)
			{
				// ���θ��� ��ü�� �������� ã��
				Navigation::GetInstance().SetSEpos(1, _bossTransform->GetPosition().x, _bossTransform->GetPosition().y, _bossTransform->GetPosition().z,
					targetPos.x, _bossTransform->GetPosition().y, targetPos.z);

				auto targetPosition = Navigation::GetInstance().FindRaycastPath(1);

				// �� ��ü�� �������� ������� �׺�޽� ��ã��
				Navigation::GetInstance().SetSEpos(1, _bossTransform->GetPosition().x, _bossTransform->GetPosition().y, _bossTransform->GetPosition().z,
					targetPosition.x, _bossTransform->GetPosition().y, targetPosition.z);

				_stopover = Navigation::GetInstance().FindStraightPath(1);

				if (_stopover.empty())
				{
					return false;
				}

				for (auto& index : _stopover)
				{
					index.first.y = _bossTransform->GetPosition().y;
					index.second.y = _bossTransform->GetPosition().y;
				}

				_prevPos = _stopover[0].first;
			}
			// �׳� Ÿ�� �����Ǳ��� �׺�޽��� ��ã��
			else
			{
				Navigation::GetInstance().SetSEpos(1, _bossTransform->GetPosition().x, _bossTransform->GetPosition().y, _bossTransform->GetPosition().z,
					targetPos.x, _bossTransform->GetPosition().y, targetPos.z);

				_stopover = Navigation::GetInstance().FindStraightPath(1);

				for (auto& index : _stopover)
				{
					index.first.y = _bossTransform->GetPosition().y;
					index.second.y = _bossTransform->GetPosition().y;
				}

				_prevPos = _stopover[0].first;
			}

			// ���� �����̱� ����
			_isMoving = true;
		}

		// ���� �����ִٸ�
		if (_stopover.size() > 0)
		{
			// Ÿ���� ���Ͽ� �̵�
			//auto trs = _bossTransform->GetPosition();
			auto isChasing = MoveToTarget(_prevPos, _stopover[_nodeCount].second, speed);

			// ��常ŭ �Ѿư��ٸ�
			if (!isChasing)
			{
				// ���� ��带 ������� �̵�
				_nodeCount++;
			}

			// ��� ��带 �о� ������ Ÿ�ٿ� �����ߴٸ�
			if (_nodeCount >= _stopover.size())
			{
				// ���� ������ ���� �ʱ�ȭ
				_nodeCount = 0;
				_isRotate = false;
				_isRotateFinish = false;

				_backStepReady = false;

				return true;
			}
		}
	}

	return false;
}


std::function<bool()> KunrealEngine::Boss::CreateBackStepLogic(BossPattern* pattern, float moveSpeed, float distance)
{
	auto backStepLogic = [pattern, this]()
	{
		auto moveFinish = BackStep(50.0f, 30.0f);

		if (moveFinish == true)
		{
			return false;
		}

		// ���� ������ �����Ŵ
		pattern->_playNextPattern = true;

		return true;
	};

	return backStepLogic;
}


std::function<bool()> KunrealEngine::Boss::CreateBasicAttackLogic(BossPattern* pattern, GameObject* subObject, float activeColliderFrame)
{
	pattern->SetSubObject(subObject);

	auto attackLogic = [pattern, subObject, activeColliderFrame, this]()
	{
		auto animator = _boss->GetComponent<Animator>();
		auto isAnimationPlaying = animator->Play(pattern->_animName, pattern->_speed, false);


		// ���� �������� �Ѿ�� ������ üũ�� �ݶ��̴��� Ű��
		if (pattern->_colliderOnCount > 0)
		{
			if (animator->GetCurrentFrame() >= activeColliderFrame)
			{
				if (subObject != nullptr)
				{
					subObject->GetComponent<BoxCollider>()->SetActive(true);
				}

				if (subObject->GetComponent<Particle>() != nullptr)
				{
					subObject->GetComponent<Particle>()->SetActive(true);
				}
			}
		}

		// 1Ÿ�� �¾Ҵٸ�
		if (pattern->_colliderOnCount == 0)
		{
			pattern->SetNextPatternForcePlay(true);
		}

		if (isAnimationPlaying == false)
		{
			return false;
		}

		return true;
	};

	return attackLogic;
}


std::function<bool()> KunrealEngine::Boss::CreateBasicAttackLogic(BossPattern* pattern, GameObject* subObject, GameObject* subObject2, float activeColliderFrame)
{
	pattern->SetSubObject(subObject);

	auto attackLogic = [pattern, subObject, subObject2, activeColliderFrame, this]()
	{
		auto animator = _boss->GetComponent<Animator>();
		auto isAnimationPlaying = animator->Play(pattern->_animName, pattern->_speed, false);


		// ���� �������� �Ѿ�� ������ üũ�� �ݶ��̴��� Ű��
		if (pattern->_colliderOnCount > 0)
		{
			if (animator->GetCurrentFrame() >= activeColliderFrame)
			{
				if (subObject != nullptr)
				{
					subObject->GetComponent<BoxCollider>()->SetActive(true);
					subObject2->GetComponent<MeshRenderer>()->SetActive(true);
				}

				if (subObject->GetComponent<Particle>() != nullptr)
				{
					subObject->GetComponent<Particle>()->SetActive(true);
				}
			}
		}

		// 1Ÿ�� �¾Ҵٸ�
		if (pattern->_colliderOnCount == 0)
		{
			pattern->SetNextPatternForcePlay(true);
		}

		if (isAnimationPlaying == false)
		{
			return false;
		}

		return true;
	};

	return attackLogic;
}

std::function<bool()> KunrealEngine::Boss::CreateBasicAttackLogic(BossPattern* pattern, const std::string& animName, GameObject* subObject, float activeColliderFrame)
{
	auto attackLogic = [pattern, animName, subObject, activeColliderFrame, this]()
	{
		auto animator = _boss->GetComponent<Animator>();
		auto isAnimationPlaying = animator->Play(animName, pattern->_speed, false);

		// ���� �������� �Ѿ�� ������ üũ�� �ݶ��̴��� Ű��
		if (pattern->_colliderOnCount > 0)
		{
			if (animator->GetCurrentFrame() >= activeColliderFrame)
			{
				if (subObject != nullptr)
				{
					subObject->GetComponent<BoxCollider>()->SetActive(true);
				}
			}
		}

		// 1Ÿ�� �¾Ҵٸ�
		if (pattern->_colliderOnCount == 0)
		{
			pattern->SetNextPatternForcePlay(true);
		}

		if (isAnimationPlaying == false)
		{
			return false;
		}

		return true;
	};

	return attackLogic;
}

bool KunrealEngine::Boss::BackStep(float speed, float distance)
{
	if (_backStepReady == false)
	{
		_backStepPos = _bossTransform->GetPosition();

		auto dir = GetDirection();
		
		DirectX::XMStoreFloat3(&_direction, dir);

		_backStepReady = true;

		_isMoving = false;
	}

	DirectX::XMFLOAT3 newPosition;

	newPosition.x = _backStepPos.x - distance * _direction.x;
	newPosition.y = _backStepPos.y;
	newPosition.z = _backStepPos.z - distance * _direction.z;

	return Move(newPosition, speed, false, true);
}

const DirectX::XMVECTOR KunrealEngine::Boss::GetDirection()
{
	auto direction = ToolBox::RotateVector(DirectX::XMFLOAT3(0, 0, -1.0f), _bossTransform->GetRotation().y);

	DirectX::XMVECTOR dirVec = DirectX::XMLoadFloat3(&direction);

	return dirVec;
}


KunrealEngine::Boss* KunrealEngine::Boss::GetBoss()
{
	return this;
}

void KunrealEngine::Boss::SetBossCollider()
{
	_boss->AddComponent<BoxCollider>();
}


void KunrealEngine::Boss::SetSubObject(bool tf)
{
	for (const auto& pattern : _basicPattern[_info._phase-1])
	{
		for (const auto& subPattern : pattern->_patternList)
		{
			for (const auto& object : subPattern->_subObject)
			{
				object->SetTotalComponentState(false);

				// ������Ʈ�� ���� ��Ȱ��ȭ ��Ű��
				object->SetActive(tf);

				object->SetTag("BossSub");
			}
		}
	}

	for (const auto& pattern : _corePattern)
	{
		for (const auto& subPattern : pattern->_patternList)
		{
			for (const auto& object : subPattern->_subObject)
			{
				object->SetTotalComponentState(false);

				// ������Ʈ�� ���� ��Ȱ��ȭ ��Ű��
				object->SetActive(tf);

				object->SetTag("BossSub");
			}
		}
	}
}

bool KunrealEngine::Boss::RotateToTarget(const DirectX::XMFLOAT3& targetPos)
{
	auto bossPosition = _bossTransform->GetPosition();

	if (!_isAngleCheck)
	{
		_rotAngle = ToolBox::GetAngleWithDirection(bossPosition, targetPos, _bossTransform->GetRotation().y);

		_prevRot = _bossTransform->GetRotation();

		_sumRot = 0.0f;

		_isAngleCheck = true;
	}

	// ������ ��Ÿ��� �޾ƿ�
	if (!_isRotate)
	{
		// ������ �÷��̾ �ٶ󺸰� ��
		_isRotate = LookAtPlayer(_rotAngle, _info._rotationSpeed);
	}

	if (_isRotate)
	{
		_isAngleCheck = false;
		return true;
	}

	return false;
}


bool KunrealEngine::Boss::Rotate(float angle, float speed)
{
	return LookAtPlayer(angle, speed);
}


void KunrealEngine::Boss::RotateByAngle(float angle)
{
	_bossTransform->SetRotation(_bossTransform->GetRotation().x, angle, _bossTransform->GetRotation().z);

	_prevRot = _bossTransform->GetRotation();
}

bool KunrealEngine::Boss::RotateClockWise(float rotateSpeed, bool isClockWise)
{
	if (isClockWise)
	{
		return LookAtPlayer(360.0f, rotateSpeed);
	}
	else
	{
		return LookAtPlayer(-360.0f, rotateSpeed);
	}
}


bool KunrealEngine::Boss::RotateClockWise(float goalAngle, float rotateSpeed, bool isClockWise)
{
	if (isClockWise)
	{
		return LookAtPlayer(goalAngle, rotateSpeed);
	}
	else
	{
		return LookAtPlayer(-goalAngle, rotateSpeed);
	}
}

void KunrealEngine::Boss::SetTexture()
{

}

void KunrealEngine::Boss::SetBossTransform()
{

}

const std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3> KunrealEngine::Boss::GetBossPosition()
{
	std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3> pos;

	pos.first = _prevPos;
	pos.second = _bossTransform->GetPosition();

	return pos;
}

void KunrealEngine::Boss::UpdateMoveNode()
{
	auto bossPos = _bossTransform->GetPosition();
	auto playerPos = _playerTransform->GetPosition();

	auto tempVec = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&bossPos), DirectX::XMLoadFloat3(&playerPos));
	auto dirVec = DirectX::XMVector3Normalize(tempVec);
	auto range = _nowTitlePattern->_range + _nowTitlePattern->_rangeOffset;

	Navigation::GetInstance().SetSEpos(1, _bossTransform->GetPosition().x, _bossTransform->GetPosition().y, _bossTransform->GetPosition().z,
		playerPos.x + range * dirVec.m128_f32[0], _bossTransform->GetPosition().y, playerPos.z + range * dirVec.m128_f32[2]);

	_stopover = Navigation::GetInstance().FindStraightPath(1);

	for (auto& index : _stopover)
	{
		index.first.y = _bossTransform->GetPosition().y;
		index.second.y = _bossTransform->GetPosition().y;
	}

	_nodeCount = 0;
}

bool KunrealEngine::Boss::MoveToTarget(DirectX::XMFLOAT3& startPos, DirectX::XMFLOAT3& targetPos, float speed)
{
	float moveSpeed = speed * TimeManager::GetInstance().GetDeltaTime();

	// ���������� �Ÿ� ���
	auto dist = ToolBox::GetDistance(startPos, targetPos);
	if (dist > 0.5f)
	{
		// �÷��̾� �̵�
		_boss->GetComponent<Animator>()->Play("Run", speed, true);

		DirectX::XMVECTOR currentPosVec = DirectX::XMLoadFloat3(&startPos);

		DirectX::XMVECTOR direction = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&targetPos), currentPosVec);

		direction = DirectX::XMVector3Normalize(direction);

		DirectX::XMVECTOR newPosition = DirectX::XMVectorAdd(currentPosVec, DirectX::XMVectorScale(direction, moveSpeed));
		_bossTransform->SetPosition(newPosition.m128_f32[0], _bossTransform->GetPosition().y, newPosition.m128_f32[2]);

		_prevPos = _bossTransform->GetPosition();

		return true;
	}

	else
	{
		return false;
	}
}

void KunrealEngine::Boss::UpdateMoveNode(DirectX::XMFLOAT3& targetPos)
{
	Navigation::GetInstance().SetSEpos(1, _bossTransform->GetPosition().x, _bossTransform->GetPosition().y, _bossTransform->GetPosition().z,
		targetPos.x, targetPos.y, targetPos.z);

	_stopover = Navigation::GetInstance().FindStraightPath(1);

	for (auto& index : _stopover)
	{
		index.first.y = _bossTransform->GetPosition().y;
		index.second.y = _bossTransform->GetPosition().y;
	}

	_nodeCount = 0;
}
