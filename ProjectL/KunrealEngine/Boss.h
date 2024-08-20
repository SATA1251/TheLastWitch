/// <summary>
/// 보스가 상속받을 클래스
/// 보스가 기본적으로 지닐 정보를 담고 있음
/// 
/// 윤종화 정진수
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
		// 반드시 호출
		void Initialize(GameObject* boss);
		// 반드시 호출
		void Update();

	public:
		// 어떠한 모델로 렌더링 할 것인지를 지정
		virtual void SetMesh() abstract;

	public:
		// 패턴 생성
		virtual void CreatePattern() abstract;

	public:
		// 어떠한 텍스쳐를 입힐 것인지를 지정
		virtual void SetTexture();

	public:
		// 보스의 포지션 지정
		virtual void SetBossTransform();

	public:
		const std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3> GetBossPosition();

		// 상태에 따른 함수들
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
		// 보스의 정보 넣기
		void SetInfo(BossBasicInfo& info);

		// 기본 패턴 만든 목록을 넣기
		void SetBasicPatternList(std::vector<BossPattern*>* basicPatternList);

		// 코어 패턴 만든 목록을 넣기
		void SetCorePatternList(std::vector<BossPattern*>* corePatternList);

		void SetStartTime(float time);

		// 현재 보스가 바라보는 방향 벡터 가져오기
		const DirectX::XMVECTOR GetDirection();

		// 보스 객체 반환
		Boss* GetBoss();

	public:
		// 현재 보스의 상태 가져오기
		const BossStatus& GetStatus();
		
		// 현재 진행중인 패턴의 데미지 가져오기
		const float GetDamage();

		BossPattern* GetNowPattern();

		BossPattern* GetNowPlayingPattern();

		BossPattern* GetNowSpecialPattern();

		BossBasicInfo& GetBossInfo();

		bool isDead();

	public:
		// 보스 히트 판정용 콜라이더 생성
		virtual void SetBossCollider();

		void SetSubObject(bool tf);

		/// 보스 패턴 구현할때 가져다 쓰세요.
	public:
		// 플레이어 바라보도록 회전시키기
		bool RotateToTarget(const DirectX::XMFLOAT3& targetPos);

		// 특정 각도만큼 점점 회전시키기
		bool Rotate(float angle, float speed);

		// 특정 각도로 바로 회전시키기
		void RotateByAngle(float angle);

		bool RotateClockWise(float rotateSpeed, bool isClockWise);

		bool RotateClockWise(float goalAngle, float rotateSpeed, bool isClockWise);

		// 특정 포지션으로 이동시키기 (raycast true시 무언가에 막히면 거기까지만 찾아감)
		bool Move(DirectX::XMFLOAT3& targetPos, float speed, bool roateToTarget, bool rayCast);

		std::function<bool()> CreateBackStepLogic(BossPattern* pattern, float moveSpeed, float distance);

		std::function<bool()> CreateBasicAttackLogic(BossPattern* pattern, GameObject* subObject, float activeColliderFrame);

		std::function<bool()> CreateBasicAttackLogic(BossPattern* pattern, GameObject* subObject, GameObject* subObject2, float activeColliderFrame);

		std::function<bool()> CreateBasicAttackLogic(BossPattern* pattern, const std::string& animName, GameObject* subObject, float activeColliderFrame);

		// 플레이어를 바라보도록 텔레포트
		void TeleportToPlayer();

		// 지정한 곳으로 텔레포트
		bool Teleport(const DirectX::XMFLOAT3& targetPos, bool lookAtPlayer, float hideTime = 0.0f);

		// 지정한 곳으로 강제 이동
		void ForceMove(const DirectX::XMFLOAT3& targetPos);

		// 보스와 플레이어 사이의 각도 계산
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

		// 백스탭 이동
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
		
		// 보스의 움직임과 무관하게 일정 주기마다 실행
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
		// 텔포 파티클

		GameObject* _particleTeleport;

		float _teltimer;

	private:
		Coroutine_Func(patternEnd)
		{
			Boss* boss = this;

			auto delay = _nowTitlePattern->_afterDelay;
			Waitforsecond(delay);

			// 코어 패턴이었다면
			if (boss->_isCorePattern)
			{
				// 실행한 코어 패턴을 백터에서 제거
				(boss->_corePattern).pop_back();

				boss->_isCorePattern = false;
			}

			boss->_status = BossStatus::IDLE;

			// 기본 패턴을 실행할 수 있도록 초기화
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