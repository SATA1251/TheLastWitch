#pragma once
#include "KunrealAPI.h"
#include "Boss.h"
#include "Coroutine.h"

namespace KunrealEngine
{
	class SoundPlayer;

	class _DECLSPEC Kamen : public Component, public Boss
	{
	public:
		Kamen();
		virtual ~Kamen();

		void Initialize() override;
		void Release() override;

		void FixedUpdate() override;
		void Update() override;
		void LateUpdate() override;

		void OnTriggerEnter() override;
		void OnTriggerStay() override;
		void OnTriggerExit() override;

		void SetActive(bool active) override;

		virtual void SetMesh() override;
		virtual void SetTexture() override;
		virtual void SetBossTransform() override;
		virtual void SetBossCollider() override;

	public:
		virtual void CreatePattern() override;

		virtual void Idle() override;

		virtual void SpecialAttack2() override;

		virtual void InitializeEnterCameraMove() override;

		virtual bool EnterCameraMove() override;

	public:
		void GamePattern();

	public:
		void Reset();

	private:
		void StopAllSpecialPattern();

	private:
		void CreateParticleObject();

	private:
		void CreateSubObject();

	private:
		// 기본 패턴
		void LeftRightPattern();
		void RightLeftPattern();
		void BackStepCallPattern();
		void TeleportSpellPattern();
		void EmergenceAttackPattern();

		// 검 단독 패턴
		void SwordTurnClockPattern();
		void SwordTurnAntiClockPattern();
		void SwordLinearAttackPattern();
		void SwordChopPattern();

		// 검 WITH 카멘
		void BasicSwordAttackPattern();

		// 코어 패턴
	private:
		void CoreEmmergencePattern();

		void CoreSwordMutipleAttackPattern();

		void CoreSwordMeteorPattern();

		// 기본 패턴 조각들
	private:
		void CreateLeftAttack();
		void CreateLeftAttackThrowingFire();
		void CreateRightAttack();
		void CreateRightAttackThrowingFire();
		void CreateSpellAttack();
		void CreateCallAttack();
		void CreateCall2Attack();
		void CreateBossRandomInsideWarning();
		void CreateFiveWayAttack();

		// 검 with 카멘 패턴
	private:
		void CreateSwordAttack();

		// 검 단독 패턴
	private:
		void CreateSwordEmergence();
		void CreateSwordHide();

		void CraeteSwordTurnClockWise();
		void CreateSwordTurnAntiClock();
		void CreateOutsideSafe();
		void CreateInsideSafe();
		void CreateDonutSafe();
		void CreateDonutAttack();

		void CreateSwordLookPlayer();

		void CreateSwordLinearReady();
		void CreateSwordLinearAttack();

		void CreateSwordChopAttack();

	private:
		void CreateBattleCry();

	private:
		void CreateDecalTest();

		// 1->2 페이즈 핵심 패턴
	private:
		void CreateSwordMeteorAppear();
		void CreateRentalFraud();
		void CreateSwordMeteorAttack();

		//검 핵심 패턴
	private:
		void CreateSwordMultipleAttack();
		void CreateKamenHoldSword();

		// 0줄 패턴
	private:
		void CreateGenkiAttack();

		// 3페이즈 with 검
	private:
		void CreateSwordSwingVertical();
		void CreateSwordSwingTwice();
		void CreateSwordSwingTwiceHard();
		void CreateSwordSwingHorizontal();
		void CreateSwordRotationAttack();

		// 카멘 이동 관련
	private:
		void CreateTurn180();
		void CreateTurn270();
		void CreateBackStep();
		void CreateTeleportToCenter();
		void CreateTeleportToCenterWithLook();
		void CreateTeleportRandomWithLook();
		void CreateTeleportWithSwordSwing();
		void CreateTurnClockWise();
		void CreateTurnAntiClockWise();
		void CreateReverseEmergence();
		void CreateEmergence();

		// 핵심 기믹 패턴
	private:
		void CreateEmergenceAttack();

	public:
		const DirectX::XMFLOAT3& GetSwordDirection();

		const DirectX::XMFLOAT3& GetSwordPos();

		const DirectX::XMFLOAT3& GetGetSwordInsideCenterPos();

