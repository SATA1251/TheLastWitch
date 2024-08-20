#include "BattleUIManager.h"
#include "EventManager.h"
#include "TimeManager.h"
#include "PlayerAbility.h"
#include "ButtonSystem.h"

KunrealEngine::BattleUIManager::BattleUIManager()
	:_bosshpsize(83.0f), _playerhpsize(47.5f), _skillcoolsize(1.0f), _isdied(false), _battleuibox(nullptr)
	, _bosshp_bar(nullptr), _bosshp_bar_downGauge(nullptr), _playerhp_bar(nullptr), _playerhp_bar_downGauge(nullptr)
	, _potion_cool(nullptr), _dash_cool(nullptr), _ui_skill1_cool(nullptr), _ui_skill2_cool(nullptr)
	, _ui_skill3_cool(nullptr), _ui_skill4_cool(nullptr), pre_playerhp(0), pre_bosshp(0), _died2(nullptr)
	, _eventmanager(nullptr), booshp_targetscale(0), playerhp_targetscale(0)
{
	_eventmanager = &KunrealEngine::EventManager::GetInstance();
}

KunrealEngine::BattleUIManager::~BattleUIManager()
{
}

void KunrealEngine::BattleUIManager::Initialize()
{
	SceneManager& scene = KunrealEngine::SceneManager::GetInstance();

	GameObject* booshpbar_pack;		// 전투 UI 배경
	GameObject* battleui_pack;		// 전투 UI 배경

	GameObject* player_background;		// 전투 UI 배경
	GameObject* player_background1;		// 전투 UI 배경
	GameObject* player_background2;		// 전투 UI 배경

	GameObject* boss_background;		// 전투 UI 배경
	GameObject* boss_background1;		// 전투 UI 배경
	GameObject* boss_background2;		// 전투 UI 배경

	GameObject* bosshp_barback;			// 보스 체력바 배경
	GameObject* bosshp_background;		// 플레이어 체력바 배경
	GameObject* bosshp_background1;		// 플레이어 체력바 배경
	GameObject* bosshp_background2;		// 플레이어 체력바 배경

	GameObject* playerhp_barback;		// 플레이어 체력바 배경
	GameObject* playerhp_background;	// 플레이어 체력바 배경
	GameObject* playerhp_background1;	// 플레이어 체력바 배경
	GameObject* playerhp_background2;	// 플레이어 체력바 배경

	GameObject* ui_skill1_icon;			// 1번 스킬
	GameObject* ui_skill1;				// 1번 스킬
	GameObject* ui_skill1_text;			// 1번 스킬
	GameObject* ui_skill1_tip;			// 1번 스킬

	GameObject* ui_skill2_icon;			// 2번 스킬
	GameObject* ui_skill2;				// 2번 스킬
	GameObject* ui_skill2_text;			// 2번 스킬
	GameObject* ui_skill2_tip;			// 2번 스킬

	GameObject* ui_skill3_icon;			// 3번 스킬
	GameObject* ui_skill3;				// 3번 스킬
	GameObject* ui_skill3_text;			// 3번 스킬
	GameObject* ui_skill3_tip;			// 3번 스킬

	GameObject* ui_skill4_icon;			// 4번 스킬
	GameObject* ui_skill4;				// 4번 스킬
	GameObject* ui_skill4_text;			// 4번 스킬
	GameObject* ui_skill4_tip;			// 4번 스킬

	GameObject* potion_icon;			// 포션 먹기
	GameObject* potion;					// 포션 먹기
	GameObject* potion_tip;					// 포션 먹기

	GameObject* dash_icon;				// 대쉬(회피?)
	GameObject* dash;					// 대쉬(회피?)
	GameObject* dash_text;				// 대쉬(회피?)
	GameObject* dash_tip;				// 대쉬(회피?)

	_battleuibox = this->GetOwner();
	_battleuibox->GetComponent<Transform>()->SetPosition(320.0f, 0.0f, 1.0f);

	booshpbar_pack = scene.GetCurrentScene()->CreateObject("boss_pack");
	booshpbar_pack->SetParent(_battleuibox);
	booshpbar_pack->GetComponent<Transform>()->SetPosition(0.0f, -500.0f, 1.0f);
	
	battleui_pack = scene.GetCurrentScene()->CreateObject("battle_pack");
	battleui_pack->SetParent(_battleuibox);
	battleui_pack->GetComponent<Transform>()->SetPosition(0.0f, 600.0f, 1.0f);
	//battleui_pack->GetComponent<Transform>()->SetPosition(0.0f, 0.0f, 1.0f);

	player_background = scene.GetCurrentScene()->CreateObject("background");
	player_background->SetParent(battleui_pack);
	player_background->AddComponent<ImageRenderer>();
	player_background->GetComponent<ImageRenderer>()->SetImage("ui/freebox-middle.png");
	player_background->GetComponent<ImageRenderer>()->SetPosition(673.0f, 933.0f);
	player_background->GetComponent<Transform>()->SetScale(2.81f, 0.5f, 1.0f);

	player_background1 = scene.GetCurrentScene()->CreateObject("background");
	player_background1->SetParent(battleui_pack);
	player_background1->AddComponent<ImageRenderer>();
	player_background1->GetComponent<ImageRenderer>()->SetImage("ui/freebox-sideL.png");
	player_background1->GetComponent<ImageRenderer>()->SetPosition(651.0f, 933.0f);
	player_background1->GetComponent<Transform>()->SetScale(0.5f, 0.5f, 1.0f);

	player_background2 = scene.GetCurrentScene()->CreateObject("background");
	player_background2->SetParent(battleui_pack);
	player_background2->AddComponent<ImageRenderer>();
	player_background2->GetComponent<ImageRenderer>()->SetImage("ui/freebox-sideR.png");
	player_background2->GetComponent<ImageRenderer>()->SetPosition(1268.0f, 933.0f);
	player_background2->GetComponent<Transform>()->SetScale(0.5f, 0.5f, 1.0f);

	////////////////////////////////////////////////////////////////////////////////////////

	ui_skill1_icon = scene.GetCurrentScene()->CreateObject("ui_skill1_icon");
	ui_skill1_icon->SetParent(battleui_pack);
	ui_skill1_icon->AddComponent<ImageRenderer>();
	ui_skill1_icon->GetComponent<ImageRenderer>()->SetImage("ui/SkillIcon/Shot.png");
	ui_skill1_icon->GetComponent<ImageRenderer>()->SetPosition(700.0f, 967.f);
	ui_skill1_icon->GetComponent<Transform>()->SetScale(0.22f, 0.22f, 0.22f);

	_ui_skill1_cool = scene.GetCurrentScene()->CreateObject("_ui_skill1_cool");
	_ui_skill1_cool->SetParent(battleui_pack);
	_ui_skill1_cool->AddComponent<ImageRenderer>();
	_ui_skill1_cool->GetComponent<ImageRenderer>()->SetImage("ui/skill_backspace.png");
	_ui_skill1_cool->GetComponent<ImageRenderer>()->SetPosition(708.f, 980.f);
	_ui_skill1_cool->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

	ui_skill1 = scene.GetCurrentScene()->CreateObject("ui_skill1");
	ui_skill1->SetParent(battleui_pack);
	ui_skill1->AddComponent<ImageRenderer>();
	ui_skill1->GetComponent<ImageRenderer>()->SetImage("ui/skill space.png");
	ui_skill1->GetComponent<ImageRenderer>()->SetPosition(708.0f, 980.f);
	ui_skill1->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

	ui_skill1_text = scene.GetCurrentScene()->CreateObject("ui_skill1");
	ui_skill1_text->SetParent(battleui_pack);
	ui_skill1_text->AddComponent<ImageRenderer>();
	ui_skill1_text->GetComponent<ImageRenderer>()->SetImage("ui/SkillText/QText.png");
	ui_skill1_text->GetComponent<ImageRenderer>()->SetPosition(708.0f, 980.f);
	ui_skill1_text->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

	ui_skill1_tip = scene.GetCurrentScene()->CreateObject("ui_skill1");
	ui_skill1_tip->SetParent(battleui_pack);
	ui_skill1_tip->AddComponent<ImageRenderer>();
	ui_skill1_tip->GetComponent<ImageRenderer>()->SetImage("ui/SkillExplain/QSkillText.png");
	ui_skill1_tip->GetComponent<ImageRenderer>()->SetPosition(708.0f, 650.f);
	ui_skill1_tip->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);
	ui_skill1_tip->SetActive(false);
	
	/////////////////////////////////////////////////////////////

	ui_skill2_icon = scene.GetCurrentScene()->CreateObject("ui_skill2_icon");
	ui_skill2_icon->SetParent(battleui_pack);
	ui_skill2_icon->AddComponent<ImageRenderer>();
	ui_skill2_icon->GetComponent<ImageRenderer>()->SetImage("ui/SkillIcon/wSkill.png");
	ui_skill2_icon->GetComponent<ImageRenderer>()->SetPosition(817.f, 977.f);
	ui_skill2_icon->GetComponent<Transform>()->SetScale(0.2f, 0.2f, 0.2f);

	_ui_skill2_cool = scene.GetCurrentScene()->CreateObject("_ui_skill2_cool");
	_ui_skill2_cool->SetParent(battleui_pack);
	_ui_skill2_cool->AddComponent<ImageRenderer>();
	_ui_skill2_cool->GetComponent<ImageRenderer>()->SetImage("ui/skill_backspace.png");
	_ui_skill2_cool->GetComponent<ImageRenderer>()->SetPosition(820.f, 980.f);
	_ui_skill2_cool->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

	ui_skill2 = scene.GetCurrentScene()->CreateObject("ui_skill2");
	ui_skill2->SetParent(battleui_pack);
	ui_skill2->AddComponent<ImageRenderer>();
	ui_skill2->GetComponent<ImageRenderer>()->SetImage("ui/skill space.png");
	ui_skill2->GetComponent<ImageRenderer>()->SetPosition(820.f, 980.f);
	ui_skill2->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

	ui_skill2_text = scene.GetCurrentScene()->CreateObject("ui_skill2");
	ui_skill2_text->SetParent(battleui_pack);
	ui_skill2_text->AddComponent<ImageRenderer>();
	ui_skill2_text->GetComponent<ImageRenderer>()->SetImage("ui/SkillText/WText.png");
	ui_skill2_text->GetComponent<ImageRenderer>()->SetPosition(820.f, 980.f);
	ui_skill2_text->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

	ui_skill2_tip = scene.GetCurrentScene()->CreateObject("ui_skill2");
	ui_skill2_tip->SetParent(battleui_pack);
	ui_skill2_tip->AddComponent<ImageRenderer>();
	ui_skill2_tip->GetComponent<ImageRenderer>()->SetImage("ui/SkillExplain/WSkillText.png");
	ui_skill2_tip->GetComponent<ImageRenderer>()->SetPosition(820.f, 650.f);
	ui_skill2_tip->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);
	ui_skill2_tip->SetActive(false);

	/////////////////////////////////////////////////////////////
	
	ui_skill3_icon = scene.GetCurrentScene()->CreateObject("ui_skill3_icon");
	ui_skill3_icon->SetParent(battleui_pack);
	ui_skill3_icon->AddComponent<ImageRenderer>();
	ui_skill3_icon->GetComponent<ImageRenderer>()->SetImage("ui/SkillIcon/laser.png");
	ui_skill3_icon->GetComponent<ImageRenderer>()->SetPosition(932.f, 975.f);
	ui_skill3_icon->GetComponent<Transform>()->SetScale(0.2f, 0.2f, 0.2f);

	_ui_skill3_cool = scene.GetCurrentScene()->CreateObject("_ui_skill3_cool");
	_ui_skill3_cool->SetParent(battleui_pack);
	_ui_skill3_cool->AddComponent<ImageRenderer>();
	_ui_skill3_cool->GetComponent<ImageRenderer>()->SetImage("ui/skill_backspace.png");
	_ui_skill3_cool->GetComponent<ImageRenderer>()->SetPosition(935.f, 980.f);
	_ui_skill3_cool->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

	ui_skill3 = scene.GetCurrentScene()->CreateObject("ui_skill3");
	ui_skill3->SetParent(battleui_pack);
	ui_skill3->AddComponent<ImageRenderer>();
	ui_skill3->GetComponent<ImageRenderer>()->SetImage("ui/skill space.png");
	ui_skill3->GetComponent<ImageRenderer>()->SetPosition(935.f, 980.f);
	ui_skill3->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

	ui_skill3_text = scene.GetCurrentScene()->CreateObject("ui_skill3");
	ui_skill3_text->SetParent(battleui_pack);
	ui_skill3_text->AddComponent<ImageRenderer>();
	ui_skill3_text->GetComponent<ImageRenderer>()->SetImage("ui/SkillText/EText.png");
	ui_skill3_text->GetComponent<ImageRenderer>()->SetPosition(935.f, 980.f);
	ui_skill3_text->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

	ui_skill3_tip = scene.GetCurrentScene()->CreateObject("ui_skill3");
	ui_skill3_tip->SetParent(battleui_pack);
	ui_skill3_tip->AddComponent<ImageRenderer>();
	ui_skill3_tip->GetComponent<ImageRenderer>()->SetImage("ui/SkillExplain/ESkillText.png");
	ui_skill3_tip->GetComponent<ImageRenderer>()->SetPosition(935.f, 650.f);
	ui_skill3_tip->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);
	ui_skill3_tip->SetActive(false);

	///////////////////////////////////////////////////////////////////////////////////////////////
	
	ui_skill4_icon = scene.GetCurrentScene()->CreateObject("ui_skill4");
	ui_skill4_icon->SetParent(battleui_pack);
	ui_skill4_icon->AddComponent<ImageRenderer>();
	ui_skill4_icon->GetComponent<ImageRenderer>()->SetImage("ui/SkillIcon/Meteor.png");
	ui_skill4_icon->GetComponent<ImageRenderer>()->SetPosition(1050.f, 975.f);
	ui_skill4_icon->GetComponent<Transform>()->SetScale(0.19f, 0.19f, 0.19f);

	_ui_skill4_cool = scene.GetCurrentScene()->CreateObject("_ui_skill4_cool");
	_ui_skill4_cool->SetParent(battleui_pack);
	_ui_skill4_cool->AddComponent<ImageRenderer>();
	_ui_skill4_cool->GetComponent<ImageRenderer>()->SetImage("ui/skill_backspace.png");
	_ui_skill4_cool->GetComponent<ImageRenderer>()->SetPosition(1050.f, 980.f);
	_ui_skill4_cool->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

	ui_skill4 = scene.GetCurrentScene()->CreateObject("ui_skill4_icon");
	ui_skill4->SetParent(battleui_pack);
	ui_skill4->AddComponent<ImageRenderer>();
	ui_skill4->GetComponent<ImageRenderer>()->SetImage("ui/skill space.png");
	ui_skill4->GetComponent<ImageRenderer>()->SetPosition(1050.f, 980.f);
	ui_skill4->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

	ui_skill4_text = scene.GetCurrentScene()->CreateObject("ui_skill4_icon");
	ui_skill4_text->SetParent(battleui_pack);
	ui_skill4_text->AddComponent<ImageRenderer>();
	ui_skill4_text->GetComponent<ImageRenderer>()->SetImage("ui/SkillText/RText.png");
	ui_skill4_text->GetComponent<ImageRenderer>()->SetPosition(1050.f, 980.f);
	ui_skill4_text->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

	ui_skill4_tip = scene.GetCurrentScene()->CreateObject("ui_skill4_icon");
	ui_skill4_tip->SetParent(battleui_pack);
	ui_skill4_tip->AddComponent<ImageRenderer>();
	ui_skill4_tip->GetComponent<ImageRenderer>()->SetImage("ui/SkillExplain/RSkillText.png");
	ui_skill4_tip->GetComponent<ImageRenderer>()->SetPosition(1050.f, 650.f);
	ui_skill4_tip->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);
	ui_skill4_tip->SetActive(false);

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	potion_icon = scene.GetCurrentScene()->CreateObject("ui_skill5");
	potion_icon->SetParent(battleui_pack);
	potion_icon->AddComponent<ImageRenderer>();
	potion_icon->GetComponent<ImageRenderer>()->SetImage("ui/potion.png");
	potion_icon->GetComponent<ImageRenderer>()->SetPosition(1170.f, 976.f);
	potion_icon->GetComponent<Transform>()->SetScale(0.8f, 0.8f, 1.0f);

	_potion_cool = scene.GetCurrentScene()->CreateObject("ui_skill5");
	_potion_cool->SetParent(battleui_pack);
	_potion_cool->AddComponent<ImageRenderer>();
	_potion_cool->GetComponent<ImageRenderer>()->SetImage("ui/skill_backspace.png");
	_potion_cool->GetComponent<ImageRenderer>()->SetPosition(1170.f, 976.f);
	_potion_cool->GetComponent<Transform>()->SetScale(0.4f, 0.4f, 1.0f);

	potion = scene.GetCurrentScene()->CreateObject("ui_skill5");
	potion->SetParent(battleui_pack);
	potion->AddComponent<ImageRenderer>();
	potion->GetComponent<ImageRenderer>()->SetImage("ui/sub-skill.png");
	potion->GetComponent<ImageRenderer>()->SetPosition(1170.f, 976.f);
	potion->GetComponent<Transform>()->SetScale(0.8f, 0.8f, 1.0f);

	_potion_text = scene.GetCurrentScene()->CreateObject("ui_skill5");
	_potion_text->SetParent(battleui_pack);
	_potion_text->AddComponent<ImageRenderer>();
	_potion_text->GetComponent<ImageRenderer>()->SetImage("ui/SkillText/Potion5.png");
	_potion_text->GetComponent<ImageRenderer>()->SetPosition(1170.f, 976.f);
	_potion_text->GetComponent<Transform>()->SetScale(0.8f, 0.8f, 1.0f);

	potion_tip = scene.GetCurrentScene()->CreateObject("ui_skill5");
	potion_tip->SetParent(battleui_pack);
	potion_tip->AddComponent<ImageRenderer>();
	potion_tip->GetComponent<ImageRenderer>()->SetImage("ui/SkillExplain/1SkillText.png");
	potion_tip->GetComponent<ImageRenderer>()->SetPosition(1170.f, 650.f);
	potion_tip->GetComponent<Transform>()->SetScale(0.8f, 0.8f, 1.0f);
	potion_tip->SetActive(false);

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	dash_icon = scene.GetCurrentScene()->CreateObject("ui_skill6");
	dash_icon->SetParent(battleui_pack);
	dash_icon->AddComponent<ImageRenderer>();
	dash_icon->GetComponent<ImageRenderer>()->SetImage("ui/SkillIcon/spaceSkill.png");
	dash_icon->GetComponent<ImageRenderer>()->SetPosition(1170.f, 1023.f);
	dash_icon->GetComponent<Transform>()->SetScale(0.16f, 0.16f, 0.16f);

	_dash_cool = scene.GetCurrentScene()->CreateObject("ui_skill6");
	_dash_cool->SetParent(battleui_pack);
	_dash_cool->AddComponent<ImageRenderer>();
	_dash_cool->GetComponent<ImageRenderer>()->SetImage("ui/skill_backspace.png");
	_dash_cool->GetComponent<ImageRenderer>()->SetPosition(1170.f, 1023.f);
	_dash_cool->GetComponent<Transform>()->SetScale(0.4f, 0.4f, 1.0f);

	dash = scene.GetCurrentScene()->CreateObject("ui_skill6");
	dash->SetParent(battleui_pack);
	dash->AddComponent<ImageRenderer>();
	dash->GetComponent<ImageRenderer>()->SetImage("ui/sub-skill.png");
	dash->GetComponent<ImageRenderer>()->SetPosition(1170.f, 1023.f);
	dash->GetComponent<Transform>()->SetScale(0.8f, 0.8f, 1.0f);

	dash_text = scene.GetCurrentScene()->CreateObject("ui_skill6");
	dash_text->SetParent(battleui_pack);
	dash_text->AddComponent<ImageRenderer>();
	dash_text->GetComponent<ImageRenderer>()->SetImage("ui/SkillText/SpaceText.png");
	dash_text->GetComponent<ImageRenderer>()->SetPosition(1170.f, 1023.f);
	dash_text->GetComponent<Transform>()->SetScale(0.8f, 0.8f, 1.0f);

	dash_tip = scene.GetCurrentScene()->CreateObject("ui_skill6");
	dash_tip->SetParent(battleui_pack);
	dash_tip->AddComponent<ImageRenderer>();
	dash_tip->GetComponent<ImageRenderer>()->SetImage("ui/SkillExplain/SpaceSkillText.png");
	dash_tip->GetComponent<ImageRenderer>()->SetPosition(1170.f, 650.f);
	dash_tip->GetComponent<Transform>()->SetScale(0.8f, 0.8f, 1.0f);
	dash_tip->SetActive(false);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	ui_skill1->AddComponent<ButtonSystem>();
	ui_skill1->GetComponent<ButtonSystem>()->SetImage(ui_skill1->GetComponent<ImageRenderer>());
	ui_skill1->GetComponent<ButtonSystem>()->Setfocused(ui_skill1_tip->GetComponent<ImageRenderer>());

	ui_skill2->AddComponent<ButtonSystem>();
	ui_skill2->GetComponent<ButtonSystem>()->SetImage(ui_skill2->GetComponent<ImageRenderer>());
	ui_skill2->GetComponent<ButtonSystem>()->Setfocused(ui_skill2_tip->GetComponent<ImageRenderer>());
	
	 ui_skill3->AddComponent<ButtonSystem>();
	 ui_skill3->GetComponent<ButtonSystem>()->SetImage(ui_skill3->GetComponent<ImageRenderer>());
	 ui_skill3->GetComponent<ButtonSystem>()->Setfocused(ui_skill3_tip->GetComponent<ImageRenderer>());
	
	 ui_skill4->AddComponent<ButtonSystem>();
	 ui_skill4->GetComponent<ButtonSystem>()->SetImage(ui_skill4->GetComponent<ImageRenderer>());
	 ui_skill4->GetComponent<ButtonSystem>()->Setfocused(ui_skill4_tip->GetComponent<ImageRenderer>());
	
	 potion->AddComponent<ButtonSystem>();
	 potion->GetComponent<ButtonSystem>()->SetImage(potion->GetComponent<ImageRenderer>());
	 potion->GetComponent<ButtonSystem>()->Setfocused(potion_tip->GetComponent<ImageRenderer>());
	
	 dash->AddComponent<ButtonSystem>();
	 dash->GetComponent<ButtonSystem>()->SetImage(dash->GetComponent<ImageRenderer>());
	 dash->GetComponent<ButtonSystem>()->Setfocused(dash_tip->GetComponent<ImageRenderer>());
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// player
	playerhp_background = scene.GetCurrentScene()->CreateObject("_playerhp_background");
	playerhp_background->SetParent(battleui_pack);
	playerhp_background->AddComponent<ImageRenderer>();
	playerhp_background->GetComponent<ImageRenderer>()->SetImage("ui/Gauge-middle.png");
	playerhp_background->GetComponent<ImageRenderer>()->SetPosition(689.0f, 950.f);
	playerhp_background->GetComponent<Transform>()->SetScale(32.14, 1.0f, 0.1f);

	_playerhp_bar_downGauge = scene.GetCurrentScene()->CreateObject("playerhp_bar");
	_playerhp_bar_downGauge->SetParent(battleui_pack);
	_playerhp_bar_downGauge->AddComponent<ImageRenderer>();
	_playerhp_bar_downGauge->GetComponent<ImageRenderer>()->SetImage("ui/GaugeFillY-middle.png");
	_playerhp_bar_downGauge->GetComponent<ImageRenderer>()->SetPosition(678.0f, 954.f);
	_playerhp_bar_downGauge->GetComponent<Transform>()->SetScale(_playerhpsize, 1.0f, 0.1f);

	_playerhp_bar = scene.GetCurrentScene()->CreateObject("playerhp_bar");
	_playerhp_bar->SetParent(battleui_pack);
	_playerhp_bar->AddComponent<ImageRenderer>();
	_playerhp_bar->GetComponent<ImageRenderer>()->SetImage("ui/GaugeFillR-middle.png");
	_playerhp_bar->GetComponent<ImageRenderer>()->SetPosition(678.0f, 954.f);
	_playerhp_bar->GetComponent<Transform>()->SetScale(_playerhpsize, 1.0f, 0.1f);

	playerhp_background1 = scene.GetCurrentScene()->CreateObject("backgroundL");
	playerhp_background1->SetParent(battleui_pack);
	playerhp_background1->AddComponent<ImageRenderer>();
	playerhp_background1->GetComponent<ImageRenderer>()->SetImage("ui/gauge-sideL_cover.png");
	playerhp_background1->GetComponent<ImageRenderer>()->SetPosition(674.0f, 950.0f);
	playerhp_background1->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

	playerhp_background2 = scene.GetCurrentScene()->CreateObject("backgroundR");
	playerhp_background2->SetParent(battleui_pack);
	playerhp_background2->AddComponent<ImageRenderer>();
	playerhp_background2->GetComponent<ImageRenderer>()->SetImage("ui/gauge-sideR_cover.png");
	playerhp_background2->GetComponent<ImageRenderer>()->SetPosition(1235.0f, 950.0f);
	playerhp_background2->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

	/// boss
	boss_background = scene.GetCurrentScene()->CreateObject("boss_background");
	boss_background->SetParent(booshpbar_pack);
	boss_background->AddComponent<ImageRenderer>();
	boss_background->GetComponent<ImageRenderer>()->SetImage("ui/freebox-middle.png");
	boss_background->GetComponent<ImageRenderer>()->SetPosition(454.0f, 6.0f);
	boss_background->GetComponent<Transform>()->SetScale(4.71f, 0.11f, 1.0f);

	boss_background1 = scene.GetCurrentScene()->CreateObject("boss_background1");
	boss_background1->SetParent(booshpbar_pack);
	boss_background1->AddComponent<ImageRenderer>();
	boss_background1->GetComponent<ImageRenderer>()->SetImage("ui/freebox-sideL.png");
	boss_background1->GetComponent<ImageRenderer>()->SetPosition(445.0f, 6.0f);
	boss_background1->GetComponent<Transform>()->SetScale(0.2f, 0.11f, 1.0f);

	boss_background2 = scene.GetCurrentScene()->CreateObject("boss_background2");
	boss_background2->SetParent(booshpbar_pack);
	boss_background2->AddComponent<ImageRenderer>();
	boss_background2->GetComponent<ImageRenderer>()->SetImage("ui/freebox-sideR.png");
	boss_background2->GetComponent<ImageRenderer>()->SetPosition(1452.0f, 6.0f);
	boss_background2->GetComponent<Transform>()->SetScale(0.2f, 0.11f, 1.0f);

	bosshp_background = scene.GetCurrentScene()->CreateObject("bosshp_background");
	bosshp_background->SetParent(booshpbar_pack);
	bosshp_background->AddComponent<ImageRenderer>();
	bosshp_background->GetComponent<ImageRenderer>()->SetImage("ui/Gauge-middle.png");
	bosshp_background->GetComponent<ImageRenderer>()->SetPosition(469.f, 11.f);
	bosshp_background->GetComponent<Transform>()->SetScale(57.0, 1.0f, 0.1f);

	_bosshp_bar_downGauge = scene.GetCurrentScene()->CreateObject("_bosshp_bar_downGauge");
	_bosshp_bar_downGauge->SetParent(booshpbar_pack);
	_bosshp_bar_downGauge->AddComponent<ImageRenderer>();
	_bosshp_bar_downGauge->GetComponent<ImageRenderer>()->SetImage("ui/GaugeFillY-middle.png");
	_bosshp_bar_downGauge->GetComponent<ImageRenderer>()->SetPosition(455.f, 14.f);
	_bosshp_bar_downGauge->GetComponent<Transform>()->SetScale(_bosshpsize, 1.0f, 1.f);

	_bosshp_bar = scene.GetCurrentScene()->CreateObject("_bosshp_bar");
	_bosshp_bar->SetParent(booshpbar_pack);
	_bosshp_bar->AddComponent<ImageRenderer>();
	_bosshp_bar->GetComponent<ImageRenderer>()->SetImage("ui/GaugeFillR-middle.png");
	_bosshp_bar->GetComponent<ImageRenderer>()->SetPosition(455.0f, 14.f);
	_bosshp_bar->GetComponent<Transform>()->SetScale(_bosshpsize, 1.0f, 1.f);

	bosshp_background1 = scene.GetCurrentScene()->CreateObject("bosshp_background1");
	bosshp_background1->SetParent(booshpbar_pack);
	bosshp_background1->AddComponent<ImageRenderer>();
	bosshp_background1->GetComponent<ImageRenderer>()->SetImage("ui/gauge-sideL_cover.png");
	bosshp_background1->GetComponent<ImageRenderer>()->SetPosition(454.0f, 11.0f);
	bosshp_background1->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

	bosshp_background2 = scene.GetCurrentScene()->CreateObject("bosshp_background2");
	bosshp_background2->SetParent(booshpbar_pack);
	bosshp_background2->AddComponent<ImageRenderer>();
	bosshp_background2->GetComponent<ImageRenderer>()->SetImage("ui/gauge-sideR_cover.png");
	bosshp_background2->GetComponent<ImageRenderer>()->SetPosition(1436.0f, 11.0f);
	bosshp_background2->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

	// 플레이어가 죽었을때 쓸 UI
	_died2 = scene.GetCurrentScene()->CreateObject("you_die2");
	_died2->SetParent(_battleuibox);
	_died2->AddComponent<ImageRenderer>();
	_died2->GetComponent<ImageRenderer>()->SetImage("ui/YouDied.png");
	_died2->GetComponent<ImageRenderer>()->SetPosition(0.0f, 300.0f);
	_died2->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);


	_battleuibox->SetActive(true);
	_ui_skill1_cool->SetActive(false);
	_ui_skill2_cool->SetActive(false);
	_ui_skill3_cool->SetActive(false);
	_ui_skill4_cool->SetActive(false);
	_dash_cool->SetActive(false);
	_potion_cool->SetActive(false);

	_died2->SetActive(false);

}

