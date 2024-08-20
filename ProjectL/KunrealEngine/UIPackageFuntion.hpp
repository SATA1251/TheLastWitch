#pragma once
#include <windows.h>
#include "CommonHeader.h"
#include "KunrealAPI.h"
#include "Coroutine.h"
#include "ButtonSystem.h"

namespace KunrealEngine
{
	//일시정지 메뉴 UI를 불러오는 함수. UI들을 묶은 부모 오브젝트의 포인터를 반환합니다.
	static GameObject* MakeMenuUIPack()
	{
		KunrealEngine::SceneManager& scene = KunrealEngine::SceneManager::GetInstance();
		KunrealEngine::GameObject* pauseuibox;
		KunrealEngine::GameObject* button_option;
		KunrealEngine::GameObject* button_title;
		KunrealEngine::GameObject* button_quit;
		KunrealEngine::GameObject* button_exit;
		KunrealEngine::GameObject* buttonfocused_option;
		KunrealEngine::GameObject* buttonfocused_title;
		KunrealEngine::GameObject* buttonfocused_quit;
		KunrealEngine::GameObject* buttonfocused_exit;
		KunrealEngine::GameObject* Text_option;
		KunrealEngine::GameObject* Text_title;
		KunrealEngine::GameObject* Text_quit;
		KunrealEngine::GameObject* Text_exit;
		KunrealEngine::GameObject* imagebackground;
		KunrealEngine::GameObject* controltip;

		pauseuibox = scene.GetCurrentScene()->CreateObject("pauseuibox");
		pauseuibox->GetComponent<Transform>()->SetPosition(320.0f, 0.0f, 0.0f);

		imagebackground = scene.GetCurrentScene()->CreateObject("MenuBackground");
		imagebackground->SetParent(pauseuibox);
		imagebackground->AddComponent<ImageRenderer>();
		imagebackground->GetComponent<ImageRenderer>()->SetImage("ui/MenuBackground.png");
		imagebackground->GetComponent<ImageRenderer>()->SetPosition(725.f, 207.f);
		imagebackground->GetComponent<Transform>()->SetScale(1.5f, 1.5f, 1.5f);

		// 옵션 버튼
		button_option = scene.GetCurrentScene()->CreateObject("button_resume");
		button_option->SetParent(pauseuibox);
		button_option->AddComponent<ImageRenderer>();
		//button_resume->GetComponent<ImageRenderer>().
		button_option->GetComponent<ImageRenderer>()->SetImage("ui/button-long.png");
		button_option->GetComponent<ImageRenderer>()->SetPosition(800.0f, 340.0f);
		button_option->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

		buttonfocused_option = scene.GetCurrentScene()->CreateObject("button_resume_focus");
		buttonfocused_option->SetParent(pauseuibox);
		buttonfocused_option->AddComponent<ImageRenderer>();
		buttonfocused_option->GetComponent<ImageRenderer>()->SetImage("ui/button-long-focus.png");
		buttonfocused_option->GetComponent<ImageRenderer>()->SetPosition(800.0f, 340.0f);
		buttonfocused_option->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

		Text_option = scene.GetCurrentScene()->CreateObject("button_text_option");
		Text_option->SetParent(pauseuibox);
		Text_option->AddComponent<ImageRenderer>();
		Text_option->GetComponent<ImageRenderer>()->SetImage("ui/UiTexts/Option.png");
		Text_option->GetComponent<ImageRenderer>()->SetPosition(800.0f, 340.0f);
		Text_option->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

		button_option->AddComponent<ButtonSystem>();
		button_option->GetComponent<ButtonSystem>()->SetImage(button_option->GetComponent<ImageRenderer>());
		button_option->GetComponent<ButtonSystem>()->Setfocused(buttonfocused_option->GetComponent<ImageRenderer>());
		button_option->GetComponent<ButtonSystem>()->SetButtonFunc([pauseuibox]()
			{
				KunrealEngine::SceneManager::GetInstance().GetCurrentScene()->GetGameObject("Option")->SetActive(true);
				pauseuibox->SetActive(false);
			});


		//타이틀로 돌아가기
		button_title = scene.GetCurrentScene()->CreateObject("button_title");
		button_title->SetParent(pauseuibox);
		button_title->AddComponent<ImageRenderer>();
		button_title->GetComponent<ImageRenderer>()->SetImage("ui/button-long.png");
		button_title->GetComponent<ImageRenderer>()->SetPosition(800.0f, 500.0f);
		button_title->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

		buttonfocused_title = scene.GetCurrentScene()->CreateObject("button_title_focus");
		buttonfocused_title->SetParent(pauseuibox);
		buttonfocused_title->AddComponent<ImageRenderer>();
		buttonfocused_title->GetComponent<ImageRenderer>()->SetImage("ui/button-long-focus.png");
		buttonfocused_title->GetComponent<ImageRenderer>()->SetPosition(800.0f, 500.0f);
		buttonfocused_title->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

		Text_title = scene.GetCurrentScene()->CreateObject("button_title_text");
		Text_title->SetParent(pauseuibox);
		Text_title->AddComponent<ImageRenderer>();
		Text_title->GetComponent<ImageRenderer>()->SetImage("ui/UiTexts/ReturnTitle.png");
		Text_title->GetComponent<ImageRenderer>()->SetPosition(800.0f, 500.0f);
		Text_title->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

		button_title->AddComponent<ButtonSystem>();
		button_title->GetComponent<ButtonSystem>()->SetImage(button_title->GetComponent<ImageRenderer>());
		button_title->GetComponent<ButtonSystem>()->Setfocused(buttonfocused_title->GetComponent<ImageRenderer>());
		button_title->GetComponent<ButtonSystem>()->SetButtonFunc([pauseuibox]()
			{
				pauseuibox->SetActive(false);
				if (KunrealEngine::SceneManager::GetInstance().GetCurrentScene()->GetSceneName() == "Title")
				{
					GameObject* obj = KunrealEngine::SceneManager::GetInstance().GetCurrentScene()->GetGameObject("controltip");
					obj->SetActive(true);
				}

				if (KunrealEngine::SceneManager::GetInstance().GetCurrentScene()->GetSceneName() == "Main")
				{
					//타이틀 씬으로 전환되는 코드
					//KunrealEngine::SceneManager::GetInstance().ChangeScene("Title");	
				}
			});


		// 게임 종료
		button_quit = scene.GetCurrentScene()->CreateObject("button_Quit");
		button_quit->SetParent(pauseuibox);
		button_quit->AddComponent<ImageRenderer>();
		button_quit->GetComponent<ImageRenderer>()->SetImage("ui/button-long.png");
		button_quit->GetComponent<ImageRenderer>()->SetPosition(800.0f, 660.0f);
		button_quit->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

		buttonfocused_quit = scene.GetCurrentScene()->CreateObject("button_Quit_focus");
		buttonfocused_quit->SetParent(pauseuibox);
		buttonfocused_quit->AddComponent<ImageRenderer>();
		buttonfocused_quit->GetComponent<ImageRenderer>()->SetImage("ui/button-long-focus.png");
		buttonfocused_quit->GetComponent<ImageRenderer>()->SetPosition(800.0f, 660.0f);
		buttonfocused_quit->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

		Text_quit = scene.GetCurrentScene()->CreateObject("button_Quit_text");
		Text_quit->SetParent(pauseuibox);
		Text_quit->AddComponent<ImageRenderer>();
		Text_quit->GetComponent<ImageRenderer>()->SetImage("ui/UiTexts/QuitGame.png");
		Text_quit->GetComponent<ImageRenderer>()->SetPosition(800.0f, 660.0f);
		Text_quit->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

		button_quit->AddComponent<ButtonSystem>();
		button_quit->GetComponent<ButtonSystem>()->SetImage(button_quit->GetComponent<ImageRenderer>());
		button_quit->GetComponent<ButtonSystem>()->Setfocused(buttonfocused_quit->GetComponent<ImageRenderer>());
		button_quit->GetComponent<ButtonSystem>()->SetButtonFunc([pauseuibox]()
			{
				pauseuibox->SetActive(false);
				//
				//  게임 종료되는 코드
				//
				//DestroyWindow();
				PostQuitMessage(0);
			});


		// 메뉴 나가기
		button_exit = scene.GetCurrentScene()->CreateObject("button_exit");
		button_exit->SetParent(pauseuibox);
		button_exit->AddComponent<ImageRenderer>();
		button_exit->GetComponent<ImageRenderer>()->SetImage("ui/button-short.png");
		button_exit->GetComponent<ImageRenderer>()->SetPosition(1095.f, 200.f);
		button_exit->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

		buttonfocused_exit = scene.GetCurrentScene()->CreateObject("button_exit_focus");
		buttonfocused_exit->SetParent(pauseuibox);
		buttonfocused_exit->AddComponent<ImageRenderer>();
		buttonfocused_exit->GetComponent<ImageRenderer>()->SetImage("ui/button-short-focus.png");
		buttonfocused_exit->GetComponent<ImageRenderer>()->SetPosition(1095.f, 200.f);
		buttonfocused_exit->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

		Text_exit = scene.GetCurrentScene()->CreateObject("button_exit_text");
		Text_exit->SetParent(pauseuibox);
		Text_exit->AddComponent<ImageRenderer>();
		Text_exit->GetComponent<ImageRenderer>()->SetImage("ui/UiTexts/Xbutton.png");
		Text_exit->GetComponent<ImageRenderer>()->SetPosition(1095.f, 200.f);
		Text_exit->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

		button_exit->AddComponent<ButtonSystem>();
		button_exit->GetComponent<ButtonSystem>()->SetImage(button_exit->GetComponent<ImageRenderer>());
		button_exit->GetComponent<ButtonSystem>()->Setfocused(buttonfocused_exit->GetComponent<ImageRenderer>());
		button_exit->GetComponent<ButtonSystem>()->SetButtonFunc([pauseuibox]()
			{
				pauseuibox->SetActive(false);
			});


		controltip = scene.GetCurrentScene()->CreateObject("controltip");
		controltip->SetParent(pauseuibox);
		controltip->AddComponent<ImageRenderer>();
		controltip->GetComponent<ImageRenderer>()->SetImage("ui/SkillExplain/control_tip.png");
		controltip->GetComponent<ImageRenderer>()->SetPosition(320.f, 180.f);
		controltip->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);
		controltip->AddComponent<ButtonSystem>();
		controltip->GetComponent<ButtonSystem>()->SetImage(controltip->GetComponent<ImageRenderer>());
		controltip->GetComponent<ButtonSystem>()->SetButtonFunc([pauseuibox,controltip]()
			{
				static bool first = true;
				if (first)
				{
					first = false;
				}

				else if (!first)
				{
					controltip->SetActive(false);
					pauseuibox->SetActive(true);
					first = true;
				}
			});

