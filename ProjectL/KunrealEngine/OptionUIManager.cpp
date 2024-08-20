#include "OptionUIManager.h"
#include "ButtonSystem.h"
#include "SoundSystem.h"
#include "EventManager.h"

KunrealEngine::OptionUIManager::OptionUIManager()
{
}

KunrealEngine::OptionUIManager::~OptionUIManager()
{
}

void KunrealEngine::OptionUIManager::Initialize()
{
	scene = KunrealEngine::SceneManager::GetInstance().GetCurrentScene();
	KunrealEngine::GameObject* imagebackgroundmiddle;
	KunrealEngine::GameObject* imagebackgroundfill;
	KunrealEngine::GameObject* imagebackgroundL;
	KunrealEngine::GameObject* imagebackgroundR;

	KunrealEngine::GameObject* button_bgmon;
	KunrealEngine::GameObject* button_sfxon;

	KunrealEngine::GameObject* button_bgm;
	KunrealEngine::GameObject* button_sfx;

	KunrealEngine::GameObject* easy_text;
	KunrealEngine::GameObject* normal_text;
	KunrealEngine::GameObject* hard_text;
	KunrealEngine::GameObject* hardcore_text;

	KunrealEngine::GameObject* button_exit;
	KunrealEngine::GameObject* button_exit_focus;
	KunrealEngine::GameObject* button_exit_text;

	optionuibox = this->GetOwner();
	optionuibox->GetComponent<Transform>()->SetPosition(320.0f, 0.0f, 0.0f);
	optionuibox->AddComponent<SoundPlayer>();

	imagebackgroundfill = scene->CreateObject("background_f");
	imagebackgroundfill->SetParent(optionuibox);
	imagebackgroundfill->AddComponent<ImageRenderer>();
	imagebackgroundfill->GetComponent<ImageRenderer>()->SetImage("ui/freebox-middle.png");
	imagebackgroundfill->GetComponent<ImageRenderer>()->SetPosition(440.f, 75.f);
	imagebackgroundfill->GetComponent<Transform>()->SetScale(5.0f, 3.0f, 1.0f);

	imagebackgroundmiddle = scene->CreateObject("background_m");
	imagebackgroundmiddle->SetParent(optionuibox);
	imagebackgroundmiddle->AddComponent<ImageRenderer>();
	imagebackgroundmiddle->GetComponent<ImageRenderer>()->SetImage("ui/freebox-constmiddle.png");
	imagebackgroundmiddle->GetComponent<ImageRenderer>()->SetPosition(900.f, 75.f);
	imagebackgroundmiddle->GetComponent<Transform>()->SetScale(3.0f, 3.0f, 1.0f);


	imagebackgroundL = scene->CreateObject("background_L");
	imagebackgroundL->SetParent(optionuibox);
	imagebackgroundL->AddComponent<ImageRenderer>();
	imagebackgroundL->GetComponent<ImageRenderer>()->SetImage("ui/freebox-sideL.png");
	imagebackgroundL->GetComponent<ImageRenderer>()->SetPosition(305.f, 75.f);
	imagebackgroundL->GetComponent<Transform>()->SetScale(3.0f, 3.0f, 1.0f);

	imagebackgroundR = scene->CreateObject("background_R");
	imagebackgroundR->SetParent(optionuibox);
	imagebackgroundR->AddComponent<ImageRenderer>();
	imagebackgroundR->GetComponent<ImageRenderer>()->SetImage("ui/freebox-sideR.png");
	imagebackgroundR->GetComponent<ImageRenderer>()->SetPosition(1500.f,  75.f);
	imagebackgroundR->GetComponent<Transform>()->SetScale(3.0f, 3.0f, 1.0f);

	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	//BGM 사운드 조절
	InitializeVolumeButton(_bgms, &_bgm_step, DirectX::XMFLOAT2{ 700.0f, 350.0f });

	button_bgmon = scene->CreateObject("button_bgmon");
	button_bgmon->SetParent(optionuibox);
	button_bgmon->AddComponent<ImageRenderer>();
	button_bgmon->GetComponent<ImageRenderer>()->SetImage("ui/sound-on.png");
	button_bgmon->GetComponent<ImageRenderer>()->SetPosition(670.0f, 290.0f);
	button_bgmon->GetComponent<Transform>()->SetScale(0.7f, 0.7f, 0.7f);
	button_bgmon->AddComponent<ButtonSystem>();
	button_bgmon->GetComponent<ButtonSystem>()->SetImage(button_bgmon->GetComponent<ImageRenderer>());
	button_bgmon->GetComponent<ButtonSystem>()->SetButtonFunc([this]()
		{
			auto image = SceneManager::GetInstance().GetCurrentScene()->GetGameObject("button_bgmon")->GetComponent<ImageRenderer>();
			std::string path = "Resources/Textures/";
			if (image->GetImageName() == path + "ui/sound-on.png")
			{
				image->ChangeImage("ui/sound-off.png");
				_bgm_step = 0;
			}

			else if (image->GetImageName() == path + "ui/sound-off.png")
			{
				image->ChangeImage("ui/sound-on.png");
				_bgm_step = 1;
			}
		});

	button_bgm = scene->CreateObject("button_bgm");
	button_bgm->SetParent(optionuibox);
	button_bgm->AddComponent<ImageRenderer>();
	button_bgm->GetComponent<ImageRenderer>()->SetImage("ui/BGM_text.png");
	button_bgm->GetComponent<ImageRenderer>()->SetPosition(405.0f, 274.0f);
	button_bgm->GetComponent<Transform>()->SetScale(0.7f, 0.7f, 0.7f);

	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	//SFX 사운드 조절
	InitializeVolumeButton(_sfxs, &_sfx_step, DirectX::XMFLOAT2{ 700.0f, 500.0f });

	button_sfxon = scene->CreateObject("button_sfxon");
	button_sfxon->SetParent(optionuibox);
	button_sfxon->AddComponent<ImageRenderer>();
	button_sfxon->GetComponent<ImageRenderer>()->SetImage("ui/sound-on.png");
	button_sfxon->GetComponent<ImageRenderer>()->SetPosition(670.0f, 440.0f);
	button_sfxon->GetComponent<Transform>()->SetScale(0.7, 0.7, 0.7f);
	button_sfxon->AddComponent<ButtonSystem>();
	button_sfxon->GetComponent<ButtonSystem>()->SetImage(button_sfxon->GetComponent<ImageRenderer>());
	button_sfxon->GetComponent<ButtonSystem>()->SetButtonFunc([this]()
		{
			auto image = SceneManager::GetInstance().GetCurrentScene()->GetGameObject("button_sfxon")->GetComponent<ImageRenderer>();
			std::string path = "Resources/Textures/";
			if (image->GetImageName() == path + "ui/sound-on.png")
			{
				image->ChangeImage("ui/sound-off.png");
				_sfx_step = 0;
			}

			else if (image->GetImageName() == path + "ui/sound-off.png")
			{
				image->ChangeImage("ui/sound-on.png");
				_sfx_step = 1;
			}
		});

	button_sfx = scene->CreateObject("button_sfx");
	button_sfx->SetParent(optionuibox);
	button_sfx->AddComponent<ImageRenderer>();
	button_sfx->GetComponent<ImageRenderer>()->SetImage("ui/SFX_text.png");
	button_sfx->GetComponent<ImageRenderer>()->SetPosition(405.0f, 405.0f);
	button_sfx->GetComponent<Transform>()->SetScale(0.7, 0.7, 0.7f);

	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////

	button_check1 = scene->CreateObject("button_check1");
	button_check1->SetParent(optionuibox);
	button_check1->AddComponent<ImageRenderer>();
	button_check1->GetComponent<ImageRenderer>()->SetImage("ui/Check-false.png");
	button_check1->GetComponent<ImageRenderer>()->SetPosition(860.0f, 625.0f);
	button_check1->GetComponent<Transform>()->SetScale(2.0f, 2.0f, 2.0f);
	button_check1->AddComponent<ButtonSystem>();
	button_check1->GetComponent<ButtonSystem>()->SetImage(button_check1->GetComponent<ImageRenderer>());
	button_check1->GetComponent<ButtonSystem>()->SetButtonFunc([this]()
		{
			if (SceneManager::GetInstance().GetCurrentScene()->GetSceneName() == "Title")
			{
				this->DifficultyLevel = 0;
				this->setDifficulty();
			}
		});

	button_check2 = scene->CreateObject("button_check2");
	button_check2->SetParent(optionuibox);
	button_check2->AddComponent<ImageRenderer>();
	button_check2->GetComponent<ImageRenderer>()->SetImage("ui/Check-false.png");
	button_check2->GetComponent<ImageRenderer>()->SetPosition(1400.0f, 625.0f);
	button_check2->GetComponent<Transform>()->SetScale(2.0f, 2.0f, 2.0f);
	button_check2->AddComponent<ButtonSystem>();
	button_check2->GetComponent<ButtonSystem>()->SetImage(button_check2->GetComponent<ImageRenderer>());
	button_check2->GetComponent<ButtonSystem>()->SetButtonFunc([this]()
		{
			if (SceneManager::GetInstance().GetCurrentScene()->GetSceneName() == "Title")
			{
				this->DifficultyLevel = 1;
				this->setDifficulty();
			}
		});

	button_check3 = scene->CreateObject("button_check1");
	button_check3->SetParent(optionuibox);
	button_check3->AddComponent<ImageRenderer>();
	button_check3->GetComponent<ImageRenderer>()->SetImage("ui/Check-false.png");
	button_check3->GetComponent<ImageRenderer>()->SetPosition(860.0f, 725.0f);
	button_check3->GetComponent<Transform>()->SetScale(2.0f, 2.0f, 2.0f);
	button_check3->AddComponent<ButtonSystem>();
	button_check3->GetComponent<ButtonSystem>()->SetImage(button_check3->GetComponent<ImageRenderer>());
	button_check3->GetComponent<ButtonSystem>()->SetButtonFunc([this]()
		{
			if (SceneManager::GetInstance().GetCurrentScene()->GetSceneName() == "Title")
			{
				this->DifficultyLevel = 2;
				this->setDifficulty();
			}
		});

	easy_text = scene->CreateObject("fullscreen_text");
	easy_text->SetParent(optionuibox);
	easy_text->AddComponent<ImageRenderer>();
	easy_text->GetComponent<ImageRenderer>()->SetImage("ui/FullScreen.png");
	easy_text->GetComponent<ImageRenderer>()->SetPosition(430.0f, 560.0f);
	easy_text->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

	normal_text = scene->CreateObject("windowed_text");
	normal_text->SetParent(optionuibox);
	normal_text->AddComponent<ImageRenderer>();
	normal_text->GetComponent<ImageRenderer>()->SetImage("ui/Windowed.png");
	normal_text->GetComponent<ImageRenderer>()->SetPosition(950.0f, 560.0f);
	normal_text->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

	hard_text = scene->CreateObject("fullscreen_text");
	hard_text->SetParent(optionuibox);
	hard_text->AddComponent<ImageRenderer>();
	hard_text->GetComponent<ImageRenderer>()->SetImage("ui/HARDCORE.png");
	hard_text->GetComponent<ImageRenderer>()->SetPosition(430.0f, 660.0f);
	hard_text->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////

	button_exit = scene->CreateObject("button_exit");
	button_exit->SetParent(optionuibox);
	button_exit->AddComponent<ImageRenderer>();
	button_exit->GetComponent<ImageRenderer>()->SetImage("ui/button-short.png");
	button_exit->GetComponent<ImageRenderer>()->SetPosition(1468.f, 114.f);
	button_exit->GetComponent<Transform>()->SetScale(1.2f, 1.2f, 1.0f);
	button_exit->AddComponent<ButtonSystem>();
	button_exit->GetComponent<ButtonSystem>()->SetImage(button_exit->GetComponent<ImageRenderer>());
	button_exit->GetComponent<ButtonSystem>()->SetButtonFunc([this]()
		{
			SceneManager::GetInstance().GetCurrentScene()->GetGameObject("pauseuibox")->SetActive(true);
			this->optionuibox->SetActive(false);
		});

	button_exit_focus = scene->CreateObject("button_exit");
	button_exit_focus->SetParent(optionuibox);
	button_exit_focus->AddComponent<ImageRenderer>();
	button_exit_focus->GetComponent<ImageRenderer>()->SetImage("ui/button-short-focus.png");
	button_exit_focus->GetComponent<ImageRenderer>()->SetPosition(1468.f, 114.f);
	button_exit_focus->GetComponent<Transform>()->SetScale(1.2f, 1.2f, 1.0f);
	button_exit->GetComponent<ButtonSystem>()->Setfocused(button_exit_focus->GetComponent<ImageRenderer>());
	button_exit_focus = scene->CreateObject("button_exit");
	
	button_exit_focus->SetParent(optionuibox);
	button_exit_focus->AddComponent<ImageRenderer>();
	button_exit_focus->GetComponent<ImageRenderer>()->SetImage("ui/UiTexts/Xbutton.png");
	button_exit_focus->GetComponent<ImageRenderer>()->SetPosition(1468.f, 114.f);
	button_exit_focus->GetComponent<Transform>()->SetScale(1.2f, 1.2f, 1.0f);

	optionuibox->SetActive(false);
}