void KunrealEngine::BattleUIManager::Release()
{

}

void KunrealEngine::BattleUIManager::FixedUpdate()
{

}

void KunrealEngine::BattleUIManager::Update()
{
	auto  abill = _eventmanager->_playerAbill;
	auto& bossinfo = _eventmanager->_bossComp->GetBossInfo();
	auto& playerinfo = _eventmanager->_playerComp->GetPlayerData();


	// 이건 테스트 용으로 보스나 플레이어의 체력을 강제로 만질 수 있게 해둔 부분
	//if (InputSystem::GetInstance()->KeyDown(KEY::UP))
	//{
	//	bossinfo._hp += 10;
	//}
	//
	//else if (InputSystem::GetInstance()->KeyDown(KEY::DOWN))
	//{
	//	bossinfo._hp -= 10;
	//}

	//if (InputSystem::GetInstance()->KeyDown(KEY::PGUP))
	//{
	//	playerinfo._hp += 10;
	//}
	//
	//else if (InputSystem::GetInstance()->KeyDown(KEY::PGDOWN))
	//{
	//	playerinfo._hp -= 100;
	//}
	 
	// 체력을 받아와서 게이지에 반영하는 부분
	int bosshp = bossinfo._hp;
	int playerhp = playerinfo._hp;

	if (pre_bosshp != bosshp)
	{
		SetBossHpbar();
	}
	if (pre_playerhp != playerhp)
	{
		SetPlayerHpBar();
	}

	pre_bosshp = bosshp;
	pre_playerhp = playerhp;

	// 플레이어 체력이 0이 되었을 때?
	if (playerhp <= 0 && !_isdied)
	{
		ActiveDiedUI();
	}

	// 스킬 쿨타임 세팅
	if (abill->_isShotDetected)
	{
		SetSkillcool1();
	}

	if (abill->_isCircleDetected)
	{
		SetSkillcool2();
	}

	if (abill->_isLaserDetected)
	{
		SetSkillcool3();
	}

	if (abill->_isMeteorDetected)
	{
		SetSkillcool4();
	}
	
	if (InputSystem::GetInstance()->KeyDown(KEY::SPACE)
		&& GetCurrentScene()->GetGameObject("Player")->GetComponent<PlayerMove>()->GetisDashed())
	{
		Setdashcool();
	}

	if (InputSystem::GetInstance()->KeyDown(KEY::_1)
		&& !GetCurrentScene()->GetGameObject("Player")->GetComponent<PlayerAbility>()->_isPotionReady)
	{
		Setpotioncool();
	}

}

