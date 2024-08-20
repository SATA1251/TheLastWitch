#pragma once

#include <DirectXMath.h>
#include "CommonHeader.h"
#include "Component.h"
#include "Coroutine.h"

namespace KunrealEngine
{
	class Transform;

	class _DECLSPEC Player : public Component
	{
		friend class PlayerMove;
		friend class PlayerAbility;
	public:
		// 플레이어의 상태
		enum class Status
		{
			IDLE,				// 평상시
			WALK,				// 이동
			DASH,				// 대시
			ABILITY,			// 스킬 사용		// 스킬 구성에 따라 나뉠 예정
			STAGGERED,			// 플레이어가 무력화 되었을 때		// 상태이상 등
			PARALYSIS,			// 경직
			SWEEP,				// 날아가는중
			DEAD,				// 사망
			BEFORESTART,		// 게임 시작 전 누워있는 상태
			CINEMATIC,			// 특수 상태
		};

		// 플레이어의 스탯 정보
		struct PlayerInfo
		{
			float _hp;				// 체력
			float _maxhp;			// 최대체력
			float _stamina;			// 기력

			float _moveSpeed;		// 이동 속도
			float _dashSpeed;		// 대시 속도
			float _dashRange;		// 대시 거리
			float _dashCooldown;	// 대시 쿨타임

			float _spellPower;		// 스킬 사용 시 데미지 배율
			float _damageReduce;	// 피해 감소

			float _speedScale;		// 전체적인 속도에 영향

			PlayerInfo(float hp, float maxhp, float stamina, float moveSpeed, float dashSpeed, float dashRange, float dashCooldown, float spellpower, float damageReduce, float speedScale)
				:_hp(hp), _maxhp(maxhp), _stamina(stamina), _moveSpeed(moveSpeed), _dashSpeed(dashSpeed), _dashRange(dashRange), _dashCooldown(dashCooldown), _spellPower(spellpower), _damageReduce(damageReduce), _speedScale(speedScale)
			{}
		};

	public:
		Player();
		~Player();

		void Initialize() override;
		void Release() override;

		void FixedUpdate() override;
		void Update() override;
		void LateUpdate() override;

		void OnTriggerEnter() override;
		void OnTriggerStay() override;
		void OnTriggerExit() override;

		void SetActive(bool active) override;

		PlayerInfo _playerInfo;					// 플레이어 기본 정보 스탯 등
	private:
		Transform* _transform;					// 플레이어의 transform
		GameObject* _owner;						// 컴포넌트를 담고 있는 오브젝트
		Status _playerStatus;					// 플레이어의 상태 이동 기절 등
		Status _tempStatus;						// 애니메이션 변환하며 Stop 불러줄 용도
		DirectX::XMVECTOR _directionVector;		// 플레이어가 바라보는 방향 벡터

		int _abilityAnimationIndex;				// 어떤 스킬 애니메이션
		int _cinematicIndex;

		// 피격 후 날아가는 로직 관련
		bool _isSweep;							// 날아가는 트리거
		float _sweepRange;						// 얼마나 멀리 날아갈것인가
		float _movedRange;						// 얼만큼 이동했는가 기록용
		float _sweepDuration;					// 얼마나 오래 체공할 것인가
		float _sweepAnimationSpeed;				// 날아가는 애니메이션 속도
		float _gravity;							// 포물선에 적용할 중력가속도
		std::vector<DirectX::XMFLOAT3> _sweepNode;	// 날아가는 좌표노드
		int _nodeCount;							// 노드 체크 보조변수
		bool _playerBindFlag;					// true일때 행동불가

		GameObject* _deathParticle1;				// 플레이어 죽을 때 출력될 파티클
		GameObject* _deathParticle2;
		GameObject* _deathParticle3;
		GameObject* _deathParticle4;
		GameObject* _deathParticle5;
		GameObject* _deathParticle6;

		std::vector<GameObject*> _deathParticleVector;
		float _deathAnimationSpeed;

		float _playerStartX;					// 보스맵 시작 x z 좌표
		float _playerStartZ;

		int _knock_downSound;
		int _diedsound;
		int _staandup;
		int _hit;

	private:
		// 플레이어의 상태에 따라 애니메이션 출력
		void AnimateByStatus();

		// 플레이어 사망처리
		void CheckDeath();

		// 피격 이후 처리
		void AfterHit();

		// 게임 시작 전 누워있다 일어나기
		void BeforeStart();

		// 플레이어 상태 이상을 처리할 코루틴 함수
		Coroutine_Func(afterSweep)
		{
			auto* playerComp = this;
			Waitforsecond(playerComp->_sweepDuration + 0.3f);				// 누운 다음 1초 뒤 기상

			playerComp->_isSweep = false;
			playerComp->_playerStatus = Status::IDLE;
		};

		/// 디버그 모드에서만 사용할 보조 함수
		void DebugFunc();

	public:
		// 플레이어가 바라보는 방향벡터
		const DirectX::XMVECTOR GetDirectionVector();

		// 플레이어의 상태 반환
		const Status GetPlayerStatus();

		// 플레이어의 정보 반환
		PlayerInfo& GetPlayerData();

		// 플레이어 피격처리
		/// 보스쪽에서 타입을 ENUM으로 바꾸면 매개변수도 변경예정
		void SetHitState(int patternType);

		// 플레이어 피격 후 날아가는 좌표 계산
		void CalculateSweep(DirectX::XMVECTOR direction);
		void CalculateSweep(DirectX::XMFLOAT3 direction);

		// 플레이어 피격 후 날아가는 로직
		void PlayerSweep();

		// 플레이어 scene 이동
		void MoveToScene(std::string sceneName);

		// 플레이어 상태, 체력 초기화
		void ResetPlayerStatus();

		// Flag에 따라 플레이어 행동 불가, 가능 여부 결정	// int 매개변수는 정의 참조
		// 상세는 정의 참조
		void SetPlayerBindFlag(bool flag, int state = 0);
		bool GetPlayerBindFlag();

		void SetPlayerIdle();

		// 플레이어가 스킬 시전중인지 Flag
		bool _onCasting;
	public:
		bool _iswakeuped;
	};
}