void KunrealEngine::OptionUIManager::Release()
{
}

void KunrealEngine::OptionUIManager::FixedUpdate()
{
}

void KunrealEngine::OptionUIManager::Update()
{
	static int prevbgm = -1;
	static int prevsfx = -1;

	if (prevbgm != _bgm_step)
	{
		for (int a = 0 ; a < _bgm_step ; a++)
		{
			_bgms[a]->GetComponent<ImageRenderer>()->ChangeImage("ui/volume-in.png");
		}

		for (int b = _bgm_step ; b < 10 ; b++)
		{
			_bgms[b]->GetComponent<ImageRenderer>()->ChangeImage("ui/volume-empty.png");
		}
	}

	if (prevsfx != _sfx_step)
	{
		for (int a = 0 ; a < _sfx_step; a++)
		{
			_sfxs[a]->GetComponent<ImageRenderer>()->ChangeImage("ui/volume-in.png");
		}

		for (int b = _sfx_step ; b < 10; b++)
		{
			_sfxs[b]->GetComponent<ImageRenderer>()->ChangeImage("ui/volume-empty.png");
		}
	}

	if (_bgm_step > 0 && prevbgm == 0)
	{
		scene = KunrealEngine::SceneManager::GetInstance().GetCurrentScene();
		scene->GetGameObject("button_bgmon")->GetComponent<ImageRenderer>()->ChangeImage("ui/sound-on.png");
	}

	if (_sfx_step > 0 && prevsfx == 0)
	{
		scene = KunrealEngine::SceneManager::GetInstance().GetCurrentScene();
		scene->GetGameObject("button_sfxon")->GetComponent<ImageRenderer>()->ChangeImage("ui/sound-on.png");
	}

	prevbgm = _bgm_step;
	prevsfx = _sfx_step;
}

