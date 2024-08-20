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

		// Component의 가상함수들 오버라이드
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
		// 이쪽 변수 확인시 참조.
		// 이미지에 변화가 오지 않을 배경 이미지들은 그냥 Initialize 할때 함수 내부에서 지역변수로 생성하고 관리했음.
		// (필요하다면 Setparent 해둘거니까 그걸 통해서 접근하면 됨)
		// 여기 있는 변수들은 모두 오브젝트에 변화가 있을거라고 판단해서 가져다 쓰기 쉽게 꺼내둔 것들

		GameObject* _battleuibox;	// 부모가 되어줄 오브젝트(Empty)

		GameObject* _ui_skill1_cool;	// 1번 스킬 쿨다운
		GameObject* _ui_skill2_cool;	// 2번 스킬 쿨다운
		GameObject* _ui_skill3_cool;	// 3번 스킬 쿨다운
		GameObject* _ui_skill4_cool;	// 4번 스킬 쿨다운

		GameObject* _potion_cool;			// 포션 먹기
		GameObject* _potion_text;			// 포션 먹기
		GameObject* _dash_cool;			// 대쉬(회피?)

		GameObject* _playerhp_bar;		// 플레이어 체력바
		GameObject* _playerhp_bar_downGauge;	// 보스 체력바 체력 감소 연출용
		GameObject* _bosshp_bar;			// 보스 체력바
		GameObject* _bosshp_bar_downGauge;	// 보스 체력바 체력 감소 연출용
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