		const DirectX::XMFLOAT3& GetEgoPos();

		const float GetDonutSize();

		const DirectX::XMFLOAT3& GetMultipleSwordDirVec(int index);

		// 분신용 편의성 함수
	private:
		DirectX::XMVECTOR GetEgoDirection();

		// 카멘에 붙이는 칼 콜라이더 위치 조정용
	private:
		void SetKamenSwordCollider(int startFrame, int EndFrame, int nowFrame);
		void SetKamenSwordCollider();

		void HoldKamenSword();

	private:
		// 베지어 곡선 활용 함수
		// 일정 수치 사이의 랜덤 보조점을 세팅 (해당 함수는 20 ~ 30의 거리)
		std::vector<DirectX::XMFLOAT3> BezierSetting(GameObject* bezierObject, DirectX::XMFLOAT3 endPoint);

	private:
		BossPattern* _leftAttack;
		BossPattern* _rightAttack;
		BossPattern* _spellAttack;
		BossPattern* _callAttack;
		BossPattern* _call2Attack;
		BossPattern* _bossRandomInsideWarning;
		BossPattern* _fiveWayAttack;

		BossPattern* _backStep;
		BossPattern* _turn180;
		BossPattern* _turn270;
		BossPattern* _teleport;
		BossPattern* _teleportWithLook;
		BossPattern* _teleportRandomWithLook;
		BossPattern* _reverseEmergence;
		BossPattern* _emergence;

		BossPattern* _leftFireAttack;
		BossPattern* _rightFireAttack;

		BossPattern* _turnClockWise;
		BossPattern* _turnAntiClockWise;
		BossPattern* _outsideSafe;
		BossPattern* _insideSafe;
		BossPattern* _donutSafe;
		BossPattern* _donutAttack;

		BossPattern* _basicSwordAttack;
		BossPattern* _swordEmmergence;
		BossPattern* _swordHide;
		BossPattern* _swordTurnClockWise;
		BossPattern* _swordTurnAntiClockWise;
		BossPattern* _swordLinearAtack;
		BossPattern* _swordLinearReady;
		BossPattern* _swordLookPlayer;
		BossPattern* _swordChopAttack;

		/// 3페이즈
		BossPattern* _swordSwingVertical;
		BossPattern* _swordSwingTwice;
		BossPattern* _swordSwingTwiceHard;
		BossPattern* _swordSwingHorizontal;
		BossPattern* _swordRotationAttack;

		BossPattern* _swordSwingTeleport;

	private:
		BossPattern* _emergence9Lich;
		BossPattern* _swordMultipleAttack;

		BossPattern* _battleCry;
		BossPattern* _holdSword;

		BossPattern* _swordMeteorAppear;
		BossPattern* _rentalFraud;
		BossPattern* _swordMeteorAttack;

		BossPattern* _genkiAttack;

	private:
		// 패턴에 필요한 subObject들
		GameObject* _leftHand;
		GameObject* _rightHand;
		GameObject* _call;
		GameObject* _call2;
		GameObject* _lazer;
		GameObject* _lazerCollider;
		GameObject* _bossInsideWarning;
		GameObject* _bossInsideAttack;
		std::vector<GameObject*> _fiveAttack;

		GameObject* _swordInsideWarning;
		GameObject* _swordInsideAttack;
		GameObject* _swordOutsideWarning;
		GameObject* _swordDonutWarning1;
		GameObject* _swordDonutWarning2;
		GameObject* _swordDonutWarning3;

		std::vector<GameObject*> _swordDonutAttack;

		GameObject* _freeSword;
		GameObject* _freeSwordCollider;
		GameObject* _swordPath;

		GameObject* _kamenSword;
		GameObject* _kamenSwordCollider;
		std::vector<GameObject*> _blade;
		GameObject* _largeBlade;

		std::vector<GameObject*> _handFire;
		std::vector<GameObject*> _handFireDecal;

		GameObject* _leftHandBone;
		GameObject* _rightHandBone;

		GameObject* _alterEgo;

		std::vector<GameObject*> _egoHandFire;
		std::vector<GameObject*> _egoHandFireDecal;