void KunrealEngine::BattleUIManager::LateUpdate()
{
}

void KunrealEngine::BattleUIManager::OnTriggerEnter()
{
}

void KunrealEngine::BattleUIManager::OnTriggerStay()
{
}

void KunrealEngine::BattleUIManager::OnTriggerExit()
{
}

void KunrealEngine::BattleUIManager::SetActive(bool active)
{

}

void KunrealEngine::BattleUIManager::SetBossHpbar()
{
	if (_eventmanager->_bossComp != nullptr)
	{
		float currenthp = _eventmanager->_bossComp->GetBossInfo()._hp;
		float maxhp = _eventmanager->_bossComp->GetBossInfo().GetMaxHP();
		float minhp = 0;
		float minhpbar = 0;

		booshp_targetscale = ((currenthp - minhp) / (maxhp - minhp)) * (_bosshpsize - minhpbar) + minhpbar;
		_bosshp_bar->GetComponent<Transform>()->SetScale(booshp_targetscale, 1.0f, 1.0f);
		_CoroutineIs(bossdowngauge)
		{
			//닼소식 체력 감소 연출을 어떻게 해야 할까
			// 일단 체력을 줄인다, 줄어든 체력의 앞에 다운게이지를 둔다, 다운 게이지의 스케일을 이전 체력 수치 만큼 체운다
			// 그리고 점점 스케일을 줄인다. 0이 될때까지. 
			// 줄어든 체력바의 포지션을 어떻게 잡을것이며, 이전 체력 기준으로 끝설정(스케일 조정)을 어케 할것 인가?
			// 다른 방법이 없을까
			// 일단 둘다 깐다. 빨간 체력은 바로 깐다, 노란 체력은 천천히 깐다.
			// 이러면 위치 구하고 스케일 구하고 할 필요가 없을 것 같은데 
			// 이걸로 함 해보자 

			auto  hpbarcontrol = this;
			auto  downGauge = _bosshp_bar_downGauge;	//GameObject
			float scale = downGauge->GetComponent<Transform>()->GetScale().x;
			float& targetscale = booshp_targetscale;
			float speed = 6.0f;


			while (targetscale - TimeManager::GetInstance().GetDeltaTime() * speed < scale)
			{
				downGauge->GetComponent<Transform>()->SetScale(scale, 1.0f, 1.0f);
				scale -= TimeManager::GetInstance().GetDeltaTime() * speed;
				Return_null;
			}
			targetscale = 0;
		};

		Startcoroutine(bossdowngauge);
	}
}