		controltip->SetActive(false);

		pauseuibox->SetActive(false);
		return pauseuibox;
	}

	static void ResetMenuUIPack(GameObject* object ,std::string nowscene, std::string targetscene)
	{
		// 일단 사용할 씬을 받자.
		Scene* now = SceneManager::GetInstance().GetScene(nowscene);
		Scene* target = SceneManager::GetInstance().GetScene(targetscene);
		// 그리고 나(파라미터)를 바꿔준다.
		object->MoveToScene(target);

		auto childs = object->GetChilds();
		for (auto child : childs)
		{
			child->MoveToScene(target);
		}
	}

	//일시정지 메뉴 UI를 불러오는 함수. UI들을 묶은 부모 오브젝트의 포인터를 반환합니다.
	static GameObject* MakeTitleUIPack()
	{
		KunrealEngine::SceneManager& scene = KunrealEngine::SceneManager::GetInstance();
		KunrealEngine::GameObject* titleuibox;				// 부모용 빈 오브젝트
		KunrealEngine::GameObject* title_image;				// 게임 로고
		KunrealEngine::GameObject* button_Start;			// 게임시작 버튼(화면 전체)
		KunrealEngine::GameObject* button_quit;				// 게임종료 버튼
		KunrealEngine::GameObject* button_quit_focus;		// 게임종료 버튼 focus시 객체/
		KunrealEngine::GameObject* button_quit_text;		// 게임종료 버튼 focus시 객체/

		titleuibox = scene.GetCurrentScene()->CreateObject("titleuibox");
		titleuibox->_autoAwake = true;
		titleuibox->GetComponent<Transform>()->SetPosition(320.0f, 0.0f, 0.0f);

		title_image = scene.GetCurrentScene()->CreateObject("Title_Image");
		title_image->SetParent(titleuibox);
		title_image->_autoAwake;
		title_image->AddComponent<ImageRenderer>();
		title_image->GetComponent<ImageRenderer>()->SetImage("ui/title_logo.png");
		title_image->GetComponent<ImageRenderer>()->SetPosition(525.f, 20.f);
		title_image->GetComponent<Transform>()->SetScale(0.4f, 0.4f, 1.0f);


		// 게임 시작
		// 페이드 아웃 된다던가, 위나 아래로 올라가며 사라진다던가 하는 연출은 전달받은게 없으므로 일단은 disable
		button_Start = scene.GetCurrentScene()->CreateObject("button_Start");
		button_Start->_autoAwake = true;
		button_Start->SetParent(titleuibox);
		button_Start->AddComponent<ImageRenderer>();
		button_Start->GetComponent<ImageRenderer>()->SetImage("ui/title.png");
		button_Start->GetComponent<ImageRenderer>()->SetPosition(0.0f, 400.0f);
		button_Start->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);
		button_Start->AddComponent<ButtonSystem>();
		button_Start->GetComponent<ButtonSystem>()->SetImage(button_Start->GetComponent<ImageRenderer>());
		button_Start->GetComponent<ButtonSystem>()->SetButtonFunc([titleuibox]()
			{
				titleuibox->GetChild("button_Start")->GetComponent<ButtonSystem>()->SetActive(false);
				Coroutine_staticFunc(disable_staritng, titleuibox)
				{
					auto titleobject = titleuibox;
					while (1)
					{
						Transform* trans = titleobject->GetChild("button_Start")->GetComponent<Transform>();
						float		x = trans->GetPosition().x;
						float		y = trans->GetPosition().y;
						float		z = trans->GetPosition().z;
						trans->SetPosition(x, y + 20, z);

						Transform* trans2 = titleobject->GetChild("Title_Image")->GetComponent<Transform>();
						x = trans2->GetPosition().x;
						y = trans2->GetPosition().y;
						z = trans2->GetPosition().z;
						trans2->SetPosition(x, y - 20, z);

						if (trans->GetPosition().y > 1080 && trans2->GetPosition().y < 0)
						{
							titleobject->SetActive(false);
							//titleobject->GetChild("Title_Image")->GetComponent<ImageRenderer>()->SetPosition(525.f, 20.f);
							//KunrealEngine::SceneManager::GetInstance().ChangeScene("MAIN_GAME");
							break;
						}
						Return_null;
					}
				};
				Startcoroutine(disable_staritng);
			});

		// 게임 종료
		button_quit_focus = scene.GetCurrentScene()->CreateObject("button_Quit");
		button_quit_focus->SetParent(titleuibox);
		button_quit_focus->AddComponent<ImageRenderer>();
		button_quit_focus->GetComponent<ImageRenderer>()->SetImage("ui/button-short-focus.png");
		button_quit_focus->GetComponent<ImageRenderer>()->SetPosition(1814.0f * 1.33333f, 975.0f * 1.33333f);
		button_quit_focus->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);


		button_quit = scene.GetCurrentScene()->CreateObject("button_Quit");
		button_quit->SetParent(titleuibox);
		button_quit->AddComponent<ImageRenderer>();
		button_quit->GetComponent<ImageRenderer>()->SetImage("ui/button-short.png");
		button_quit->GetComponent<ImageRenderer>()->SetPosition(1814.0f * 1.33333f, 975.0f * 1.33333f);
		button_quit->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

		button_quit_text = scene.GetCurrentScene()->CreateObject("button_text");
		button_quit_text->SetParent(titleuibox);
		button_quit_text->AddComponent<ImageRenderer>();
		button_quit_text->GetComponent<ImageRenderer>()->SetImage("ui/UiTexts/Xbutton.png");
		button_quit_text->GetComponent<ImageRenderer>()->SetPosition(1814.0f * 1.33333f, 975.0f * 1.33333f);
		button_quit_text->GetComponent<Transform>()->SetScale(1.0f, 1.0f, 1.0f);

		button_quit->AddComponent<ButtonSystem>();
		button_quit->GetComponent<ButtonSystem>()->SetImage(button_quit->GetComponent<ImageRenderer>());
		button_quit->GetComponent<ButtonSystem>()->Setfocused(button_quit_focus->GetComponent<ImageRenderer>());
		button_quit->GetComponent<ButtonSystem>()->SetButtonFunc([titleuibox]()
			{
				titleuibox->SetActive(false);
				// 
				//  게임 종료되는 코드
				//
				//DestroyWindow();
				PostQuitMessage(0);
			});

		titleuibox->SetActive(true);

		return titleuibox;
	}
}