		GameObject* _egoCall2;

		GameObject* _egoLazer;
		GameObject* _egoLazerCollider;

		GameObject* _egoLeftHandBone;
		GameObject* _egoRightHandBone;

		GameObject* _egoInsideWarning;

		GameObject* _egoInsideAttack;

		GameObject* _egoEmAttack;

		std::vector<GameObject*> _fakeBoss;

		GameObject* _kamenSwordLight;

		std::vector<GameObject*> _multipleSwordVec;
		std::vector<GameObject*> _multipleSwordWarningVec;

		GameObject* _meteorSword;
		std::vector<GameObject*> _rentalArea;

		std::vector<GameObject*> _rentalCollider;

		GameObject*_cinematicCamera;
		GameObject* _cinematicCamera2;
		GameObject* _mainPlayCamera;

		std::vector<GameObject*> _callDecalVec;
		std::vector<GameObject*> _egoCallDecalVec;
		GameObject* _emergenceDecal;
		GameObject* _egoEmergenceDecal;

		GameObject* _swordOutSideSafeDecal;

		GameObject* _swordMeteorDecal;

	private:
		// Call 거리 체크용
		float _callMoveDistance;
		unsigned int _call2PrevStep;
		bool _isRotateFinish;

		unsigned int _egoCall2PrevStep;

	private:
		std::vector<DirectX::XMFLOAT3> _fakeDirVec;
		std::vector<float> _fakeMoveDistance;

	private:
		std::vector<bool> _handFireReady;
		std::vector<DirectX::XMFLOAT3> _handFireDir;

	private:
		std::vector<bool> _egoHandFireReady;
		std::vector<DirectX::XMFLOAT3> _egoHandFireDir;

	private:
		bool _isCoreStart;
		bool _isRandomStart;

	private:
		unsigned int _targetIndex;

	private:
		float _donutSize;

	private:
		std::function<void()> _callInitLogic;

	private:
		DirectX::XMFLOAT3 _centerPos;

	private:
		DirectX::XMFLOAT3 _randomPos;

	private:
		DirectX::XMFLOAT3 _emergencePos;

	private:
		float _swordRotateAngle;

		DirectX::XMFLOAT3 _swordStartPos;
		DirectX::XMFLOAT3 _swordOriginPos;

		float _swordCircleWarningSize;

		float _bossCircleWarningSize;


		float _swordMoveDistance;
		float _swordMoveDistance2;


		DirectX::XMFLOAT3 _swordDirection;
		DirectX::XMFLOAT3 _swordRotation;

		DirectX::XMFLOAT3 _swordDirection2;

		float _swordChopSpeed;

		float _timer;

		float _timer2;

		float _timer3;

		float _swordTimer;
		float _swordTimer2;

		float _warningMaxTimer;

	private:
		bool _isSwordSecondAttackStart;

	private:
		bool _isSpecial2Ready;
		bool _isSpecial2Playing;
		bool _isEgoAppearInit;
		bool _isEgoAppearFinish;
		bool _isEgoAttackReady;
		bool _isEgoAttack;
		float _egoTimer;

	private:
		bool _rentalSuccess;
	
	private:
		bool _genkiAttackStart;

		bool _genkiHitV;

	private:
		bool _isDecalPosChecked[10];

	private:
		DirectX::XMFLOAT3 _cameraOriginPos;
		DirectX::XMFLOAT3 _cameraMove;
		DirectX::XMFLOAT2 _cameraRot;
		bool _cameraMoveFinish;

		unsigned int _nowCameraStep;

		// 1페이즈 핵심 패턴
	private:
		std::vector<unsigned int> _rentalNumVec;

		// 2페이즈 핵심 패턴
	private:
		std::vector<DirectX::XMFLOAT3> _multipleSwordDirectionVec;
		std::vector<float> _multipleSwordMoveDistance;
		std::vector<bool> _multipleSwordLookPlayer;
		std::vector<bool> _multipleSwordMoveStart;


		// 파티클
	private:
		std::vector<GameObject*> _particleBossfire1;
		std::vector<GameObject*> _particleBossfire2;

		std::vector<GameObject*> _particleEgofire1;
		std::vector<GameObject*> _particleEgofire2;

