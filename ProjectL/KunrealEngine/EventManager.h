#pragma once
#include "CommonHeader.h"
#include "Coroutine.h"
#include "ImageRenderer.h"

struct BossBasicInfo;

enum class eDifficulty
{
	eEasy,			// 시연꺼
	eNormal,		// 원래꺼 
	eHardCore		// 1대 맞으면 죽음	
};

namespace KunrealEngine
{
	class GameObject;
	class Component;
	class PlayerAbility;
	class Ability;
	class Camera;
	
	class Player;
	class Kamen;
	class EngineCore;

	class EventManager
	{
	private:
		EventManager();
		~EventManager();

	public:
		void Initialize();
		void Release();

		void Update();

		void SetCamera(std::string name);
		std::vector<DirectX::XMFLOAT2> CamShake(float radius, int numPoints);
		std::vector<float> CamShakeLinear(float sigma, int numPoints);
		void CamShake(float harder);

		// 싱글톤
	public:
		static EventManager& GetInstance()
		{
			static EventManager _instance;
			return _instance;
		}

	public:
		GameObject* _player;
		GameObject* _boss;
		GameObject* _mainCamera;

	public:
		Player* _playerComp;
		Kamen* _bossComp;

		PlayerAbility* _playerAbill;

		EngineCore* _core;

		bool _eventStart;

		bool _iscamfollow;
		float _camshakex;
		float _camshakez;

	public:
		///
		/// 카멘은 이곳을 가져가도록!
		/// 이 함수들은 그냥 Main내에서 호출된걸 전재로 합니다.
		/// 
		void BattleUIHide(float speed);
		void BattleUIOpen(float speed);
		void ActiveVbutton(float speed, float xpos, float ypos);
		void ActiveSpaceButton(float time, float xpos, float ypos);

		GameObject* _Vbutton;
		GameObject* _Spacebutton;

		void CreateFadeObject();
	private:
		unsigned int _insideSafeCount;

		GameObject* _fadeObjectTitle;
		GameObject* _fadeObjectMain;
		GameObject* _fadeObjectEnding;
		GameObject* _fadeObjectWhiteMain;
		GameObject* _fadeObjectWhiteEnding;

	public:
		// 플레이어가 보스에게 주는 데미지 계산
		void CalculateDamageToBoss(Ability* abil);

	private:
		// 보스가 플레이어에게 주는 데미지 계산
		void CalculateDamageToPlayer();

		// 보스가 플레이어에게 주는 데미지 계산2
		void CalculateDamageToPlayer2();

	private:
		// 스테이지가 바뀌었을 때 보스가 누구인지 알려주는 함수
		void SetBossObject();

		void UIcontrol();

		// 일단 여기다가... 위치 고민중
	private:
		const DirectX::XMVECTOR& SetWarningAttackDirection(GameObject* subObject);
		const DirectX::XMVECTOR& SetBossAttackDirection(GameObject* subObject);
		const DirectX::XMVECTOR& SetEgoAttackDirection(GameObject* subObject);

		_Coroutine(DashCamMove);

		// 플레이어 사망후 타이틀화면으로
		void MoveToTitle();

		// 메인에서 엔딩씬으로
		void MoveToEnding();

		// 엔딩씬 오브젝트들 초기화
		void ResetEndingSceneObjects();
	public:
		// 엔딩씬 이동 트리거
		void ActivateEndingSceneTrigger();

		// FadeOut 	
		void ActivateFadeOutTrigger();

		// 엔딩씬 fade 연출
		void ActivateEndingFadeTrigger();

		// 엔진코어 받아오기
		void GetEngineCore(EngineCore* core);

		// 난이도 조절
		void SetDifficulty(eDifficulty dif);

	public:
		eDifficulty _difficulty;

	private:
		Coroutine_Func(fadeout)
		{
			EventManager* manager = this;
			GameObject* blackBG = manager->_fadeObjectMain;
			blackBG->SetActive(true);
			blackBG->GetComponent<ImageRenderer>()->SetAlpha(0.0f);

			Waitforsecond(3.0f);

			while (blackBG->GetComponent<ImageRenderer>()->GetAlpha() < 1.0f)
			{
				blackBG->GetComponent<ImageRenderer>()->SetAlpha(blackBG->GetComponent<ImageRenderer>()->GetAlpha() + 0.005f);

				if (blackBG->GetComponent<ImageRenderer>()->GetAlpha() >= 1.0f)
				{
					blackBG->GetComponent<ImageRenderer>()->SetAlpha(1.0f);

					Waitforsecond(2.0f);
					blackBG->SetActive(false);
					manager->MoveToTitle();

					break;
				}

				Return_null;
			}		
		};