void KunrealEngine::BattleUIManager::SetPlayerHpBar()
{
	float currenthp = _eventmanager->_playerComp->GetPlayerData()._hp;
	float maxhp = _eventmanager->_playerComp->GetPlayerData()._maxhp;
	float minhp = 0;
	float minhpbar = 0;

	playerhp_targetscale = ((currenthp - minhp) / (maxhp - minhp)) * (_playerhpsize - minhpbar) + minhpbar;
	_playerhp_bar->GetComponent<Transform>()->SetScale(playerhp_targetscale, 1.0f, 1.0f);
	_CoroutineIs(playerdowngauge)
	{
		//auto  hpbarcontrol = this;
		auto  downGauge = _playerhp_bar_downGauge;	//GameObject
		float scale = downGauge->GetComponent<Transform>()->GetScale().x;
		float& targetscale = playerhp_targetscale;
		float speed = 3.0f;

		while (targetscale - TimeManager::GetInstance().GetDeltaTime() * speed < scale)
		{
			downGauge->GetComponent<Transform>()->SetScale(scale, 1.0f, 1.0f);
			scale -= TimeManager::GetInstance().GetDeltaTime() * speed;
			Return_null;
		}
		targetscale = 0;
	};

	Startcoroutine(playerdowngauge);
}

