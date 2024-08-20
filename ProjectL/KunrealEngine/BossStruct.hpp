#pragma once
#include <string>
#include <functional>
#include "SceneManager.h"
#include "Scene.h"
#include "Animator.h"
#include "GameObject.h"

// ������ ����
enum class BossStatus
{
	ENTER,				// ���� �� �÷��̾� ����ġ�� ��
	IDLE,				// ���� ���� ������ ����

	CHASE,				// �÷��̾� ����

	STAGGERED,			// ���� ����ȭ
	OFF_STAGGRED,
	DEAD,				// ���

	PATTERN_READY,		// ���� �غ�
	BASIC_ATTACK,		// �⺻ ����
	CORE_ATTACK,		// ��ų ���		// ��ų ������ ���� ���� ����
	PATTERN_END,		// ������ ���� ���

	WIN					// �÷��̾ ���� ���
};


// ������ �⺻���� ���� ����
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

	float _hp;				// ü��
	unsigned int _phase;	// ü�� ��� ������

	float _armor;			// ���ذ���
	float _damage;			// ������
	float _attackRange;		// �⺻ ���� ����

	float _moveSpeed;		// �̵��ӵ�
	float _rotationSpeed;	// ȸ�� �ӵ�
	float _baseAnimSpeed;	// �ִϸ��̼� �ӵ�

	float _staggeredGauge;	// ����ȭ ������

private:
	float _maxHp;				// �ִ� ü��
	float _maxStaggeredGauge;	// ����ȭ ������ �ִ�ġ
	unsigned int _maxPhase;			// ������ �����

	// ������ ������
};

struct BossPattern
{
	enum class eAttackState
	{
		eNone,
		eParalysis,		// ����
		ePush			// �ǰ�
	};

	enum class eColliderType
	{
		eNone,
		eBox,		// physx ��� �浹 ó��
		eCylinder,	// physx ��� �浹 ó��
		eCircleSafe,// Ư�� ���� �������� �Ÿ��� ���� �̳��� ��� �浹 ó��
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
		// ���� �÷���
		bool isLogicPlaying = true;
		if (_patternList[_index]->_logic != nullptr)
		{
			isLogicPlaying = _patternList[_index]->_logic();
		}
		else
		{
			isLogicPlaying = false;
		}

		// ������ �����ٸ�
		if (isLogicPlaying == false)
		{
			KunrealEngine::SceneManager::GetInstance().GetCurrentScene()->GetObjectWithTag("Boss")->GetComponent<KunrealEngine::Animator>()->Stop();
			// ������ ���� ���� ������Ʈ���� ��� ��
			for (const auto& object : _patternList[_index]->_subObject)
			{
				// ��� ������Ʈ�� ��
				object->SetTotalComponentState(false);
			}

			// ������ ��Ʈ���� �ʾҴٸ� �ڿ� �������� ��������
			if (_playNextPattern == false && _index > 0)
			{
				if (_patternList[_index]->_colliderOnCount > 0)
				{
					//if (_patternList[_index]->_playNextPattern)
						return false;
				}
			}

			//// ���� ������ �����϶�� ������ �ƴ϶��
			//if (_patternList[_index]->_playNextPattern == false)
			//{
			//	return false;
			//}

			// ���� ���� ������ ����
			_index++;

			if (_index > _patternList.size() - 1)
			{
				return false;
			}

			// �� ������ �ʱ�ȭ ���� ����
			_patternList[_index]->Initialize();

			// ������ ���� ���� ������Ʈ���� ��� ��
			for (const auto& object : _patternList[_index]->_subObject)
			{
				// ������Ʈ�� ��������, ���� ���ο��� �˾Ƽ� ó�� �ؾ� ��
				object->SetActive(true);
			}
		}

		// ��� ���� ������ ������ true ��ȯ
		return true;
	}

	bool SpecialPatternPlay()
	{
		// ���� �÷���
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

		// ������ �����ٸ�
		if (isLogicPlaying == false)
		{
			// ������ ���� ���� ������Ʈ���� ��� ��
			for (const auto& object : _patternList[_index]->_subObject)
			{
				// ��� ������Ʈ�� ��
				object->SetTotalComponentState(false);
			}

			// ���� ���� ������ ����
			_index++;

			if (_index > _patternList.size() - 1)
			{
				return false;
			}

			// �� ������ �ʱ�ȭ ���� ����
			_patternList[_index]->Initialize();

			// ������ ���� ���� ������Ʈ���� ��� ��
			for (const auto& object : _patternList[_index]->_subObject)
			{
				// ������Ʈ�� ��������, ���� ���ο��� �˾Ƽ� ó�� �ؾ� ��
				object->SetActive(true);
			}
		}

		// ��� ���� ������ ������ true ��ȯ����
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
	
	std::string _patternName;		// ���� �̸�

	std::string _animName;			// ���� �ִϸ��̼� �̸�
									
	float _damage;					// ������
									
	float _speed;					// ����, �ִϸ��̼� ���ǵ�
									
	float _range;					// ���� ����
									
	float _afterDelay;				// ���� �� ������ �ִ� �ð� (��)
									
	float _triggerHp;				// ������ �ߵ��� ������ hp

	float _coolDown;				// ���� ��Ÿ��

	float _rangeOffset;				// ����������� ������ �Ѿƿ� �������� ����, offset�� ���ϼ��� ����� �Ÿ����� chase

	bool _isActive;					// ���� Ȱ��ȭ ����

	//�������� �ǰ�����
	eAttackState _attackState;		// ���� ���� ����

	std::function<bool()> _logic;	// ���� ����

	std::function<void()> _initializeLogic;	// �� ���� �ʱ�ȭ������� �͵��� ��Ƴ���

	unsigned int _maxColliderOnCount; // ���� �� �ݶ��̴��� ������ Ƚ���� �ִ�ġ (�⺻ 1)

	unsigned int _colliderOnCount; // ���� �� �ݶ��̴��� ������ Ƚ���� ������ �����ϴ� �κ� (�⺻ 1)

	std::vector<KunrealEngine::GameObject*> _subObject;

	std::vector<bool> _isColliderActive;

	std::vector<bool> _isColliderHit;

	bool _isRemainMesh;		// �ǰ��� ������ �޽��� �����ִ°�

	bool _playNextPattern;

	std::vector<BossPattern*> _patternList;

	unsigned int _index;

	bool _skipChase;

	bool _skipMove;
	
	bool _withEgo;

	eColliderType _colliderType;
};