#pragma once
#include <string>
#include <functional>
#include "SceneManager.h"
#include "Scene.h"
#include "Animator.h"
#include "GameObject.h"

// 보스의 상태
enum class BossStatus
{
	ENTER,				// 조우 시 플레이어 마주치기 전
	IDLE,				// 다음 공격 패턴을 정함

	CHASE,				// 플레이어 추적

	STAGGERED,			// 보스 무력화
	OFF_STAGGRED,
	DEAD,				// 사망

	PATTERN_READY,		// 공격 준비
	BASIC_ATTACK,		// 기본 공격
	CORE_ATTACK,		// 스킬 사용		// 스킬 구성에 따라 나뉠 예정
	PATTERN_END,		// 패턴이 끝난 경우

	WIN					// 플레이어가 죽은 경우
};


// 보스의 기본적인 스탯 정보
struct BossBasicInfo
{
	BossBasicInfo()
		: _hp(0.0f), _phase(1), _armor(0.0f), _damage(0.0f), _attackRange(0.0f), _moveSpeed(40.0f), _rotationSpeed(120.0f), _baseAnimSpeed(30.0f),
		_staggeredGauge(0.0f), _maxHp(0.0f), _maxStaggeredGauge(0.0f), _maxPhase(0)
	{};

	BossBasicInfo& SetHp(float hp) { _maxHp = hp; _hp = hp;   return *this; };
	BossBasicInfo& SetPhase(unsigned int phase) { _phase = phase;  return *this; };
	BossBasicInfo& SetMaxPhase(unsigned int phase) { _maxPhase = phase;  return *this; };
	BossBasicInfo& SetArmor(float armor) { _armor = armor;  return *this; };
	BossBasicInfo& SetDamage(float damage) { _damage = damage;  return *this; };
	BossBasicInfo& SetAttackRange(float attackRange) { _attackRange = attackRange;  return *this; };
	BossBasicInfo& SetMoveSpeed(float moveSpeed) { _moveSpeed = moveSpeed;  return *this; };
	BossBasicInfo& SetRotateSpeed(float rotationSpeed) { _rotationSpeed = rotationSpeed;  return *this; };
	BossBasicInfo& SetBaseAnimationSpeed(float animSpeed) { _baseAnimSpeed = animSpeed;  return *this; };
	BossBasicInfo& SetsStaggeredGauge(float staggeredGauge) { _maxStaggeredGauge = staggeredGauge; _staggeredGauge = staggeredGauge;  return *this; };

	float GetMaxHP() { return _maxHp; };
	unsigned int GetMaxPhase() { return _maxPhase; };
	float GetMaxStaggeredGauage() { return _maxStaggeredGauge; };

	float _hp;				// 체력
	unsigned int _phase;	// 체력 비례 페이즈

	float _armor;			// 피해감소
	float _damage;			// 데미지
	float _attackRange;		// 기본 공격 범위

	float _moveSpeed;		// 이동속도
	float _rotationSpeed;	// 회전 속도
	float _baseAnimSpeed;	// 애니메이션 속도

	float _staggeredGauge;	// 무력화 게이지

private:
	float _maxHp;				// 최대 체력
	float _maxStaggeredGauge;	// 무력화 게이지 최대치
	unsigned int _maxPhase;			// 페이즈 몇개인지

	// 데미지 감소율
};

struct BossPattern
{
	enum class eAttackState
	{
		eNone,
		eParalysis,		// 경직
		ePush			// 피격
	};

	enum class eColliderType
	{
		eNone,
		eBox,		// physx 기반 충돌 처리
		eCylinder,	// physx 기반 충돌 처리
		eCircleSafe,// 특정 원의 중점과의 거리가 일정 이내일 경우 충돌 처리
		eDonut,
		eSwordMultiple,
		eSwordDirection,
		eSwordColliderWithMeshCollider,
		eSwordColliderWithCylinderCollider
	};


	BossPattern()
		: _patternName(""), _animName(""), _damage(0.0f), _speed(0.0f), _range(0.0f), _afterDelay(0.0f), _triggerHp(0.0f),
		_coolDown(0.0f), _rangeOffset(5.0f),
		_isActive(true), _maxColliderOnCount(1), _colliderOnCount(1), _subObject(),
		_logic(), _initializeLogic(nullptr), _attackState(eAttackState::eNone), _isRemainMesh(false), _playNextPattern(true), _skipChase(false), _skipMove(false),
		_index(0), _withEgo(false), _colliderType(eColliderType::eNone)
	{
		_patternList.emplace_back(this);
	}