/// 1번 스킬
void KunrealEngine::BattleUIManager::SetSkillcool1()
{
	// 스킬 쿨다운을 보자.
	// 이놈은 스킬을 쓰면 까만 화면이 깔린다.
	// 쿨타임이 흐름에 따라 박스의 스케일이 줄어든다. 위든 아래든.
	// 그러면서 까만 화면이 점점 사라지고 스케일이 0이 되면 스킬쿨이 다 돌아간것. active를 끈다.
	_CoroutineIs(skillgauge1)
	{
		auto control = this;
		auto skillcool = _ui_skill1_cool;
		auto ability = _eventmanager->_playerAbill->_abilityContainer[0];

		// 스킬 쿨타임 동안 skillcool 오브젝트 활성화
		skillcool->SetActive(true);

		// 선형 보간을 위한 변수들
		// 여기서 보간 해야 할 부분은 쿨타임과 스케일이다.
		// 쿨타임은 받아온뒤 여기서 델타타임을 계속 빼준다 0이 될때까지
		// 스케일은 이 남은 쿨타임과 보간해서 현재 스케일을 구한다.

		float currenttime = ability->_cooldown;
		float maxcool = ability->_cooldown;
		float mincool = 0;

		float _coolsize = _skillcoolsize;
		float coolgauge = 0;

		float speed = 1.0f;
		
		while (0.1 <= currenttime)
		{
			currenttime -= TimeManager::GetInstance().GetDeltaTime();
			if (currenttime < 0.1)
			{
				currenttime == 0.1;
			}

			float nowscale = ((currenttime - mincool) / (maxcool - mincool)) * (_coolsize - coolgauge) + coolgauge;
			skillcool->GetComponent<Transform>()->SetScale(1.0f, nowscale, 1.0f);

			Return_null;
		}

		control->_eventmanager->_playerAbill->_isShotDetected = false;
		skillcool->SetActive(false);
		skillcool->GetComponent<Transform>()->SetScale(1.0f, _skillcoolsize, 1.0f);
	};
	Startcoroutine(skillgauge1);
}

