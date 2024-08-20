/// <summary>
/// ������ ��ӹ��� Ŭ����
/// ������ �⺻������ ���� ������ ��� ����
/// 
/// ����ȭ ������
/// </summary>

#pragma once
#include <vector>
#include "BossStruct.hpp"
#include "Coroutine.h"
#include "CommonHeader.h"

enum class BossStatus;
struct BossPattern;
struct BossBasicInfo;

namespace DirectX
{
	struct XMFLOAT3;
}

namespace KunrealEngine
{ 
	class GameObject;
	class BoxCollider;

	class _DECLSPEC Boss
	{
		friend class PlayerAbility;
	public:
		Boss();
		virtual ~Boss();

	public:
		void DebugTest();

	public:
		// �ݵ�� ȣ��
		void Initialize(GameObject* boss);
		// �ݵ�� ȣ��
		void Update();

	public:
		// ��� �𵨷� ������ �� �������� ����
		virtual void SetMesh() abstract;

	public:
		// ���� ����
		virtual void CreatePattern() abstract;

	public:
		// ��� �ؽ��ĸ� ���� �������� ����
		virtual void SetTexture();

	public:
		// ������ ������ ����
		virtual void SetBossTransform();

	public:
		const std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3> GetBossPosition();

		// ���¿� ���� �Լ���
	public:
		virtual void Enter();
		virtual void Idle();
		virtual void Chase();
		virtual void Staggred();
		virtual void OffStaggred();
		virtual void Dead();
		virtual void PatternReady();
		virtual void BasicAttack();
		virtual void CoreAttack();
		virtual void SpecialAttack();
		virtual void SpecialAttack2();
		virtual void PatternEnd();
		virtual void Win();

		virtual void InitializeEnterCameraMove();
		virtual bool EnterCameraMove();

	public:
		void PatternForceEnd();
		void ChangePhase(unsigned int phase);

	public:
		// ������ ���� �ֱ�
		void SetInfo(BossBasicInfo& info);

		// �⺻ ���� ���� ����� �ֱ�
		void SetBasicPatternList(std::vector<BossPattern*>* basicPatternList);

		// �ھ� ���� ���� ����� �ֱ�
		void SetCorePatternList(std::vector<BossPattern*>* corePatternList);

		void SetStartTime(float time);

		// ���� ������ �ٶ󺸴� ���� ���� ��������
		const DirectX::XMVECTOR GetDirection();

		// ���� ��ü ��ȯ
		Boss* GetBoss();

	public:
		// ���� ������ ���� ��������
		const BossStatus& GetStatus();
		
		// ���� �������� ������ ������ ��������
		const float GetDamage();

		BossPattern* GetNowPattern();

		BossPattern* GetNowPlayingPattern();

		BossPattern* GetNowSpecialPattern();

		BossBasicInfo& GetBossInfo();

		bool isDead();

	public:
		// ���� ��Ʈ ������ �ݶ��̴� ����
		virtual void SetBossCollider();

		void SetSubObject(bool tf);

		/// ���� ���� �����Ҷ� ������ ������.
	public:
		// �÷��̾� �ٶ󺸵��� ȸ����Ű��
		bool RotateToTarget(const DirectX::XMFLOAT3& targetPos);

		// Ư�� ������ŭ ���� ȸ����Ű��
		bool Rotate(float angle, float speed);

		// Ư�� ������ �ٷ� ȸ����Ű��
		void RotateByAngle(float angle);

		bool RotateClockWise(float rotateSpeed, bool isClockWise);

		bool RotateClockWise(float goalAngle, float rotateSpeed, bool isClockWise);

		// Ư�� ���������� �̵���Ű�� (raycast true�� ���𰡿� ������ �ű������ ã�ư�)
		bool Move(DirectX::XMFLOAT3& targetPos, float speed, bool roateToTarget, bool rayCast);

		std::function<bool()> CreateBackStepLogic(BossPattern* pattern, float moveSpeed, float distance);

		std::function<bool()> CreateBasicAttackLogic(BossPattern* pattern, GameObject* subObject, float activeColliderFrame);

		std::function<bool()> CreateBasicAttackLogic(BossPattern* pattern, GameObject* subObject, GameObject* subObject2, float activeColliderFrame);

		std::function<bool()> CreateBasicAttackLogic(BossPattern* pattern, const std::string& animName, GameObject* subObject, float activeColliderFrame);

		// �÷��̾ �ٶ󺸵��� �ڷ���Ʈ
		void TeleportToPlayer();