	~BossPattern()
	{
		for (auto object : _subObject)
		{
			KunrealEngine::SceneManager::GetInstance().GetCurrentScene()->DeleteGameObject(object);
		}
	}
	bool Play() 
	{ 
		// 로직 플레이
		bool isLogicPlaying = true;
		if (_patternList[_index]->_logic != nullptr)
		{
			isLogicPlaying = _patternList[_index]->_logic();
		}
		else
		{
			isLogicPlaying = false;
		}

		// 로직이 끝난다면
		if (isLogicPlaying == false)
		{
			KunrealEngine::SceneManager::GetInstance().GetCurrentScene()->GetObjectWithTag("Boss")->GetComponent<KunrealEngine::Animator>()->Stop();
			// 패턴이 지닌 하위 오브젝트들을 모두 끔
			for (const auto& object : _patternList[_index]->_subObject)
			{
				// 모든 컴포넌트는 끔
				object->SetTotalComponentState(false);
			}

			// 패턴이 히트되지 않았다면 뒤에 패턴으로 가지않음
			if (_playNextPattern == false && _index > 0)
			{
				if (_patternList[_index]->_colliderOnCount > 0)
				{
					//if (_patternList[_index]->_playNextPattern)
						return false;
				}
			}

			//// 다음 패턴을 실행하라는 패턴이 아니라면
			//if (_patternList[_index]->_playNextPattern == false)
			//{
			//	return false;
			//}

			// 다음 패턴 로직을 실행
			_index++;

			if (_index > _patternList.size() - 1)
			{
				return false;
			}

			// 각 패턴의 초기화 로직 실행
			_patternList[_index]->Initialize();

			// 패턴이 지닌 하위 오브젝트들을 모두 켬
			for (const auto& object : _patternList[_index]->_subObject)
			{
				// 컴포넌트는 꺼져있음, 로직 내부에서 알아서 처리 해야 함
				object->SetActive(true);
			}
		}

		// 모든 패턴 실행이 끝나면 true 반환
		return true;
	}

	bool SpecialPatternPlay()
	{
		// 로직 플레이
		if (_index >= _patternList.size())
		{
			return false;
		}

		bool isLogicPlaying = true;
		if (_patternList[_index]->_logic != nullptr)
		{
			isLogicPlaying = _patternList[_index]->_logic();
		}
		else
		{
			isLogicPlaying = false;
		}

		// 로직이 끝난다면
		if (isLogicPlaying == false)
		{
			// 패턴이 지닌 하위 오브젝트들을 모두 끔
			for (const auto& object : _patternList[_index]->_subObject)
			{
				// 모든 컴포넌트는 끔
				object->SetTotalComponentState(false);
			}

			// 다음 패턴 로직을 실행
			_index++;

			if (_index > _patternList.size() - 1)
			{
				return false;
			}

			// 각 패턴의 초기화 로직 실행
			_patternList[_index]->Initialize();

			// 패턴이 지닌 하위 오브젝트들을 모두 켬
			for (const auto& object : _patternList[_index]->_subObject)
			{
				// 컴포넌트는 꺼져있음, 로직 내부에서 알아서 처리 해야 함
				object->SetActive(true);
			}
		}

		// 모든 패턴 실행이 끝나면 true 반환해줌
		return true;
	}