		Coroutine_Func(fadein)
		{
			EventManager* manager = this;
			GameObject* blackBG = manager->_fadeObjectTitle;
			blackBG->SetActive(true);
			blackBG->GetComponent<ImageRenderer>()->SetAlpha(1.0f);

			while (blackBG->GetComponent<ImageRenderer>()->GetAlpha() > 0.0f)
			{
				blackBG->GetComponent<ImageRenderer>()->SetAlpha(blackBG->GetComponent<ImageRenderer>()->GetAlpha() - 0.005f);

				if (blackBG->GetComponent<ImageRenderer>()->GetAlpha() < 0.0f)
				{
					blackBG->GetComponent<ImageRenderer>()->SetAlpha(0.0f);
					blackBG->SetActive(false);

					break;
				}

				Return_null;
			}
		};

		Coroutine_Func(endingSceneFade)
		{
			EventManager* manager = this;
			GameObject* blackBG = manager->_fadeObjectEnding;
			blackBG->SetActive(true);
			blackBG->GetComponent<ImageRenderer>()->SetAlpha(0.0f);

			while (blackBG->GetComponent<ImageRenderer>()->GetAlpha() < 1.0f)
			{
				blackBG->GetComponent<ImageRenderer>()->SetAlpha(blackBG->GetComponent<ImageRenderer>()->GetAlpha() + 0.01f);

				if (blackBG->GetComponent<ImageRenderer>()->GetAlpha() >= 1.0f)
				{
					blackBG->GetComponent<ImageRenderer>()->SetAlpha(1.0f);
					break;
				}

				Return_null;
			}

			Waitforsecond(2.5f);

			while (blackBG->GetComponent<ImageRenderer>()->GetAlpha() > 0.0f)
			{
				blackBG->GetComponent<ImageRenderer>()->SetAlpha(blackBG->GetComponent<ImageRenderer>()->GetAlpha() - 0.01f);

				if (blackBG->GetComponent<ImageRenderer>()->GetAlpha() < 0.0f)
				{
					blackBG->GetComponent<ImageRenderer>()->SetAlpha(0.0f);
					blackBG->SetActive(false);
					break;
				}

				Return_null;
			}
		};

		Coroutine_Func(WhiteFadeOut)
		{
			EventManager* manager = this;
			GameObject* whiteBG = manager->_fadeObjectWhiteMain;
			whiteBG->SetActive(true);
			whiteBG->GetComponent<ImageRenderer>()->SetAlpha(0.0f);

			while (whiteBG->GetComponent<ImageRenderer>()->GetAlpha() < 1.0f)
			{
				whiteBG->GetComponent<ImageRenderer>()->SetAlpha(whiteBG->GetComponent<ImageRenderer>()->GetAlpha() + 0.005f);

				if (whiteBG->GetComponent<ImageRenderer>()->GetAlpha() >= 1.0f)
				{
					whiteBG->GetComponent<ImageRenderer>()->SetAlpha(1.0f);

					Waitforsecond(1.0f);
					whiteBG->SetActive(false);
					manager->MoveToEnding();

					break;
				}

				Return_null;
			}
		};

		Coroutine_Func(WhiteFadeIn)
		{
			EventManager* manager = this;
			GameObject* blackBG = manager->_fadeObjectWhiteEnding;
			blackBG->SetActive(true);
			blackBG->GetComponent<ImageRenderer>()->SetAlpha(1.0f);

			while (blackBG->GetComponent<ImageRenderer>()->GetAlpha() > 0.0f)
			{
				blackBG->GetComponent<ImageRenderer>()->SetAlpha(blackBG->GetComponent<ImageRenderer>()->GetAlpha() - 0.01f);

				if (blackBG->GetComponent<ImageRenderer>()->GetAlpha() < 0.0f)
				{
					blackBG->GetComponent<ImageRenderer>()->SetAlpha(0.0f);
					blackBG->SetActive(false);

					break;
				}

				Return_null;
			}
		};
	};
}