		// ������ ������ �ڷ���Ʈ
		bool Teleport(const DirectX::XMFLOAT3& targetPos, bool lookAtPlayer, float hideTime = 0.0f);

		// ������ ������ ���� �̵�
		void ForceMove(const DirectX::XMFLOAT3& targetPos);

		// ������ �÷��̾� ������ ���� ���
		float CalculateAngle(const DirectX::XMFLOAT3& bossPosition, const DirectX::XMFLOAT3& playerPosition);

		void SetSpecialPatternPlayPhase(unsigned int phase);

		void StopSpecialPattern();

	public:
		void ResetBoss();

	private:
		static bool CompareCorePattern(const BossPattern* pattern1, const BossPattern* pattern2);

		void SortCorePattern();

		void SetSubObjectScene();
		
	private:
		bool LookAtPlayer(float angle, float rotateSpeed);

		bool MoveToPlayer(DirectX::XMFLOAT3& startPos, DirectX::XMFLOAT3& targetPos, float speed);

		void UpdateMoveNode();

		void UpdateMoveNode(DirectX::XMFLOAT3& targetPos);

		bool MoveToTarget(DirectX::XMFLOAT3& startPos, DirectX::XMFLOAT3& targetPos, float speed);

		// �齺�� �̵�
		bool BackStep(float speed, float distance);

	protected:
		BossStatus _status;

		BossBasicInfo _info;

		GameObject* _boss;

		GameObject* _player;

		DirectX::XMFLOAT3 _bossOriginPos;

		std::vector<std::vector<BossPattern*>> _basicPattern;
		std::vector<BossPattern*> _corePattern;

		std::vector<BossPattern*> _corePatternOrigin;
		
		// ������ �����Ӱ� �����ϰ� ���� �ֱ⸶�� ����
		std::vector<BossPattern*> _specialPattern;

		BossPattern* _finalPattern;

		BossPattern* _nowTitlePattern;

		BossPattern* _nowPlayingPattern;

		BossPattern* _nowSpecialPattern;

		int _patternIndex;
		int _exPatternIndex;

		float _distance;

		Transform* _bossTransform;
		Transform* _playerTransform;

		bool _isCorePattern;

	private:
		float _startTime;

	private:
		bool _isStart;
		bool _isHit;
		bool _isRotateFinish;

		bool _isAngleCheck;
		float _rotAngle;
		float _sumRot;

		DirectX::XMFLOAT3 _prevRot;

	private:
		std::vector<std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3>> _stopover;
		int _nodeCount;

	private:
		DirectX::XMFLOAT3 _direction;

		DirectX::XMFLOAT3 _prevPos;
		DirectX::XMFLOAT3 _backStepPos;

	private:
		bool _isMoving;
		bool _isRotate;
		bool _backStepReady;

		bool _isHideFinish;

	private:
		float _specialPatternTimer;
		int _specialPatternIndex;
		bool _isSpecialPatternPlaying;
		bool _canPlaySpecialPattern;
		bool _specialPatternEndLogicPlay;

	private:
		unsigned int _specialPatternPlayPhase;
		
	private:
		unsigned int _goalPhase;

	private:
		bool _stopSpecialPattern;

	private:
		bool _isEnterInitialize;

	private:
		float _deathTimer;

		float _updateTimer;

		bool _isFinalPatternInit;

	private:
		// ���� ��ƼŬ

		GameObject* _particleTeleport;

		float _teltimer;

	private:
		Coroutine_Func(patternEnd)
		{
			Boss* boss = this;

			auto delay = _nowTitlePattern->_afterDelay;
			Waitforsecond(delay);

			// �ھ� �����̾��ٸ�
			if (boss->_isCorePattern)
			{
				// ������ �ھ� ������ ���Ϳ��� ����
				(boss->_corePattern).pop_back();

				boss->_isCorePattern = false;
			}

			boss->_status = BossStatus::IDLE;

			// �⺻ ������ ������ �� �ֵ��� �ʱ�ȭ
			boss->_patternIndex = -1;

			boss->_nowTitlePattern = nullptr;
		};

		Coroutine_Func(TeleportWithHide)
		{
			Boss* boss = this;

			boss->_boss->GetComponent<MeshRenderer>()->SetActive(false);
			boss->_boss->GetComponent<BoxCollider>()->SetActive(false);

			boss->_boss->GetComponent<BoxCollider>()->FixedUpdate();

			Waitforsecond(3.0f);

			boss->_isHideFinish = true;
		};

	};
}