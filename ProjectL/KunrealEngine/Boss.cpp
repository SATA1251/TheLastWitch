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
	// 애니메이션 확인용
	GRAPHICS->DrawDebugText(500, 0, 50, "%s", _boss->GetComponent<Animator>()->GetNowAnimationName().c_str());
	GRAPHICS->DrawDebugText(800, 0, 50, "%f", this->_info._hp);

	// 상태 확인용
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
	// 디버그 확인용
	DebugTest();

	// 캐릭터가 이동시
	if (InputSystem::GetInstance()->MouseButtonUp(1) || InputSystem::GetInstance()->KeyDown(KEY::SPACE))
	{
		// 이동, 회전 관련된 것들 초기화해줌
		_isRotateFinish = false;
		_isMoving = false;
		_nodeCount = 0;

		_isAngleCheck = false;
	}

	_updateTimer += TimeManager::GetInstance().GetDeltaTime();

	if (_updateTimer >= 2.0f)
	{
		// 이동, 회전 관련된 것들 초기화해줌
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

		// 시작하면 
		if (_isStart)
		{
			// 보스 등장 애니메이션 실행
			_boss->GetComponent<Animator>()->Play("Emergence", _info._baseAnimSpeed, false);

			// 보스 등장 애니메이션 실행이 끝난다면
			if (_boss->GetComponent<Animator>()->GetIsAnimationPlaying() == false)
			{
				// 다음 애니메이션 실행을 위해 애니메이션 프레임을 0으로 되돌림
				_boss->GetComponent<Animator>()->Stop();
				// IDLE로 상태 변경

				_status = BossStatus::IDLE;
			}

			_canPlaySpecialPattern = true;
		}
	}
}

void KunrealEngine::Boss::Idle()
{
	_boss->GetComponent<Animator>()->Play("Idle", _info._baseAnimSpeed, true);

	// 핵심 패턴이 구현되어있다면
	if (!(_corePattern).empty())
	{
		// 핵심 패턴은 back에서부터 hp가 높은 순서로 되어있기에
		// 보스의 체력이 핵심 패턴의 발동 체력 이하가 된다면
		if (_info._hp <= (_corePattern).back()->_triggerHp)
		{
			// 코어패턴인지 여부 확인
			_isCorePattern = true;

			_nowTitlePattern = _corePattern.back();

			// 패턴 준비 상태로 변경
			_status = BossStatus::PATTERN_READY;

			// 함수 종료
			return;
		}
	}

	// 핵심 패턴이 아닌 기본 공격일 경우에 다음 기본 패턴을 정함
	if (_patternIndex == -1 && !_isCorePattern)
	{
		// 랜덤 패턴을 위한 랜덤 인덱스를 가져옴
		_patternIndex = ToolBox::GetRandomNum(0, _basicPattern[_info._phase-1].size() - 1);

		if (_basicPattern[_info._phase-1].size() > 1)
		{
			while (_patternIndex == _exPatternIndex)
			{
				// 랜덤 패턴을 위한 랜덤 인덱스를 가져옴
				_patternIndex = ToolBox::GetRandomNum(0, _basicPattern[_info._phase-1].size() - 1);
			}

			// 가져온 랜덤 패턴이 활성화되어있지 않다면
			while ((_basicPattern[_info._phase-1])[_patternIndex]->_isActive == false)
			{
				// 가져온 랜덤 패턴이 활성화 되어 있을때까지 다시 랜덤 인덱스를 추출
				_patternIndex = ToolBox::GetRandomNum(0, _basicPattern[_info._phase-1].size() - 1);
			}
		}
	}

	_nowTitlePattern = _basicPattern[_info._phase-1][_patternIndex];

	// Chase로 상태 변환
	_status = BossStatus::CHASE;

	if (_nowTitlePattern->_skipChase)
	{
		_status = BossStatus::PATTERN_READY;
	}
}