void KunrealEngine::OptionUIManager::LateUpdate()
{
}

void KunrealEngine::OptionUIManager::OnTriggerEnter()
{
}

void KunrealEngine::OptionUIManager::OnTriggerStay()
{
}

void KunrealEngine::OptionUIManager::OnTriggerExit()
{
}

void KunrealEngine::OptionUIManager::SetActive(bool active)
{
}

///////////////////////////////////
///////////////////////////////////
///////////////////////////////////

void KunrealEngine::OptionUIManager::InitializeVolumeButton(std::vector<GameObject*>& vecs, int* steps, DirectX::XMFLOAT2 tlposition)
{
	float yscale = 10.0f;
	float xscale = 7.0f;

	for (int step = 1 ; step < 11 ; step++)    
	{
		KunrealEngine::GameObject* bgm;

		// 오브젝트 기본 세팅
		bgm = scene->CreateObject("button");
		vecs.push_back(bgm);

		bgm->SetParent(optionuibox);
		bgm->AddComponent<ImageRenderer>();
		bgm->GetComponent<ImageRenderer>()->SetImage("ui/volume-in.png");
		bgm->GetComponent<ImageRenderer>()->SetPosition(tlposition.x, tlposition.y);
		bgm->GetComponent<Transform>()->SetScale(xscale, yscale, 1.0f);
		bgm->AddComponent<ImageRenderer>();
		bgm->AddComponent<ButtonSystem>();
		bgm->GetComponent<ButtonSystem>()->SetImage(bgm->GetComponent<ImageRenderer>());
		bgm->GetComponent<ButtonSystem>()->SetButtonFunc([this, step, steps]()
			{
				Setvolumebutton(step, steps);
			});

		// 실 오브젝트 위치 세팅
		// Y포지션
		float blposition = bgm->GetComponent<ImageRenderer>()->GetImageSize().y + tlposition.y;	// 끝점을 찾아준다.

		float reyscale = ((float)step / 10) * yscale;											// 이미지의 크기를 계산
		bgm->GetComponent<Transform>()->SetScale(xscale, reyscale, 1.0f);		// 이미지의 크기를 지정
		blposition -= (bgm->GetComponent<ImageRenderer>()->GetImageSize().y * reyscale) + tlposition.y;
		float ypos = /*tlposition.y + */(tlposition.y + blposition);

		// X포지션
		float leftpos = bgm->GetComponent<ImageRenderer>()->GetImageSize().x;
		leftpos = ((leftpos* xscale) * step) + tlposition.x;

		//총합
		bgm->GetComponent<ImageRenderer>()->SetPosition(leftpos, ypos);
	}
}

