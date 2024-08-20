#pragma once
#include "CommonHeader.h"
#include "Coroutine.h"
#include "ImageRenderer.h"

struct BossBasicInfo;

enum class eDifficulty
{
	eEasy,			// �ÿ���
	eNormal,		// ������ 
	eHardCore		// 1�� ������ ����	
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

		// �̱���
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
		/// ī���� �̰��� ����������!
		/// �� �Լ����� �׳� Main������ ȣ��Ȱ� ����� �մϴ�.
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
		// �÷��̾ �������� �ִ� ������ ���
		void CalculateDamageToBoss(Ability* abil);

	private:
		// ������ �÷��̾�� �ִ� ������ ���
		void CalculateDamageToPlayer();

		// ������ �÷��̾�� �ִ� ������ ���2
		void CalculateDamageToPlayer2();

	private:
		// ���������� �ٲ���� �� ������ �������� �˷��ִ� �Լ�
		void SetBossObject();

		void UIcontrol();

		// �ϴ� ����ٰ�... ��ġ �����
	private:
		const DirectX::XMVECTOR& SetWarningAttackDirection(GameObject* subObject);
		const DirectX::XMVECTOR& SetBossAttackDirection(GameObject* subObject);
		const DirectX::XMVECTOR& SetEgoAttackDirection(GameObject* subObject);

		_Coroutine(DashCamMove);

		// �÷��̾� ����� Ÿ��Ʋȭ������
		void MoveToTitle();

		// ���ο��� ����������
		void MoveToEnding();

		// ������ ������Ʈ�� �ʱ�ȭ
		void ResetEndingSceneObjects();
	public:
		// ������ �̵� Ʈ����
		void ActivateEndingSceneTrigger();

		// FadeOut 	
		void ActivateFadeOutTrigger();

		// ������ fade ����
		void ActivateEndingFadeTrigger();

		// �����ھ� �޾ƿ���
		void GetEngineCore(EngineCore* core);

		// ���̵� ����
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