void KunrealEngine::Boss::Chase()
{
	// 현재 실행 중인 애니메이션이 Idle 이라면
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

	// 패턴의 사거리를 받아옴
	if (!_isRotateFinish)
	{
		// 최초 진입시 각도 체크가 되어있지 않다면
		if (!_isAngleCheck)
		{
			// 각도를 측정
			_rotAngle = ToolBox::GetAngleWithDirection(bossPosition, playerPosition, _bossTransform->GetRotation().y);

			if (isnan(_rotAngle))
			{
				_rotAngle = 0.0f;
			}

			// 현재 각도를 기록
			_prevRot = _bossTransform->GetRotation();

			// 각도 누적용
			_sumRot = 0.0f;

			// 두번째 진입시부턴 안들어오게 하도록
			_isAngleCheck = true;
		}

		// 보스가 플레이어를 바라보게 함
		_isRotateFinish = LookAtPlayer(_rotAngle, _info._rotationSpeed);
	}

	// 보스가 플레이어를 바라보게 되었을 때
	if (_isRotateFinish)
	{
		if (_nowTitlePattern->_skipMove)
		{
			_status = BossStatus::PATTERN_READY;;
		}

		// 보스와 플레이어 사이의 거리를 측정
		if (!_isMoving)
		{
			_distance = ToolBox::GetDistance(bossPosition, playerPosition);
		}

		// 패턴 사거리보다 플레이어와의 거리가 가까울 경우 공격 시행
		if (_distance <= patternRange + _nowTitlePattern->_rangeOffset)
		{
			// 패턴 준비 상태로 변경
			_status = BossStatus::PATTERN_READY;

			return;
		}

		// 패턴 사거리보다 플레이어와의 거리가 멀 경우
		else
		{
			// 패턴 사거리보다 플레이어의 거리가 멀지만 일정 거리 이내일 경우 걸어서 이동
			if (_distance - patternRange <= 50.0f)
			{
				// 보스와 플레이어의 갈 수 있는 길을 노드로 계산
				if (_isMoving == false)
				{
					UpdateMoveNode();

					// 노드 계산이 끝났으므로
					_isMoving = true;

					_prevPos = _stopover[0].first;
				}

				// 계산한 노드 백터가 존재한다면
				if (_stopover.size() > 0)
				{
					// 플레이어를 향하여 이동
 					auto isChasing = MoveToPlayer(_prevPos, _stopover[_nodeCount].second, _info._moveSpeed);

					// 첫 노드만큼 쫓아갔다면 다음 노드를 기반으로 이동
					if (!isChasing)
					{
						// 다음 노드 방향 벡터에 맞게 회전
						if (_nodeCount + 1 < _stopover.size())
						{
							_isRotate = false;
							_isRotate = RotateToTarget(_stopover[_nodeCount + 1].second);
						}
						else
						{
							_nodeCount++;
						}

						// 회전이 끝났다면 다음 노드로 이동
						if (_isRotate)
						{
							_nodeCount++;
						}
					}
				}

				// 모든 노드를 읽어 보스가 플레이어를 다 쫓아갔다면
				if (_nodeCount >= _stopover.size())
				{
					// 패턴 준비 상태로 변경
					_status = BossStatus::PATTERN_READY;

					return;
				}
			}

			// 패턴 사거리보다 플레이의 거리가 멀고 일정 거리를 벗어날 경우 텔레포트로 이동
			else
			{
				TeleportToPlayer();

				// 패턴 준비 상태로 변경
				_status = BossStatus::PATTERN_READY;

				return;
			}
		}
	}
}

void KunrealEngine::Boss::Staggred()
{
	// 무력화 애니메이션 실행
	_boss->GetComponent<Animator>()->Play("Staggred", _info._baseAnimSpeed, false);

	// 무력화 애니메이션이 끝났다면
	if (_boss->GetComponent<Animator>()->GetIsAnimationPlaying() == false)
	{
		_boss->GetComponent<Animator>()->Stop();
		_status = BossStatus::OFF_STAGGRED;
	}
}