/// 2번 스킬
void KunrealEngine::BattleUIManager::SetSkillcool2()
{
	_CoroutineIs(skillgauge2)
	{
		auto control = this;
		auto skillcool = _ui_skill2_cool;
		auto ability = _eventmanager->_playerAbill->_abilityContainer[1];

		// 스킬 쿨타임 동안 skillcool 오브젝트 활성화
		skillcool->SetActive(true);

		float currenttime = ability->_cooldown;
		float maxcool = ability->_cooldown;
		float mincool = 0;

		float _coolsize = _skillcoolsize;
		float coolgauge = 0;

		float speed = 1.0f;

		while (0.1 <= currenttime)
		{
			currenttime -= TimeManager::GetInstance().GetDeltaTime();
			if (currenttime < 0.1)
			{
				currenttime == 0.1;
			}

			float nowscale = ((currenttime - mincool) / (maxcool - mincool)) * (_coolsize - coolgauge) + coolgauge;
			skillcool->GetComponent<Transform>()->SetScale(1.0f, nowscale, 1.0f);

			Return_null;
		}

		control->_eventmanager->_playerAbill->_isCircleDetected = false;
		skillcool->SetActive(false);
		skillcool->GetComponent<Transform>()->SetScale(1.0f, _skillcoolsize, 1.0f);
	};

	Startcoroutine(skillgauge2);
}