		std::vector<GameObject*> _particleCall2;
		std::vector<GameObject*> _particleEgoCall2;

		std::vector<GameObject*> _particleSwordOutsideAttack;
		std::vector<GameObject*> _particleSwordDonutAttack;

		std::vector<GameObject*> _particleEmergenceAttack;
		std::vector<GameObject*> _particleEgoEmergenceAttack;

		std::vector<GameObject*> _particleSwordDonut1;
		std::vector<GameObject*> _particleSwordDonut2;
		std::vector<GameObject*> _particleSwordDonut3;

		std::vector <GameObject*> _verticalParticleSide;

		std::vector <GameObject*> _verticalParticle;

		std::vector<GameObject*> _kamenSwordParticle;

		std::vector<GameObject*> _kamenSwordAfterImageParticle;

		std::vector<GameObject*> _bladeRParticle;
		std::vector<GameObject*> _bladeRParticleWave;

		std::vector<GameObject*> _bladeLParticle;
		std::vector<GameObject*> _bladeLParticleWave;

		std::vector<GameObject*> _largeBladeParticle;
		std::vector<GameObject*> _largeBladeParticleWave;

		std::vector<GameObject*> _laserLight;

		std::vector<GameObject*> _kamenLastphaseParticle;
		std::vector<GameObject*> _kamenLastphaseParticleBack;

		std::vector<GameObject*> _meteorSwordHitParticle;

		std::vector<GameObject*> _bossLastAttackList;

		std::vector<GameObject*> _bossShoutingList;
		GameObject* _bossShoutingForce;

		// 베지어 곡선을 이용한 파티클 이동 관련 변수
		std::vector<GameObject*> _bezierSwordParticles;
		std::vector<DirectX::XMFLOAT3> _bezierSwordParticlePoints;
		std::vector<std::vector<DirectX::XMFLOAT3>> _bezierCurvePoints;
		std::vector<float> _timerList;

		DirectX::XMFLOAT3 _targetSword;

		bool _isBezierStartSetting;
		bool _isSettingTimer;
		bool _isDead;

		GameObject* _3PhaseParticle;
		// 스펠 소멸값을 위한 변수

		float _spellDel;
		float _spellWaveDel;

		Light* _mainLight;

	private:
		std::vector<TransparentMesh*> _nowRenderingDecalVec;

	public:
		DirectX::XMFLOAT3 swordDirPos;

	private:
		std::vector<GameObject*> _playerLastBeamList;
		std::vector<GameObject*> _playerLastParticleList;

		std::vector<GameObject*> _clashLastParticleList;

		std::vector<GameObject*> _lastBezierList;

		// 마지막 연출을 위한 변수

		float _playerLastLifetimer;
		float _bossGenkiPos;

		// 사운드용
	private:
		SoundPlayer* _soundComp;
		bool _isBasicPatternSoundPlayed[5];

		int _leftFireSoundIndex;
		int _callSoundIndex;
		int _callSoundIndex2;
		int _callSoundIndex3;
		int _callSoundIndex4;
		int _callSoundIndex5;

		int _spellSoundIndex;
		int _emergenceSoundIndex;
		int _fiveWaySoundIndex;

		int _crySoundIndex;
		int _meteorSwordSoundAppearIndex;
		int _meteorSwordAttackSoundIndex;

		bool _isSwordPatternSoundPlayed[3];

		int _swordInsideSafeSoundIndex;
		int _swordOutsideSafeSoundIndex;
		int _swordLinearSoundIndex;
		int _swordChopSoundIndex1;
		int _swordChopSoundIndex2;
		int _swordChopSoundIndex3;

		std::vector<int> _swordMultipleSoundIndexVec;
		std::vector<bool> _isCorePatternSoundPlayed;

		int _swordSwingSound1;
		int _swordSwingSound2;
		int _horizontalChargeSound;
		int _horizontalSwingSound;
		int _aurabladeShotSound1;
		int _aurabladeShotSound2;
		int _vertucalSound;

		int _genkitamaCharge;
		int _getnkitamaShot;
		int _getnkitamaShouting;
		int _playerLastLaser;
	};
}