void KunrealEngine::Boss::OffStaggred()
{
	// 무력화 종료 애니메이션 실행
	_boss->GetComponent<Animator>()->Play("OffStaggred", _info._baseAnimSpeed, false);

	// 무력화 종료 애니메이션이 끝났다면
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
	
	// 보스가 죽을때 애니메이션 실행
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
	// 길찾기 끝났으므로 패턴에서의 길찾기를 위해 초기화
	_nodeCount = 0;

	// 현재 실행 중인 Idle 애니메이션을 종료
	_boss->GetComponent<Animator>()->Stop();

	// 패턴 내의 첫 패턴이 지닌 하위 오브젝트들을 모두 켬
	for (const auto& object : _nowTitlePattern->_subObject)
	{
		// 모든 컴포넌트는 꺼져있음, 로직 내부에서 알아서 처리 해야 함
		object->SetActive(true);

		// 모든 컴포넌트는 끔
		object->SetTotalComponentState(false);
	}

	// 패턴내의 첫 패턴 초기화해줘야할 것들 초기화
	_nowTitlePattern->Initialize();

	// 코어 패턴일 경우
	if (_isCorePattern)
	{
		// Core Attack으로 상태 변경
		_status = BossStatus::CORE_ATTACK;
	}
	else
	{
		// Basic Attack으로 상태 변경
		_status = BossStatus::BASIC_ATTACK;
	}

	_isAngleCheck = false;
}