/// 3번 스킬
void KunrealEngine::BattleUIManager::SetSkillcool3()
{
	_CoroutineIs(skillgauge3)
	{
		auto control = this;
		auto skillcool = _ui_skill3_cool;
		auto ability = _eventmanager->_playerAbill->_abilityContainer[2];

		// 스킬 쿨타임 동안 skillcool 오브젝트 활성화
		skillcool->SetActive(true);

		float currenttime = ability->_cooldown;
		float maxcool = ability->_cooldown;
		float mincool = 0;

		float _coolsize = _skillcoolsize;
		float coolgauge = 0;

		float speed = 1.0f;

		while (0.1 <= currenttime)
		{
			currenttime -= TimeManager::GetInstance().GetDeltaTime();
			if (currenttime < 0.1)
			{
				currenttime == 0.1;
			}

			float nowscale = ((currenttime - mincool) / (maxcool - mincool)) * (_coolsize - coolgauge) + coolgauge;
			skillcool->GetComponent<Transform>()->SetScale(1.0f, nowscale, 1.0f);

			Return_null;
		}

		control->_eventmanager->_playerAbill->_isLaserDetected = false;
		skillcool->SetActive(false);
		skillcool->GetComponent<Transform>()->SetScale(1.0f, _skillcoolsize, 1.0f);
	};

	Startcoroutine(skillgauge3);
}

/// 4번 스킬
void KunrealEngine::BattleUIManager::SetSkillcool4()
{
	_CoroutineIs(skillgauge4)
	{
		auto control = this;
		auto skillcool = _ui_skill4_cool;
		auto ability = _eventmanager->_playerAbill->_abilityContainer[3];

		// 스킬 쿨타임 동안 skillcool 오브젝트 활성화
		skillcool->SetActive(true);

		float currenttime = ability->_cooldown;
		float maxcool = ability->_cooldown;
		float mincool = 0;

		float _coolsize = 0.8;
		float coolgauge = 0;

		float speed = 1.0f;

		while (0.1 <= currenttime)
		{
			currenttime -= TimeManager::GetInstance().GetDeltaTime();
			if (currenttime < 0.1)
			{
				currenttime == 0.1;
			}

			float nowscale = ((currenttime - mincool) / (maxcool - mincool)) * (_coolsize - coolgauge) + coolgauge;
			skillcool->GetComponent<Transform>()->SetScale(1.0f, nowscale, 1.0f);

			Return_null;
		}

		control->_eventmanager->_playerAbill->_isMeteorDetected = false;
		skillcool->SetActive(false);
		skillcool->GetComponent<Transform>()->SetScale(0.8f, 0.8f, 1.0f);
	};

	Startcoroutine(skillgauge4);
}