	void Initialize() { _colliderOnCount = _maxColliderOnCount;  _index = 0;  for (int i = 0; i < _isColliderActive.size(); i++) { _isColliderActive[i] = false; _isColliderHit[i] = false; }   if (_initializeLogic != nullptr) _initializeLogic();}
	BossPattern& SetPatternName(const std::string& patterName) { _patternName = patterName; return *this; };
	BossPattern& SetAnimName(const std::string& animName) { _animName = animName; return *this; };
	BossPattern& SetDamage(float damage) { _damage = damage; return *this; };
	BossPattern& SetSpeed(float speed) { _speed = speed; return *this; };
	BossPattern& SetRange(float range) { _range = range; return *this; };
	BossPattern& SetAfterDelay(float afterDelay) { _afterDelay = afterDelay; return *this; };
	BossPattern& SetTriggerHp(float hp) { _triggerHp = hp; return *this; };
	BossPattern& SetCoolDown(float coolDown) { _triggerHp = coolDown; return *this; };
	BossPattern& SetRangeOffset(float rangeOffset) { _rangeOffset = rangeOffset; return *this; };
	BossPattern& SetActive(bool isActive) { _triggerHp = isActive; return *this; };
	BossPattern& SetMaxColliderCount(unsigned int count) { _maxColliderOnCount = count; _colliderOnCount = count; return *this; };
	BossPattern& SetLogic(std::function<bool()> logic, bool isRemainMesh = true) { _logic = logic; _isRemainMesh = isRemainMesh; return *this; };
	BossPattern& SetInitializeLogic(std::function<void()> initialize) { _initializeLogic = initialize; return *this; };
	BossPattern& SetAttackState(eAttackState attackState) { _attackState = attackState; return *this; };
	BossPattern& SetNextPatternForcePlay(bool tf) { _playNextPattern = tf; return *this; };
	BossPattern& SetPattern(BossPattern* pattern) { _patternList.emplace_back(pattern); return *this; };
	BossPattern& SetSkipChase(bool tf) { _skipChase = tf; return *this; };
	BossPattern& SetSkipMove(bool tf) { _skipMove = tf; return *this; };
	BossPattern& DeleteSubObject(KunrealEngine::GameObject* object) { auto it = std::find(_subObject.begin(), _subObject.end(), object); if (it != _subObject.end()) { _subObject.erase(it); } return *this; };
	BossPattern& SetWithEgo(bool tf) { _withEgo = tf; return *this; }
	BossPattern& SetColliderType(eColliderType type) { _colliderType = type; return *this; };
	BossPattern& SetSubObject(KunrealEngine::GameObject* object)
	{
		for (int i = 0; i < _subObject.size(); i++)
		{
			if (_subObject[i] == object)
			{
				return *this;
			}
		}
		_subObject.emplace_back(object);	
		object->SetTag("BossSub");
		object->SetTotalComponentState(false);  

		_isColliderActive.emplace_back(false);
		_isColliderHit.emplace_back(false);

		return *this;
	};

	unsigned int GetSubObjectIndex(KunrealEngine::GameObject* object)
	{
		for (int i = 0; i < _subObject.size(); i++)
		{
			if (_subObject[i] == object)
			{
				return i;
			}
		}

		return 181818;
	};
	
	std::string _patternName;		// 패턴 이름

	std::string _animName;			// 패턴 애니메이션 이름
									
	float _damage;					// 데미지
									
	float _speed;					// 패턴, 애니메이션 스피드
									
	float _range;					// 패턴 범위
									
	float _afterDelay;				// 패턴 후 가만히 있는 시간 (초)
									
	float _triggerHp;				// 패턴이 발동될 조건의 hp

	float _coolDown;				// 패턴 쿨타임

	float _rangeOffset;				// 어느정도까지 보스가 쫓아올 것인지를 지정, offset을 줄일수록 가까운 거리까지 chase

	bool _isActive;					// 패턴 활성화 여부

	//경직인지 피격인지
	eAttackState _attackState;		// 패턴 상태 여부

	std::function<bool()> _logic;	// 패턴 로직

	std::function<void()> _initializeLogic;	// 매 패턴 초기화해줘야할 것들을 담아놓음

	unsigned int _maxColliderOnCount; // 패턴 중 콜라이더가 켜지는 횟수의 최대치 (기본 1)

	unsigned int _colliderOnCount; // 패턴 중 콜라이더가 켜지는 횟수를 실제로 조정하는 부분 (기본 1)

	std::vector<KunrealEngine::GameObject*> _subObject;

	std::vector<bool> _isColliderActive;

	std::vector<bool> _isColliderHit;

	bool _isRemainMesh;		// 피격이 끝나도 메쉬가 남아있는가

	bool _playNextPattern;

	std::vector<BossPattern*> _patternList;

	unsigned int _index;

	bool _skipChase;

	bool _skipMove;
	
	bool _withEgo;

	eColliderType _colliderType;
};