void KunrealEngine::Boss::BasicAttack()
{
	auto index = _basicPattern[_info._phase-1][_patternIndex]->_index;
	_nowPlayingPattern = _basicPattern[_info._phase-1][_patternIndex]->_patternList[index];

	// 패턴을 실행
	auto isPatternPlaying = _nowTitlePattern->Play();

	// 패턴 실행이 끝났다면
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

	// 패턴을 실행
	auto isPatternPlaying = _nowTitlePattern->Play();

	// 패턴 실행이 끝났다면
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
			// 모든 컴포넌트는 꺼져있음, 로직 내부에서 알아서 처리 해야 함
			object->SetActive(true);

			// 모든 컴포넌트는 끔
			object->SetTotalComponentState(false);
		}

	}

	auto isPlaying = _specialPattern[_specialPatternIndex]->SpecialPatternPlay();

	if (isPlaying)
	{
		auto index = _specialPattern[_specialPatternIndex]->_index;
		_nowSpecialPattern = _specialPattern[_specialPatternIndex]->_patternList[index];
	}

	// 패턴 실행이 끝났다면
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
	// 다음 패턴을 위해 초기화
	_isAngleCheck = false;

	_isHideFinish = false;
	_isMoving = false;
	_isRotateFinish = false;

	// 이전 패턴 인덱스 저장
	_exPatternIndex = _patternIndex;

	// Idle 애니메이션 실행
	_boss->GetComponent<Animator>()->Play("Idle", _info._baseAnimSpeed, true);

	// 모든 하위 오브젝트를 끔
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

	// 1초동안 대기한 뒤 Idle 상태로 전환
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
	// 다음 패턴을 위해 초기화
	_isAngleCheck = false;

	_isHideFinish = false;
	_isMoving = false;
	_isRotateFinish = false;

	// 이전 패턴 인덱스 저장
	_exPatternIndex = _patternIndex;

	// Idle 애니메이션 실행
	_boss->GetComponent<Animator>()->Stop();

	// 모든 하위 오브젝트를 끔
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
	// 보스와 플레이어 사이의 각도를 계산
	auto angle = ToolBox::GetAngle(bossPosition, playerPosition);

	auto bossAngle = _boss->GetComponent<Transform>()->GetRotation().y;

	// 현재보스의 각도와 계산한 플레이어와의 각도간의 차이가 180 이하일경우
	// angle만큼 그대로 움직이면 됨 (이렇게 안하면 90, 270도 중 270도로 돌아버려 비효율적)
	if (std::abs(bossAngle - angle) < 180.0f)
	{
		return angle;
	}
	// 각도간의 차이가 180도를 넘을경우
	else
	{
		// angle이 음수일 경우
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

	// 목적지까지 거리 계산
	auto dist = ToolBox::GetDistance(startPos, targetPos);
	if (dist > 0.5f)
	{
		// 플레이어 이동
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

	// 보스와 플레이어 까지의 경로 계산
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

	// 여기에 플레이어의 방향 벡터 * 일정 거리를 곱해 플레이어와 살짝 떨어지도록 해야 함
	// 플레이어의 방향 벡터를 가져와야함

	auto target = _stopover.back().second;

	_prevPos = _bossTransform->GetPosition();

	_bossTransform->SetPosition(target.x, target.y, target.z);

	// 플레이어를 바라보도록 
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

		// 사라졌을때의 이펙트

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

		// 타겟 포지션으로의 경로 계산
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

		// 플레이어를 바라보도록 
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
	// 회전 속도
	float speed = TimeManager::GetInstance().GetDeltaTime() * rotateSpeed;

	// 에러 범위
	float errorRange = speed * 2.0f;

	_sumRot += speed;

	// 현재 각도가 목표로 하는 각도보다 작을 경우
	if (angle > 0)
	{
		if (_sumRot < angle)
		{
			// 회전 속도만큼 회전
			_bossTransform->SetRotation(_prevRot.x, _prevRot.y + _sumRot, _prevRot.z);
		}
	}
	else
	{
		if (_sumRot < std::abs(angle))
		{
			// 회전 속도만큼 회전
			_bossTransform->SetRotation(_prevRot.x, _prevRot.y - _sumRot, _prevRot.z);
		}
	}

	if (_sumRot > std::abs(angle))
	{
		// 현재 실행 중인 애니메이션이 Idle 이라면
		if (_boss->GetComponent<Animator>()->GetNowAnimationName() == "Idle")
		{
			// Idle 애니메이션을 멈추고
			_boss->GetComponent<Animator>()->Stop();
		}

		// 회전이 완료되었다고 반환
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
	// 타겟 방향으로 회전을 원할때
	bool finshRoate = true;
	if (roateToTarget == true)
	{
		finshRoate = RotateToTarget(targetPos);
	}

	// 회전이 끝난다면
	if (finshRoate == true)
	{
		// 이 함수에 처음 들어와서 한번도 움직여지지 않았다면 길찾기 (한번만 들어오도록)
		if (_isMoving == false)
		{
			// 앞에 물체가 있을때 거기까지만 이동시키고 싶다면
			if (rayCast)
			{
				// 가로막힌 물체의 포지션을 찾고
				Navigation::GetInstance().SetSEpos(1, _bossTransform->GetPosition().x, _bossTransform->GetPosition().y, _bossTransform->GetPosition().z,
					targetPos.x, _bossTransform->GetPosition().y, targetPos.z);

				auto targetPosition = Navigation::GetInstance().FindRaycastPath(1);

				// 그 물체의 포지션을 기반으로 네비메쉬 길찾기
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
			// 그냥 타겟 포지션까지 네비메쉬로 길찾기
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

			// 이제 움직이기 시작
			_isMoving = true;
		}

		// 길이 남아있다면
		if (_stopover.size() > 0)
		{
			// 타겟을 향하여 이동
			//auto trs = _bossTransform->GetPosition();
			auto isChasing = MoveToTarget(_prevPos, _stopover[_nodeCount].second, speed);

			// 노드만큼 쫓아갔다면
			if (!isChasing)
			{
				// 다음 노드를 기반으로 이동
				_nodeCount++;
			}

			// 모든 노드를 읽어 보스가 타겟에 도착했다면
			if (_nodeCount >= _stopover.size())
			{
				// 다음 로직을 위해 초기화
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

		// 다음 패턴을 실행시킴
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


		// 일정 프레임이 넘어가면 데미지 체크용 콜라이더를 키기
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

		// 1타를 맞았다면
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


		// 일정 프레임이 넘어가면 데미지 체크용 콜라이더를 키기
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

		// 1타를 맞았다면
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

		// 일정 프레임이 넘어가면 데미지 체크용 콜라이더를 키기
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

		// 1타를 맞았다면
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

				// 오브젝트에 대한 비활성화 시키기
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

				// 오브젝트에 대한 비활성화 시키기
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

	// 패턴의 사거리를 받아옴
	if (!_isRotate)
	{
		// 보스가 플레이어를 바라보게 함
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

	// 목적지까지 거리 계산
	auto dist = ToolBox::GetDistance(startPos, targetPos);
	if (dist > 0.5f)
	{
		// 플레이어 이동
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