void KunrealEngine::OptionUIManager::Setvolumebutton(int vol, int* type)
{
	*type = vol;

	if (type == &_bgm_step)
	{
		SoundSystem::GetInstance().SetvolumeGroup(SOUNDTYPE::BGM, vol);
	} 
	else if(type == &_sfx_step)
	{
		SoundSystem::GetInstance().SetvolumeGroup(SOUNDTYPE::SFX, vol);
	}
}

void KunrealEngine::OptionUIManager::setDifficulty()
{
	switch (DifficultyLevel)
	{
		case 0:
			button_check1->GetComponent<ImageRenderer>()->ChangeImage("ui/Check-true.png");
			button_check2->GetComponent<ImageRenderer>()->ChangeImage("ui/Check-false.png");
			button_check3->GetComponent<ImageRenderer>()->ChangeImage("ui/Check-false.png");
			EventManager::GetInstance().SetDifficulty(eDifficulty::eEasy);
			break;
		case 1:
			button_check1->GetComponent<ImageRenderer>()->ChangeImage("ui/Check-false.png");
			button_check2->GetComponent<ImageRenderer>()->ChangeImage("ui/Check-true.png");
			button_check3->GetComponent<ImageRenderer>()->ChangeImage("ui/Check-false.png");
			EventManager::GetInstance().SetDifficulty(eDifficulty::eNormal);
			break;
		case 2:
			button_check1->GetComponent<ImageRenderer>()->ChangeImage("ui/Check-false.png");
			button_check2->GetComponent<ImageRenderer>()->ChangeImage("ui/Check-false.png");
			button_check3->GetComponent<ImageRenderer>()->ChangeImage("ui/Check-true.png");
			EventManager::GetInstance().SetDifficulty(eDifficulty::eHardCore);
			break;
		default:
			break;
	}
}