void KunrealEngine::BattleUIManager::Setdashcool()
{
	_CoroutineIs(dashgauge)
	{
		auto control = this;
		auto skillcool = _dash_cool;
		float boxsize = 0.4f;
		//auto ability = _eventmanager->_playerAbill->_abilityContainer[3];

		// 스킬 쿨타임 동안 skillcool 오브젝트 활성화
		skillcool->SetActive(true);

		float currenttime = 8.0f;
		float maxcool = 8.0f;
		float mincool = 0;

		float _coolsize = boxsize;
		float coolgauge = 0;

		float speed = 1.0f;

		while (0.1 <= currenttime)
		{
			currenttime -= TimeManager::GetInstance().GetDeltaTime();
			if (currenttime < 0.1)
			{
				currenttime == 0.1;
			}

			float nowscale = ((currenttime - mincool) / (maxcool - mincool)) * (_coolsize - coolgauge) + coolgauge;
			skillcool->GetComponent<Transform>()->SetScale(boxsize, nowscale, 1.0f);

			Return_null;
		}

		control->_eventmanager->_playerAbill->_isMeteorDetected = false;
		skillcool->SetActive(false);
		skillcool->GetComponent<Transform>()->SetScale(boxsize, boxsize, 1.0f);
	};

	Startcoroutine(dashgauge);
}

void KunrealEngine::BattleUIManager::Setpotioncool()
{
	int num = _eventmanager->_playerAbill->_maxPotion;

	switch (num)
	{
	case 1:
		_potion_text->GetComponent<ImageRenderer>()->ChangeImage("ui/SkillText/Potion1.png");
		break;
	case 2:
		_potion_text->GetComponent<ImageRenderer>()->ChangeImage("ui/SkillText/Potion2.png");
		break;
	case 3:
		_potion_text->GetComponent<ImageRenderer>()->ChangeImage("ui/SkillText/Potion3.png");
		break;
	case 4:
		_potion_text->GetComponent<ImageRenderer>()->ChangeImage("ui/SkillText/Potion4.png");
		break;
	case 5:
		_potion_text->GetComponent<ImageRenderer>()->ChangeImage("ui/SkillText/Potion5.png");
		break;

		default:
			break;
	}

	_CoroutineIs(potiongauge)
	{
		auto control = this;
		auto skillcool = _potion_cool;
		auto ability = _eventmanager->_playerAbill;
		float boxsize = 0.4f;

		// 스킬 쿨타임 동안 skillcool 오브젝트 활성화
		skillcool->SetActive(true);

		float currenttime = ability->_potionCoolDown;
		float maxcool = ability->_potionCoolDown;
		float mincool = 0;                                                                                                                                                                                                                                                                                                                                                                                                                                                                   

		float _coolsize = boxsize;
		float coolgauge = 0;

		float speed = 1.0f;

		while (0.1 <= currenttime)
		{
			currenttime -= TimeManager::GetInstance().GetDeltaTime();
			if (currenttime < 0.1)
			{
				currenttime == 0.1;
			}

			float nowscale = ((currenttime - mincool) / (maxcool - mincool)) * (_coolsize - coolgauge) + coolgauge;
			skillcool->GetComponent<Transform>()->SetScale(boxsize, nowscale, 1.0f);

			Return_null;
		}

		control->_eventmanager->_playerAbill->_isMeteorDetected = false;
		skillcool->SetActive(false);
		skillcool->GetComponent<Transform>()->SetScale(boxsize, boxsize, 0.4f);
	};

	Startcoroutine(potiongauge);
}

void KunrealEngine::BattleUIManager::ActiveDiedUI()
{
	_CoroutineIs(diedcoro)
	{
		// UI연출? 일단 두가지로 분류
		// 1단계는 백UI가 뜬다. 매우 미묘한 시간차로 textui 가 뜬다
		// textui는 천천히 커진다 대충 백ui 보다 아슬아슬하게 커지면 될듯
		// 빛나야 하는가? 고민중
		auto uimanager = this;
		uimanager->_isdied = true;
		float delta = 0;
		float scale = 1.0f;

		uimanager->_died2->SetActive(true);
		uimanager->_died2->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);
		uimanager->_died2->GetComponent<Transform>()->SetPosition(0.0f, 300.0f, 1.0f);
		int sound = uimanager->_eventmanager->_player->GetComponent<SoundPlayer>()->FindIndex("Resources/sound/youDied.mp3");
		uimanager->_eventmanager->_player->GetComponent<SoundPlayer>()->Play(sound); 

		//int loop = 0;
		//while (loop < 50)
		//{
		//	scale += 0.005;
		//	DirectX::XMUINT2 preimgsize = uimanager->_died2->GetComponent<ImageRenderer>()->GetImageSize();
		//	
		//	Return_null;
		//
		//	DirectX::XMUINT2 imgsize = uimanager->_died2->GetComponent<ImageRenderer>()->GetImageSize();
		//	auto nowpos = uimanager->_died2->GetComponent<Transform>()->GetPosition();
		//
		//	float a = nowpos.x - (((float)imgsize.x - (float)preimgsize.x)/2);
		//	float b = nowpos.y - (((float)imgsize.y - (float)preimgsize.y)/2);
		//	GRAPHICS->DrawDebugText(300, 100, 20, "%f, %f", a, b);
		//
		//	uimanager->_died2->GetComponent<Transform>()->SetPosition(a, b, 1.0f);
		//	
		//
		//	loop++;
		//}

		Waitforsecond(3.0f);
		EventManager::GetInstance().ActivateFadeOutTrigger();
	};
	Startcoroutine(diedcoro);
}

void KunrealEngine::BattleUIManager::Resebattleuibar()
{
	SceneManager::GetInstance().GetScene("Main")->GetGameObject("boss_pack")
		->GetComponent<Transform>()->SetPosition(0.0f, -500.0f, 1.0f);
	SceneManager::GetInstance().GetScene("Main")->GetGameObject("battle_pack")
		->GetComponent<Transform>()->SetPosition(0.0f, 600.0f, 1.0f);
	
	_playerhp_bar->GetComponent<Transform>()->SetScale(_playerhpsize, 1.0f, 0.1f);
	_playerhp_bar_downGauge->GetComponent<Transform>()->SetScale(_playerhpsize, 1.0f, 0.1f);

	_bosshp_bar_downGauge->GetComponent<Transform>()->SetScale(_bosshpsize, 1.0f, 1.f);
	_bosshp_bar->GetComponent<Transform>()->SetScale(_bosshpsize, 1.0f, 1.f);
	_died2->GetComponent<ImageRenderer>()->SetScale(1.0f, 1.0f);
	_died2->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);
	_isdied = false;
}
