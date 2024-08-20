#pragma once
#include "CommonHeader.h"
#include "KunrealAPI.h"
#include "Coroutine.h"

namespace KunrealEngine
{
	class EventManager;

	class BattleUIManager : public Component
	{
	public:
		BattleUIManager();
		~BattleUIManager();

		// Component�� �����Լ��� �������̵�
		void Initialize() override;
		void Release() override;

		void FixedUpdate() override;
		void Update() override;
		void LateUpdate() override;

		void OnTriggerEnter() override;
		void OnTriggerStay() override;
		void OnTriggerExit() override;

		void SetActive(bool active) override;

	public:
		void SetBossHpbar();
		void SetPlayerHpBar();

		void SetSkillcool1();
		void SetSkillcool2();
		void SetSkillcool3();
		void SetSkillcool4();
		void Setdashcool();
		void Setpotioncool();

		void ActiveDiedUI();

		void Resebattleuibar();

	private:
		// ���� ���� Ȯ�ν� ����.
		// �̹����� ��ȭ�� ���� ���� ��� �̹������� �׳� Initialize �Ҷ� �Լ� ���ο��� ���������� �����ϰ� ��������.
		// (�ʿ��ϴٸ� Setparent �صѰŴϱ� �װ� ���ؼ� �����ϸ� ��)
		// ���� �ִ� �������� ��� ������Ʈ�� ��ȭ�� �����Ŷ�� �Ǵ��ؼ� ������ ���� ���� ������ �͵�

		GameObject* _battleuibox;	// �θ� �Ǿ��� ������Ʈ(Empty)

		GameObject* _ui_skill1_cool;	// 1�� ��ų ��ٿ�
		GameObject* _ui_skill2_cool;	// 2�� ��ų ��ٿ�
		GameObject* _ui_skill3_cool;	// 3�� ��ų ��ٿ�
		GameObject* _ui_skill4_cool;	// 4�� ��ų ��ٿ�

		GameObject* _potion_cool;			// ���� �Ա�
		GameObject* _potion_text;			// ���� �Ա�
		GameObject* _dash_cool;			// �뽬(ȸ��?)

		GameObject* _playerhp_bar;		// �÷��̾� ü�¹�
		GameObject* _playerhp_bar_downGauge;	// ���� ü�¹� ü�� ���� �����
		GameObject* _bosshp_bar;			// ���� ü�¹�
		GameObject* _bosshp_bar_downGauge;	// ���� ü�¹� ü�� ���� �����
		float booshp_targetscale;
		float playerhp_targetscale;

		int pre_playerhp;
		int pre_bosshp;

		GameObject* _died2;

	private:
		EventManager* _eventmanager;
		const float _bosshpsize;
		const float _playerhpsize;
		const float _skillcoolsize;

		_Coroutine(bossdowngauge);
		_Coroutine(playerdowngauge);

		_Coroutine(skillgauge1);
		_Coroutine(skillgauge2);
		_Coroutine(skillgauge3);
		_Coroutine(skillgauge4);
		_Coroutine(dashgauge);
		_Coroutine(potiongauge);

		_Coroutine(diedcoro);
		bool _isdied;
	};
}