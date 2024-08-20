#include "EngineCore.h"
#include <DirectXMath.h>
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "TimeManager.h"
#include "GraphicsSystem.h"
#include "PhysicsSystem.h"
#include "InputSystem.h"
#include "SoundSystem.h"
#include "Navigation.h"
#include "ButtonSystem.h"
#include "EventManager.h"
#include "ComponentHeaders.h"
#include "UIPackageFuntion.hpp"
#include "MainMapParticlePackage.hpp"
#include "ToolBox.h"
#include "TimeManager.h"
#include "Animator.h"

KunrealEngine::SceneManager& sceneInstance = KunrealEngine::SceneManager::GetInstance();
KunrealEngine::TimeManager& timeInstance = KunrealEngine::TimeManager::GetInstance();
KunrealEngine::InputSystem* inputInstance = KunrealEngine::InputSystem::GetInstance();
KunrealEngine::SoundSystem& soundInstance = KunrealEngine::SoundSystem::GetInstance();
KunrealEngine::Navigation& navigationInstance = KunrealEngine::Navigation::GetInstance();
KunrealEngine::EventManager& eventInstance = KunrealEngine::EventManager::GetInstance();

KunrealEngine::GameObject* player;
KunrealEngine::GameObject* kamen;
KunrealEngine::GameObject* spider;
KunrealEngine::GameObject* zeolight;
KunrealEngine::GameObject* bossMap;
KunrealEngine::GameObject* tree2;
KunrealEngine::GameObject* tree3;
KunrealEngine::GameObject* tree4;
KunrealEngine::GameObject* titleRock1;
KunrealEngine::GameObject* titleRock2;
KunrealEngine::GameObject* titleRock3;
KunrealEngine::GameObject* floatingObj;
KunrealEngine::GameObject* titleBoss;

// Ending
KunrealEngine::GameObject* endingBoss;
KunrealEngine::GameObject* endingRock;
KunrealEngine::GameObject* endingPlayer;

KunrealEngine::GameObject* Title_ui_box;	// 타이틀UI
KunrealEngine::GameObject* pause_ui_box;	// 일시정지
KunrealEngine::GameObject* option_ui_box;	// 일시정지
KunrealEngine::GameObject* battle_ui_box;	// 전투UI

KunrealEngine::GameObject* particleBezierTest1;
KunrealEngine::GameObject* particleBezierTest2;
KunrealEngine::GameObject* particleBezierTest3;

KunrealEngine::GameObject* testCamera;

DirectX::XMFLOAT3 targetPos;
DirectX::XMFLOAT3 p0 = { 0, 0, 0 };
DirectX::XMFLOAT3 p1 = { 0, 0, 0 };
DirectX::XMFLOAT3 p2 = { 0, 0, 0 };
DirectX::XMFLOAT3 p3 = { 0, 0, 0 };

DirectX::XMFLOAT3 titleCameraStart = { -155.f, 135.f, -45.13f };
DirectX::XMFLOAT3 titleCameraEnd = { -155.f, 100.f, -148.13f };
DirectX::XMFLOAT3 titleCameraRotStart = { -137.0f, 0.f, 0.f };
DirectX::XMFLOAT3 titleCameraRotEnd = { -180.f, 0.f, 0.f };

static float g_rageOfLee = 0.0f;
static float g_test = 0.0f;

KunrealEngine::EngineCore::EngineCore()
	:_gInterface(nullptr), _isEditor(true), _timeCount(0), _timeCountPlayerR(0), _isSettingTimer(false), _isBezierStartSetting(false), _isBezierTeleportSetting(false), _screenWidth(2560.0f), _screenHeight(1440.0f), _endingCamera(nullptr), _endingCameraAfter(nullptr)
{

}

KunrealEngine::EngineCore::~EngineCore()
{

}

void KunrealEngine::EngineCore::Initialize(HWND hwnd, HINSTANCE hInstance, int screenWidth, int screenHeight)
{
	// Initailize
	sceneInstance.Initialize();
	timeInstance.Initialize();
	GraphicsSystem::GetInstance().Initialize(hwnd, screenWidth, screenHeight);

	this->_screenWidth = (float)screenWidth;
	this->_screenHeight = (float)screenHeight;

	GameObject* meshTest = sceneInstance.GetCurrentScene()->CreateObject("meshTest");
	meshTest->AddComponent<MeshRenderer>();
	meshTest->GetComponent<MeshRenderer>()->SetMeshObject("cylinder/cylinder");
	meshTest->GetComponent<MeshRenderer>()->ChangeMeshObject("Blade/Blade");
	sceneInstance.GetCurrentScene()->DeleteGameObject(meshTest);

	PhysicsSystem::GetInstance().Initialize();
	inputInstance->Initialize(hInstance, hwnd, screenHeight, screenWidth);
	soundInstance.Initialize(hwnd);

	navigationInstance.Initialize();

	//// cube map test
	GRAPHICS->CreateCubeMap("BossMap", "Texture6.dds", true);
	auto list = GRAPHICS->GetCubeMapList();
	//GRAPHICS->SetMainCubeMap(list.back());

	sceneInstance.CreateScene("ParticleTest");

	_timeCount = 0.0f;
	_timeCountPlayerR = 0.0f;
	_isSettingTimer = false;
	_isBezierStartSetting = false;
	_isBezierTeleportSetting = false;

	// scene 만드는
	PlayGround();
	CreateEndingScene();
	CreateTitleScene();
	EventManager::GetInstance().CreateFadeObject();
}

void KunrealEngine::EngineCore::Release()
{
	sceneInstance.Release();
	timeInstance.Release();
	KunrealEngine::GraphicsSystem::GetInstance().Release();
	/// 나머지들 release 할 것들 추가
}

void KunrealEngine::EngineCore::FixedUpdate()
{
	PhysicsSystem::GetInstance().FixedUpdate();
	sceneInstance.FixedUpdateScene(sceneInstance.GetCurrentScene());
}

bool moveTo = true;


void KunrealEngine::EngineCore::Update()
{
	//UpdateParticleTest();

	inputInstance->Update(GetDeltaTime());
	sceneInstance.UpdateScene(sceneInstance.GetCurrentScene());
	CheckMousePosition();

	GRAPHICS->SetDeltaTime(TimeManager::GetInstance().GetDeltaTime());

	navigationInstance.HandleUpdate(TimeManager::GetInstance().GetDeltaTime());
	soundInstance.Update();

	//if (inputInstance->KeyDown(KEY::PERIOD))
	//{
	//	EventManager::GetInstance()._iscamfollow = EventManager::GetInstance()._iscamfollow ? false : true;
	//}
	//GRAPHICS->DrawDebugText(100, 100, 20, "FPS : %.2f", 1 / TimeManager::GetInstance().GetDeltaTime());

	inputInstance->GetMousePosition(_ingameMouseX, _ingameMouseY);

	if (sceneInstance.GetCurrentScene()->GetSceneName() == "Title")
	{
		FloatingY(titleRock1, 5);
		FloatingY(titleRock2, 3);
		FloatingY(titleRock3, 4);
		FloatingY(floatingObj, 6);

		ShiveringLight(titleRock1);
		ShiveringLight(titleRock2);
		ShiveringLight(titleRock3);
		titleBoss->GetComponent<Animator>()->Play("titleIdle", 0.5f, true);
		TitleMapParticle();
		GameObject* soundManager = sceneInstance.GetCurrentScene()->GetGameObject("TitleSceneSound");
		SoundPlayer* soundComp = soundManager->GetComponent<SoundPlayer>();
		int titlebgm1 = soundComp->FindIndex("Resources/Sound/TitleMap.mp3");
		soundComp->Play(titlebgm1);
		_bossSoundComp->Stop(0);

		titleUIPosY = GetCurrentScene()->GetGameObject("Title_Image")->GetComponent<Transform>()->GetPosition().y;
		if (titleUIPosY <= -680.0f)
		{
			SmoothTransition(titleCameraStart, titleCameraRotStart, titleCameraEnd, titleCameraRotEnd, 5.0f, TimeManager::GetInstance().GetDeltaTime());
		}


		if (inputInstance->KeyInput(KEY::NUM1))
		{
			EventManager::GetInstance().SetDifficulty(eDifficulty::eEasy);
		}
		else if (inputInstance->KeyInput(KEY::NUM2))
		{
			EventManager::GetInstance().SetDifficulty(eDifficulty::eNormal);
		}
		else if (inputInstance->KeyInput(KEY::NUM3))
		{
			EventManager::GetInstance().SetDifficulty(eDifficulty::eHardCore);
		}
	}
	else if (sceneInstance.GetCurrentScene()->GetSceneName() == "Main")
	{
		for (auto& mapParticleList : _mapParticleList)
		{
			mapParticleList->GetComponent<Particle>()->SetActive(true);
		}
		_bossSoundComp->Play(0);
	}
	else if (sceneInstance.GetCurrentScene()->GetSceneName() == "Ending")
	{
		_bossSoundComp->Stop(0);

		EndingSceneUpdate();
	}

	///// 지워제발나중에꼭기억해
	//if (sceneInstance.GetCurrentScene()->GetSceneName() == "Main" && inputInstance->KeyDown(KEY::X))
	//{
	//	EventManager::GetInstance().ActivateEndingSceneTrigger();
	//}
	/////

	MoveToMain();
	Updatecoroutine();
}

void KunrealEngine::EngineCore::LateUpdate()
{
	sceneInstance.LateUpdateScene(sceneInstance.GetCurrentScene());
	GRAPHICS->Update();	// 그래픽스쪽 카메라 업데이트 관련

	PhysicsSystem::GetInstance().PlayerForceUpdate();
}

void KunrealEngine::EngineCore::UpdateAll()
{
	timeInstance.Update();
	eventInstance.Update();
	this->FixedUpdate();
	this->Update();
	this->LateUpdate();
	this->Render();
}

void KunrealEngine::EngineCore::Render()
{
	GRAPHICS->Render();
}

void KunrealEngine::EngineCore::SetEditorMousePos(POINT position)
{
	_editorMousepos = position;
	return;
}

void KunrealEngine::EngineCore::PlayGround()
{
	sceneInstance.ChangeScene("Main");

	navigationInstance.HandleBuild(0, "bossmap.obj");
	navigationInstance.HandleBuild(1, "bossmap.obj");

	// Camera
	DirectX::XMFLOAT3 cameraPos = { 0.0f, 0.0f, 1.0f };
	// KunrealEngine::KunrealMath::Float3 cameraPos = { 40.0f, 2.0f, -30.0f };
	DirectX::XMFLOAT3 targetPos = { 0.0f, 0.0f, 0.0f };
	testCamera = sceneInstance.GetCurrentScene()->CreateObject("testCamera");
	testCamera->AddComponent<Camera>();
	testCamera->GetComponent<Camera>()->SetCameraPosition(cameraPos.x, cameraPos.y, cameraPos.z);
	testCamera->GetComponent<Camera>()->SetTargetPosition(targetPos.x, targetPos.y, targetPos.z);
	//testCamera->GetComponent<Camera>()->RotateCamera(5.f, 0.f);
	testCamera->GetComponent<Camera>()->SetMainCamera();

	testCamera->GetComponent<Transform>()->SetPosition({ 0.0f, 120.0f, -130.0f });
	testCamera->GetComponent<Transform>()->SetRotation(-43.f, 180.f, 0.f);
	testCamera->_autoAwake = true;

	EventManager::GetInstance().SetCamera("testCamera");
	
	// Player
	player = sceneInstance.GetCurrentScene()->CreateObject("Player");
	player->AddComponent<Player>();
	player->AddComponent<Light>();
	DirectX::XMFLOAT4 playerDiffuse = { 0.3f, 0.3f, 0.3f, 0.3f };
	DirectX::XMFLOAT4 playerAmbient = { 0.2f, 0.2f, 0.2f, 0.2f };
	DirectX::XMFLOAT4 playerSpecular = { 1.0f, 1.0f, 1.0f, 1.0f };
	player->GetComponent<Light>()->CreatePointLight(playerAmbient, playerDiffuse, playerSpecular, 300.f, 16.0f);
	player->GetComponent<Light>()->SetOffSet(0.0f, 15.f, -5.0f);
	player->_autoAwake = true;



	//// cube map test
	GRAPHICS->CreateCubeMap("test", "sunsetcube1024.dds", true);
	auto list = GRAPHICS->GetCubeMapList();
	GRAPHICS->SetMainCubeMap(list.back());

	// light test
	DirectX::XMFLOAT4 diffuse = { 0.9f, 0.9f, 0.9f, 1.0f };
	DirectX::XMFLOAT4 ambient = { 0.0f, 0.06f, 0.41f, 1.0f };
	DirectX::XMFLOAT4 specular = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 direction = { -1.0f, -1.0f, 1.0f };

	auto lightTest = sceneInstance.GetCurrentScene()->CreateObject("DirectionalLight");
	lightTest->AddComponent<Light>();
	lightTest->GetComponent<Light>()->CreateDirectionalLight(ambient, diffuse, specular, direction);
	lightTest->GetComponent<Light>()->SetActive(true);
	lightTest->SetActive(true);
	lightTest->_autoAwake = true;

	// PointLight 예시
	auto pointLight = sceneInstance.GetCurrentScene()->CreateObject("pointLightTest");
	pointLight->AddComponent<Light>();
	pointLight->GetComponent<Transform>()->SetPosition(0.f, 0.f, 0.f);
	pointLight->GetComponent<Light>()->CreatePointLight(ambient, diffuse, specular, 1.0f);
	pointLight->GetComponent<Light>()->SetAmbient(1.0f, 1.0f, 1.0f, 1.0f);
	pointLight->GetComponent<Light>()->SetPointRange(10.f);
	pointLight->GetComponent<Light>()->SetPointRange(100.0f);
	pointLight->SetActive(false);

	auto pointLight2 = sceneInstance.GetCurrentScene()->CreateObject("pointLightTest2");
	pointLight2->AddComponent<Light>();
	pointLight2->GetComponent<Transform>()->SetPosition(-70.f, 0.f, 50.f);
	pointLight2->GetComponent<Light>()->CreatePointLight(ambient, diffuse, specular, 0.0f);
	pointLight2->GetComponent<Light>()->SetAmbient(1.0f, 1.0f, 1.0f, 1.0f);
	pointLight2->GetComponent<Light>()->SetPointRange(50.f);
	pointLight2->SetActive(false);

	bossMap = sceneInstance.GetCurrentScene()->CreateObject("bossMap");
	bossMap->AddComponent<MeshRenderer>();
	bossMap->GetComponent<MeshRenderer>()->SetMeshObject("MapMesh/MapMesh");
	bossMap->GetComponent<Transform>()->SetScale(0.1f, 0.1f, 0.1f);
	bossMap->_autoAwake = true;

	// Kamen
	kamen = sceneInstance.GetCurrentScene()->CreateObject("kamen");
	kamen->AddComponent<Kamen>();
	DirectX::XMFLOAT4 bossDiffuse = { 0.f, 1.0f, 0.f, 1.f };
	DirectX::XMFLOAT4 bossAmbient = { 1.f, 1.f, 1.f, 1.f };
	DirectX::XMFLOAT4 bossSpecular = { 1.0f, 1.0f, 1.0f, 1.0f };
	kamen->AddComponent<Light>();
	kamen->GetComponent<Light>()->CreatePointLight(bossAmbient, bossDiffuse, bossSpecular, 300.f, 16.f);
	kamen->GetComponent<Light>()->SetOffSet(0.0f, 30.f, -10.0f);
	kamen->_autoAwake = true;


	// UI의 부모가 될 0,0pos객체
	battle_ui_box = sceneInstance.GetCurrentScene()->CreateObject("BattleUI");
	battle_ui_box->AddComponent<BattleUIManager>();
	//battle_ui_box->_autoAwake = true;


	auto bossSoundManager = sceneInstance.GetCurrentScene()->CreateObject("SoundManager");
	bossSoundManager->_autoAwake = true;
	_bossSoundComp = bossSoundManager->AddComponent<SoundPlayer>();
	auto bossSoundindex = _bossSoundComp->CreateSoundInfo("Resources/Sound/MainSceneBGM.mp3", false, true, 50.f);
	_bossSoundComp->CreateSound(bossSoundindex, 1);
	


	// 맵 꾸미기 파티클
	MapParticleSetting(_mapParticleList);
	EventManager::GetInstance().Initialize();
	EventManager::GetInstance().GetEngineCore(this);
}


void KunrealEngine::EngineCore::CheckMousePosition()
{
	// 에디터로 실행하지 않았을 경우
	if (_editorMousepos.x == -1 && _editorMousepos.y == -1)
	{
		{
			_finalMousePosition.x = _ingameMouseX;
			_finalMousePosition.y = _ingameMouseY;

			this->_isEditor = false;
		}
	}

	else
	{
		_finalMousePosition = _editorMousepos;

		this->_isEditor = true;
	}

	if (this->_isEditor)
	{
		inputInstance->UpdateEditorMousePos(_editorMousepos);
		GraphicsSystem::GetInstance().Update(_editorMousepos.x, _editorMousepos.y);
	}
	else
	{
		inputInstance->UpdateEditorMousePos(_finalMousePosition);
		GraphicsSystem::GetInstance().Update(_finalMousePosition.x, _finalMousePosition.y);
	}
}

void KunrealEngine::EngineCore::MakeObstacle()
{
	DirectX::XMFLOAT3 targetPos = GRAPHICS->ScreenToWorldPoint(InputSystem::GetInstance()->GetEditorMousePos().x, InputSystem::GetInstance()->GetEditorMousePos().y);
	DirectX::XMFLOAT3 bmin = { 4, 4, 4 };
	DirectX::XMFLOAT3 bmax = { 4, 4, 4 };
	navigationInstance.AddBoxTempObstacle(targetPos, bmin, bmax);

	KunrealEngine::GameObject* obstacle;
	obstacle = sceneInstance.GetCurrentScene()->CreateObject("obstacle");
	obstacle->AddComponent<MeshRenderer>();
	obstacle->GetComponent<MeshRenderer>()->SetMeshObject("cube/cube");
	obstacle->GetComponent<MeshRenderer>()->SetPickableState(true);
	obstacle->GetComponent<Transform>()->SetPosition(targetPos.x, targetPos.y, targetPos.z);
	obstacle->GetComponent<Transform>()->SetScale(4.0f, 4.0f, 4.0f);
	obstacle->GetComponent<Transform>()->SetRotation(0.f, 0.f, 0.f);
}

void KunrealEngine::EngineCore::RemoveObstacle()
{
	KunrealEngine::GameObject* obstacle = GraphicsSystem::GetInstance().GetPickedObject();
	DirectX::XMFLOAT3 targetPos = obstacle->GetComponent<Transform>()->GetPosition();
	navigationInstance.RemoveTempObstacle(targetPos);
	//SceneManager::GetInstance().GetCurrentScene()->DeleteGameObject(obstacle);
}

DirectX::XMFLOAT3 KunrealEngine::EngineCore::Bezier(DirectX::XMFLOAT3 startPoint, DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2, DirectX::XMFLOAT3 endPoint, float t)
{
	float u = 1.0f - t;
	float tt = t * t;
	float ttt = tt * t;
	float uu = u * u;
	float uuu = uu * u;

	DirectX::XMFLOAT3 p =
	{
		uuu * startPoint.x + 3 * uu * t * p1.x + 3 * u * tt * p2.x + ttt * endPoint.x,
		uuu * startPoint.y + 3 * uu * t * p1.y + 3 * u * tt * p2.y + ttt * endPoint.y,
		uuu * startPoint.z + 3 * uu * t * p1.z + 3 * u * tt * p2.z + ttt * endPoint.z,
	};

	return p;
}

DirectX::XMFLOAT2 KunrealEngine::EngineCore::BezierPoint2D(DirectX::XMFLOAT2 startPoint, DirectX::XMFLOAT2 p1, DirectX::XMFLOAT2 p2, DirectX::XMFLOAT2 endPoint, float t)
{
	float u = 1.0f - t;
	float tt = t * t;
	float ttt = tt * t;
	float uu = u * u;
	float uuu = uu * u;

	DirectX::XMFLOAT2 p =
	{
		uuu * startPoint.x + 3 * uu * t * p1.x + 3 * u * tt * p2.x + ttt * endPoint.x,
		uuu * startPoint.y + 3 * uu * t * p1.y + 3 * u * tt * p2.y + ttt * endPoint.y,
	};

	return p;
}

std::vector<DirectX::XMFLOAT3> KunrealEngine::EngineCore::BezierSetting(GameObject* bezierObject)
{
	p0.x = bezierObject->GetComponent<Transform>()->GetPosition().x;
	p0.y = bezierObject->GetComponent<Transform>()->GetPosition().y;
	p0.z = bezierObject->GetComponent<Transform>()->GetPosition().z;

	if (p0.x > p3.x)
	{
		p1.x = p0.x - (ToolBox::GetRandomFloat(20.0f, 30.0f) * ToolBox::GetRandomFloat(0.1f, 1.0f));
		p2.x = p0.x - (ToolBox::GetRandomFloat(20.0f, 30.0f) * ToolBox::GetRandomFloat(1.0f, 2.0f));
	}
	else
	{
		p1.x = p0.x + (ToolBox::GetRandomFloat(20.0f, 30.0f) * ToolBox::GetRandomFloat(0.1f, 1.0f));
		p2.x = p0.x + (ToolBox::GetRandomFloat(20.0f, 30.0f) * ToolBox::GetRandomFloat(1.0f, 2.0f));
	}

	p1.y = p0.y + (ToolBox::GetRandomFloat(20.0f, 30.0f) * ToolBox::GetRandomFloat(-1.0f, 1.0f));
	p2.y = p0.y + (ToolBox::GetRandomFloat(20.0f, 30.0f) * ToolBox::GetRandomFloat(-1.0f, 1.0f));

	p1.z = p0.z + (ToolBox::GetRandomFloat(20.0f, 30.0f) * ToolBox::GetRandomFloat(-1.0f, 1.0f));
	p2.z = p0.z + (ToolBox::GetRandomFloat(20.0f, 30.0f) * ToolBox::GetRandomFloat(-1.0f, 1.0f));

	std::vector<DirectX::XMFLOAT3> bezierPosintList;
	bezierPosintList.push_back(p0);
	bezierPosintList.push_back(p1);
	bezierPosintList.push_back(p2);

	//std::vector<std::vector<Point3D>> bezierPointsListSetting;
	//bezierPointsListSetting.push_back(bezierPosintList);

	return bezierPosintList;
}

void KunrealEngine::EngineCore::TitleMapParticle()
{
	DirectX::XMFLOAT3 endPoint = { -152.121f, 182.672f, 180 }; // 목표 위치

	_timeCount += TimeManager::GetInstance().GetDeltaTime();

	if (_isBezierStartSetting == false)
	{
		for (auto& bezierObject : _bezierObjectList) // 베지어 곡선 초기 설정
		{
			_bezierPointsList.push_back(BezierSetting(bezierObject));
		}
		_isBezierStartSetting = true;
	}

	int bezierPointIndex = 0;

	for (auto& bezierPoint : _bezierPointsList) // 각 점을 통한 베지어 곡선을 구함
	{
		DirectX::XMFLOAT3 particlePoint;

		if (_isSettingTimer == false)
		{
			_timeCount = ToolBox::GetRandomFloat(0.1f, 0.6f);

			_timeCountList.push_back(_timeCount);
		}

		particlePoint = Bezier(bezierPoint[0], bezierPoint[1], bezierPoint[2], endPoint, _timeCountList[bezierPointIndex]);
		_particlePointList.push_back(particlePoint);
		bezierPointIndex++;
	}
	_isSettingTimer = true;

	for (int i = 0; i < _bezierObjectList.size(); ++i) // 곡선을 따라 이동
	{
		_bezierObjectList[i]->GetComponent<Transform>()->SetPosition(_particlePointList[i].x, _particlePointList[i].y, _particlePointList[i].z);
	}


	for (auto& timeCount : _timeCountList) // 타이머
	{
		timeCount += TimeManager::GetInstance().GetDeltaTime() / 5;
	}

	int timeCountIndex = 0;

	for (auto& timeCount : _timeCountList)
	{
		if (timeCount > 1.0f)
		{
			_bezierPointsList.erase(_bezierPointsList.begin() + timeCountIndex);
			timeCount = 0.0f;

			_bezierObjectList[timeCountIndex]->GetComponent<Transform>()->SetPosition(endPoint.x + ToolBox::GetRandomFloat(-150.0f, 150.0f),
				endPoint.y + ToolBox::GetRandomFloat(-80.0f, 80.0f), endPoint.z + ToolBox::GetRandomFloat(-20.0f, 20.0f));

			_bezierPointsList.insert(_bezierPointsList.begin() + timeCountIndex, BezierSetting(_bezierObjectList[timeCountIndex]));
		}
		++timeCountIndex;
	}

	_particlePointList.clear();
}

void KunrealEngine::EngineCore::MoveToMain()
{
	if (sceneInstance.GetCurrentScene()->GetSceneName() == "Title")
	{
		if (sceneInstance.GetCurrentScene()->GetGameObject("BossPortal")->GetCollider()->IsCollided()
			&& sceneInstance.GetCurrentScene()->GetGameObject("BossPortal")->GetCollider()->GetTargetObject() == sceneInstance.GetCurrentScene()->GetObjectWithTag("Player"))
		{
			// 시작 할때 포탈로 넘어가는 소리
			GameObject* soundManager = sceneInstance.GetCurrentScene()->GetGameObject("TitleSceneSound");
			SoundPlayer* soundComp = soundManager->GetComponent<SoundPlayer>();
			int portal = soundComp->FindIndex("Resources/Sound/intro.mp3");
			soundComp->Play(portal);
			int titlebgmindex = soundComp->FindIndex("Resources/Sound/TitleMap.mp3");
			soundComp->Stop(titlebgmindex);

			// 원래 씬 교체하는 부분
			sceneInstance.ChangeScene("Main");
			ResetMenuUIPack(sceneInstance.GetScene("Title")->GetGameObject("pauseuibox"), "Title", "Main");
			ResetMenuUIPack(sceneInstance.GetScene("Title")->GetGameObject("Option"), "Title", "Main");
			battle_ui_box->GetComponent<BattleUIManager>()->Resebattleuibar();

			sceneInstance.GetCurrentScene()->GetGameObject("DirectionalLight")->GetComponent<Light>()->SetDirection(-1.0f, -1.0f, 1.0f);

			sceneInstance.GetScene("Title")->GetGameObject("TitlePlayer")->GetComponent<Player>()->_iswakeuped = false;
			sceneInstance.GetScene("Main")->GetGameObject("Player")->GetComponent<PlayerMove>()->SetPlayerY(2.0f);
			navigationInstance.HandleBuild(0, "bossmap.obj");
			navigationInstance.HandleBuild(1, "bossmap.obj");
			EventManager::GetInstance().SetCamera("testCamera");
			EventManager::GetInstance()._iscamfollow = true;

			// 큐브맵 변경
			GRAPHICS->SetMainCubeMap("BossMap");

			// 플레이어 위치 조정 및 초기화
			sceneInstance.GetScene("Main")->GetGameObject("Player")->GetComponent<Player>()->ResetPlayerStatus();

			//EventManager::GetInstance().SetDifficulty(eDifficulty::eEasy);

			titleCamera->GetComponent<Camera>()->CameraRotateY(-1 * g_test);
			g_rageOfLee = 0.0f;
			g_test = 0.0f;
		}

	}
}

void KunrealEngine::EngineCore::FloatingY(GameObject* name, float time)
{
	auto transform = name->GetComponent<Transform>();
	float objectX = name->GetComponent<Transform>()->GetPosition().x;
	float objectY = name->GetComponent<Transform>()->GetPosition().y;
	float objectZ = name->GetComponent<Transform>()->GetPosition().z;


	if (transform->_floatingFactor >= 0.0f && transform->_floatingFactor <= time)
	{
		transform->_floatingFactor += TimeManager::GetInstance().GetDeltaTime();
		name->GetComponent<Transform>()->SetPosition(objectX, objectY + 0.03f, objectZ);

		if (transform->_floatingFactor >= time - 1.0f || transform->_floatingFactor <= 1.0f)
		{
			name->GetComponent<Transform>()->SetPosition(objectX, objectY + 0.02f, objectZ);
		}

		if (transform->_floatingFactor >= time)
		{
			transform->_floatingHelper = 0.0f;
		}
	}
	else if (transform->_floatingHelper >= 0.0f && transform->_floatingHelper <= time)
	{
		transform->_floatingHelper += TimeManager::GetInstance().GetDeltaTime();
		name->GetComponent<Transform>()->SetPosition(objectX, objectY - 0.03f, objectZ);

		if (transform->_floatingHelper >= time - 1.0f || transform->_floatingHelper <= 1.0f)
		{
			name->GetComponent<Transform>()->SetPosition(objectX, objectY - 0.02f, objectZ);
		}

		if (transform->_floatingHelper >= time)
		{
			transform->_floatingFactor = 0.0f;
		}
	}
}

void KunrealEngine::EngineCore::ShiveringLight(GameObject* name)
{
	name->GetComponent<Light>()->SetOffSet(ToolBox::GetRandomFloat(-3.0f, 3.0f), 15.f, 0.0f);
}

void KunrealEngine::EngineCore::CreateTitleScene()
{
	Scene* titleScene = sceneInstance.CreateScene("Title");
	sceneInstance.ChangeScene("Title");

	navigationInstance.HandleBuild(0, "bridge_mapmesh.obj");
	// Camera
	titleCamera = sceneInstance.GetCurrentScene()->CreateObject("TitleCamera");
	titleCamera->_autoAwake = true;
	DirectX::XMFLOAT3 cameraPos = { 0.0f, 0.0f, 1.0f };

	DirectX::XMFLOAT3 targetPos = { 0.0f, 0.0f, 0.0f };
	titleCamera->AddComponent<Camera>();
	titleCamera->GetComponent<Camera>()->SetCameraPosition(cameraPos.x, cameraPos.y, cameraPos.z);
	titleCamera->GetComponent<Camera>()->SetTargetPosition(targetPos.x, targetPos.y, targetPos.z);

	titleCamera->GetComponent<Camera>()->SetMainCamera();

	titleCamera->GetComponent<Transform>()->SetPosition({ -155.0f, 135.0f,-45.130f });
	titleCamera->GetComponent<Transform>()->SetRotation(-137.f, 0.f, 0.f);

	EventManager::GetInstance().SetCamera("TitleCamera");

	// light test
	DirectX::XMFLOAT4 diffuse = { 0.3f, 0.3f, 0.430f, 0.3f };
	DirectX::XMFLOAT4 ambient = { 0.0f, 0.06f, 0.410f, 0.2f };
	DirectX::XMFLOAT4 specular = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 direction = { 1.0f, -1.0f, 0.1f };

	GameObject* titleSceneSound = sceneInstance.GetCurrentScene()->CreateObject("TitleSceneSound");
	titleSceneSound->_autoAwake = true;
	SoundPlayer* soundComp = titleSceneSound->AddComponent<SoundPlayer>();

	int titlebgm1 = soundComp->CreateSoundInfo("Resources/Sound/TitleMap.mp3");
	int portal = soundComp->CreateSoundInfo("Resources/Sound/intro.mp3", false, false);
	soundComp->CreateSound(portal, 1);
	soundComp->CreateSound(titlebgm1, 0);

	GameObject* titleLight = sceneInstance.GetCurrentScene()->CreateObject("DirectionalLight");
	titleLight->_autoAwake = true;
	titleLight->AddComponent<Light>();
	Light* light = titleLight->GetComponent<Light>();
	light->CreateDirectionalLight(ambient, diffuse, specular, direction);
	light->SetActive(true);
	titleLight->SetActive(true);

	GameObject* bridge = sceneInstance.GetCurrentScene()->CreateObject("Bridge");
	bridge->_autoAwake = true;
	bridge->GetComponent<Transform>()->SetPosition(-155.0f, 66.0f, 0.0f);
	bridge->GetComponent<Transform>()->SetScale(0.1f, 0.1f, 0.1f);
	bridge->GetComponent<Transform>()->SetRotation(0.0f, 90.0f, 0.0f);
	bridge->AddComponent<MeshRenderer>();

	MeshRenderer* bridgeMesh = bridge->GetComponent<MeshRenderer>();

	bridgeMesh->SetMeshObject("Bridge/Bridge", true);

	//// cube map test
	GRAPHICS->CreateCubeMap("TitleBackground", "DarkMoon.dds", true);
	GRAPHICS->SetMainCubeMap("TitleBackground");

	pause_ui_box = MakeMenuUIPack();
	option_ui_box = sceneInstance.GetCurrentScene()->CreateObject("Option");
	option_ui_box->AddComponent<OptionUIManager>();

	GameObject* titleUIpack = MakeTitleUIPack();

	BeforeStartTutorial();

	TitlesceneobjectSetting(_bezierObjectList);
}

void KunrealEngine::EngineCore::CreateEndingScene()
{
	Scene* endingScene = sceneInstance.CreateScene("Ending");
	//sceneInstance.ChangeScene("Ending");

	// Camera
	_endingCamera = endingScene->CreateObject("EndingCamera");
	_endingCamera->_autoAwake = true;
	DirectX::XMFLOAT3 cameraPos = { 0.0f, 0.0f, 1.0f };

	DirectX::XMFLOAT3 targetPos = { 0.0f, 0.0f, 0.0f };
	_endingCamera->AddComponent<Camera>();
	_endingCamera->GetComponent<Camera>()->SetCameraPosition(cameraPos.x, cameraPos.y, cameraPos.z);
	_endingCamera->GetComponent<Camera>()->SetTargetPosition(targetPos.x, targetPos.y, targetPos.z);
	_endingCamera->GetComponent<Transform>()->SetPosition({ -322.f, 100.0f,-148.130f });
	_endingCamera->GetComponent<Transform>()->SetRotation(-182.f, 168.f, 0.f);

	_endingCameraAfter = endingScene->CreateObject("EndingCameraAfter");
	_endingCameraAfter->_autoAwake = true;
	_endingCameraAfter->AddComponent<Camera>();
	_endingCameraAfter->GetComponent<Camera>()->SetCameraPosition(cameraPos.x, cameraPos.y, cameraPos.z);
	_endingCameraAfter->GetComponent<Camera>()->SetTargetPosition(targetPos.x, targetPos.y, targetPos.z);
	_endingCameraAfter->GetComponent<Transform>()->SetPosition(-322.f, 100.0f,-148.130f);
	//_endingCameraAfter->GetComponent<Transform>()->SetRotation(-182.f, 168.f, 0.0f);

	DirectX::XMFLOAT4 diffuse = { 1.f, 1.f, 1.f, 0.3f };
	DirectX::XMFLOAT4 ambient = { 1.f, 1.f, 1.f, 0.2f };
	DirectX::XMFLOAT4 specular = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 direction = { 0.18f, -0.16f, -1.0f };

	_endingSoundManager = endingScene->CreateObject("SoundManager");
	_endingSoundManager->_autoAwake = true;
	_endingSoundComp = _endingSoundManager->AddComponent<SoundPlayer>();
	endingSoundindex = _endingSoundComp->CreateSoundInfo("Resources/Sound/EndingBGM (mp3cut.net) (2).mp3", false, false);
	_endingSoundComp->CreateSound(endingSoundindex, 1);
	_endingSoundComp->SetVolume(20.f, endingSoundindex);

	_endingLight = endingScene->CreateObject("DirectionalLight");
	_endingLight->_autoAwake = true;
	_endingLight->AddComponent<Light>();
	Light* light = _endingLight->GetComponent<Light>();
	light->CreateDirectionalLight(ambient, diffuse, specular, direction);
	light->SetActive(true);
	_endingLight->SetActive(true);

	//// cube map test
	GRAPHICS->CreateCubeMap("EndingBackground", "Texture6.dds", true);
	//GRAPHICS->SetMainCubeMap("EndingBackground");

	endingRock = endingScene->CreateObject("EndingRock");
	endingRock->_autoAwake = true;
	endingRock->AddComponent<MeshRenderer>();
	endingRock->GetComponent<MeshRenderer>()->SetMeshObject("EndingRock/EndingRock");
	endingRock->GetComponent<Transform>()->SetPosition(-302.0f, 92.f, -190.0f);
	endingRock->GetComponent<Transform>()->SetRotation(0.0f, 0.0f, 0.0f);
	endingRock->GetComponent<Transform>()->SetScale(0.1f, 0.1f, 0.1f);

	endingBoss = endingScene->CreateObject("endingBoss");
	endingBoss->_autoAwake = true;
	endingBoss->AddComponent<MeshRenderer>();
	endingBoss->GetComponent<MeshRenderer>()->SetMeshObject("Lich/Lich");
	endingBoss->GetComponent<Transform>()->SetScale(1.5f, 1.5f, 1.5f);
	endingBoss->GetComponent<Transform>()->SetPosition(-340.0f, 100.f, -267.0f);
	endingBoss->GetComponent<Transform>()->SetRotation(0.f, 212.f, 0.f);
	auto texSize = endingBoss->GetComponent<MeshRenderer>()->GetTextures().size();
	for (int i = 0; i < texSize; i++)
	{
		endingBoss->GetComponent<MeshRenderer>()->SetDiffuseTexture(i, "Lich/T_Lich_1_D.tga");
		endingBoss->GetComponent<MeshRenderer>()->SetNormalTexture(i, "Lich/T_Lich_N.tga");
		endingBoss->GetComponent<MeshRenderer>()->SetEmissiveTexture(i, "Lich/T_Lich_01_E.tga");
	}


	endingPlayer = endingScene->CreateObject("EndingPlayer");
	endingPlayer->_autoAwake = true;
	endingPlayer->AddComponent<MeshRenderer>();
	endingPlayer->GetComponent<MeshRenderer>()->SetMeshObject("PlayerWithCloak/PlayerWithCloak");
	endingPlayer->GetComponent<Transform>()->SetPosition(-302.0f, 92.f, -190.0f);
	endingPlayer->GetComponent<Transform>()->SetRotation(3.0f, 9.f, 0.0f);
	endingPlayer->GetComponent<Transform>()->SetScale(0.1f, 0.1f, 0.1f);

	// Particle
	endingPlayer->AddComponent<Particle>();
	endingPlayer->GetComponent<Particle>()->SetParticleEffect("Lightning1", "Resources/Textures/Particles/fx_Lightning1.dds", 1000);
	//endingPlayer->GetComponent<Particle>()->SetParticlePos(-259.0f, 90.f, -255.0f);
	endingPlayer->GetComponent<Particle>()->SetParticleDuration(5.9f, 6.6f);
	endingPlayer->GetComponent<Particle>()->SetParticleVelocity(11.2f, true);
	endingPlayer->GetComponent<Particle>()->SetParticleSize(2.0f, 2.0f);
	endingPlayer->GetComponent<Particle>()->AddParticleColor(1.1f, 0.5f, 1.0f);
	endingPlayer->GetComponent<Particle>()->SetParticleDirection(-1.4f, 6.3f, -1.7f);
	endingPlayer->GetComponent<Particle>()->SetActive(false);

	_endingMeteo1 = endingScene->CreateObject("EndingMeteo1");
	_endingMeteo1->_autoAwake = true;
	_endingMeteo1->AddComponent<MeshRenderer>();
	_endingMeteo1->GetComponent<MeshRenderer>()->SetMeshObject("BigMeteo1/BigMeteo1");
	_endingMeteo1->GetComponent<Transform>()->SetPosition(153.7f, -60.f, -255.0f);
	_endingMeteo1->GetComponent<Transform>()->SetScale(0.3f, 0.3f, 0.3f);
	_endingMeteo1->AddComponent<Particle>();
	_endingMeteo1->GetComponent<Particle>()->SetParticleEffect("Meteo1", "Resources/Textures/Particles/fx_Lightning1.dds", 1000);
	_endingMeteo1->GetComponent<Particle>()->SetParticleDuration(5.9f, 6.6f);
	_endingMeteo1->GetComponent<Particle>()->SetParticleVelocity(11.2f, true);
	_endingMeteo1->GetComponent<Particle>()->SetParticleSize(1.0f, 1.0f);
	_endingMeteo1->GetComponent<Particle>()->AddParticleColor(1.0f, 1.0f, 1.0f);
	_endingMeteo1->GetComponent<Particle>()->SetParticleDirection(-1.4f, 6.3f, -1.7f);
	_endingMeteo1->GetComponent<Particle>()->SetActive(true);

	_endingMeteo2 = endingScene->CreateObject("EndingMeteo2");
	_endingMeteo2->_autoAwake = true;
	_endingMeteo2->AddComponent<MeshRenderer>();
	_endingMeteo2->GetComponent<MeshRenderer>()->SetMeshObject("BigMeteo2/BigMeteo2");
	_endingMeteo2->GetComponent<Transform>()->SetPosition(-203.6f, -50.f, -255.0f);
	_endingMeteo2->AddComponent<Particle>();
	_endingMeteo2->GetComponent<Particle>()->SetParticleEffect("Meteo2", "Resources/Textures/Particles/fx_Lightning1.dds", 1000);
	_endingMeteo2->GetComponent<Particle>()->SetParticleDuration(5.9f, 6.6f);
	_endingMeteo2->GetComponent<Particle>()->SetParticleVelocity(11.2f, true);
	_endingMeteo2->GetComponent<Particle>()->SetParticleSize(1.0f, 1.0f);
	_endingMeteo2->GetComponent<Particle>()->AddParticleColor(1.0f, 1.0f, 1.0f);
	_endingMeteo2->GetComponent<Particle>()->SetParticleDirection(-1.4f, 6.3f, -1.7f);
	_endingMeteo2->GetComponent<Particle>()->SetActive(true);

	_endingMeteo3 = endingScene->CreateObject("EndingMeteo3");
	_endingMeteo3->_autoAwake = true;
	_endingMeteo3->AddComponent<MeshRenderer>();
	_endingMeteo3->GetComponent<MeshRenderer>()->SetMeshObject("BigMeteo3/BigMeteo3");
	_endingMeteo3->GetComponent<Transform>()->SetPosition(-300.0f, -70.f, -462.0f);
	_endingMeteo3->AddComponent<Particle>();
	_endingMeteo3->GetComponent<Particle>()->SetParticleEffect("Meteo3", "Resources/Textures/Particles/fx_Lightning1.dds", 1000);
	_endingMeteo3->GetComponent<Particle>()->SetParticleDuration(5.9f, 6.6f);
	_endingMeteo3->GetComponent<Particle>()->SetParticleVelocity(11.2f, true);
	_endingMeteo3->GetComponent<Particle>()->SetParticleSize(1.0f, 1.0f);
	_endingMeteo3->GetComponent<Particle>()->AddParticleColor(1.0f, 1.0f, 1.0f);
	_endingMeteo3->GetComponent<Particle>()->SetParticleDirection(-1.4f, 6.3f, -1.7f);
	_endingMeteo3->GetComponent<Particle>()->SetActive(true);

	_endingMeteo4 = endingScene->CreateObject("EndingMeteo4");
	_endingMeteo4->_autoAwake = true;
	_endingMeteo4->AddComponent<MeshRenderer>();
	_endingMeteo4->GetComponent<MeshRenderer>()->SetMeshObject("BigMeteo4/BigMeteo4");
	_endingMeteo4->GetComponent<Transform>()->SetPosition(-173.0f, -50.f, -315.0f);
	_endingMeteo4->AddComponent<Particle>();
	_endingMeteo4->GetComponent<Particle>()->SetParticleEffect("Meteo4", "Resources/Textures/Particles/fx_Lightning1.dds", 1000);
	_endingMeteo4->GetComponent<Particle>()->SetParticleDuration(5.9f, 6.6f);
	_endingMeteo4->GetComponent<Particle>()->SetParticleVelocity(11.2f, true);
	_endingMeteo4->GetComponent<Particle>()->SetParticleSize(1.0f, 1.0f);
	_endingMeteo4->GetComponent<Particle>()->AddParticleColor(1.0f, 1.0f, 1.0f);
	_endingMeteo4->GetComponent<Particle>()->SetParticleDirection(-1.4f, 6.3f, -1.7f);
	_endingMeteo4->GetComponent<Particle>()->SetActive(true);

	_endingMeteo5 = endingScene->CreateObject("EndingMeteo5");
	_endingMeteo5->_autoAwake = true;
	_endingMeteo5->AddComponent<MeshRenderer>();
	_endingMeteo5->GetComponent<MeshRenderer>()->SetMeshObject("SmallMeteo1/SmallMeteo1");
	_endingMeteo5->GetComponent<Transform>()->SetPosition(99.0f, -100.f, -450.0f);
	_endingMeteo5->AddComponent<Particle>();
	_endingMeteo5->GetComponent<Particle>()->SetParticleEffect("Meteo5", "Resources/Textures/Particles/fx_Lightning1.dds", 1000);
	_endingMeteo5->GetComponent<Particle>()->SetParticleDuration(5.9f, 6.6f);
	_endingMeteo5->GetComponent<Particle>()->SetParticleVelocity(11.2f, true);
	_endingMeteo5->GetComponent<Particle>()->SetParticleSize(1.0f, 1.0f);
	_endingMeteo5->GetComponent<Particle>()->AddParticleColor(1.0f, 1.0f, 1.0f);
	_endingMeteo5->GetComponent<Particle>()->SetParticleDirection(-1.4f, 6.3f, -1.7f);
	_endingMeteo5->GetComponent<Particle>()->SetActive(true);

	_endingMeteo6 = endingScene->CreateObject("EndingMeteo6");
	_endingMeteo6->_autoAwake = true;
	_endingMeteo6->AddComponent<MeshRenderer>();
	_endingMeteo6->GetComponent<MeshRenderer>()->SetMeshObject("SmallMeteo2/SmallMeteo2");
	_endingMeteo6->GetComponent<Transform>()->SetPosition(77.f, -200.f, -632.0f);
	_endingMeteo6->AddComponent<Particle>();
	_endingMeteo6->GetComponent<Particle>()->SetParticleEffect("Meteo6", "Resources/Textures/Particles/fx_Lightning1.dds", 1000);
	_endingMeteo6->GetComponent<Particle>()->SetParticleDuration(5.9f, 6.6f);
	_endingMeteo6->GetComponent<Particle>()->SetParticleVelocity(11.2f, true);
	_endingMeteo6->GetComponent<Particle>()->SetParticleSize(1.0f, 1.0f);
	_endingMeteo6->GetComponent<Particle>()->AddParticleColor(1.0f, 1.0f, 1.0f);
	_endingMeteo6->GetComponent<Particle>()->SetParticleDirection(-1.4f, 6.3f, -1.7f);
	_endingMeteo6->GetComponent<Particle>()->SetActive(true);


	_endingMeteo7 = endingScene->CreateObject("EndingMeteo7");
	_endingMeteo7->_autoAwake = true;
	_endingMeteo7->AddComponent<MeshRenderer>();
	_endingMeteo7->GetComponent<MeshRenderer>()->SetMeshObject("SmallMeteo3/SmallMeteo3");
	_endingMeteo7->GetComponent<Transform>()->SetPosition(-329.0f, -150.f, -255.0f);
	_endingMeteo7->AddComponent<Particle>();
	_endingMeteo7->GetComponent<Particle>()->SetParticleEffect("Meteo7", "Resources/Textures/Particles/fx_Lightning1.dds", 1000);
	_endingMeteo7->GetComponent<Particle>()->SetParticleDuration(5.9f, 6.6f);
	_endingMeteo7->GetComponent<Particle>()->SetParticleVelocity(11.2f, true);
	_endingMeteo7->GetComponent<Particle>()->SetParticleSize(1.0f, 1.0f);
	_endingMeteo7->GetComponent<Particle>()->AddParticleColor(1.0f, 1.0f, 1.0f);
	_endingMeteo7->GetComponent<Particle>()->SetParticleDirection(-1.4f, 6.3f, -1.7f);
	_endingMeteo7->GetComponent<Particle>()->SetActive(true);


	_endingMeteo8 = endingScene->CreateObject("EndingMeteo8");
	_endingMeteo8->_autoAwake = true;
	_endingMeteo8->AddComponent<MeshRenderer>();
	_endingMeteo8->GetComponent<MeshRenderer>()->SetMeshObject("SmallMeteo4/SmallMeteo4");
	_endingMeteo8->GetComponent<Transform>()->SetPosition(-261.0f, -200.f, -172.0f);
	_endingMeteo8->AddComponent<Particle>();
	_endingMeteo8->GetComponent<Particle>()->SetParticleEffect("Meteo8", "Resources/Textures/Particles/fx_Lightning1.dds", 1000);
	_endingMeteo8->GetComponent<Particle>()->SetParticleDuration(5.9f, 6.6f);
	_endingMeteo8->GetComponent<Particle>()->SetParticleVelocity(11.2f, true);
	_endingMeteo8->GetComponent<Particle>()->SetParticleSize(1.0f, 1.0f);
	_endingMeteo8->GetComponent<Particle>()->AddParticleColor(1.0f, 1.0f, 1.0f);
	_endingMeteo8->GetComponent<Particle>()->SetParticleDirection(-1.4f, 6.3f, -1.7f);
	_endingMeteo8->GetComponent<Particle>()->SetActive(true);

	_endingEnt = endingScene->CreateObject("EndingEnt");
	_endingEnt->_autoAwake = true;
	_endingEnt->AddComponent<MeshRenderer>();
	_endingEnt->GetComponent<MeshRenderer>()->SetMeshObject("Ent_Generic/Ent_Generic");
	_endingEnt->GetComponent<Transform>()->SetPosition(-273.0f, -80.f, -475.0f);
	_endingEnt->GetComponent<Transform>()->SetRotation(0.0f, 170.f, 56.0f);
	_endingEnt->GetComponent<Transform>()->SetScale(22.0f, 22.f, 22.0f);
	size_t entTexSize = _endingEnt->GetComponent<MeshRenderer>()->GetTextures().size();
	for (int i = 0; i < entTexSize; i++)
	{
		_endingEnt->GetComponent<MeshRenderer>()->SetDiffuseTexture(i, "Ent_Generic/T_Ent_1_D.tga");
		_endingEnt->GetComponent<MeshRenderer>()->SetNormalTexture(i, "Ent_Generic/T_Ent_N.tga");
		_endingEnt->GetComponent<MeshRenderer>()->SetEmissiveTexture(i, "Ent_Generic/T_Ent_1_E.tga");
	}

	_endingSpider = endingScene->CreateObject("EndingSpider");
	_endingSpider->_autoAwake = true;
	_endingSpider->AddComponent<MeshRenderer>();
	_endingSpider->GetComponent<MeshRenderer>()->SetMeshObject("SpiderQueen/SpiderQueen");
	_endingSpider->GetComponent<Transform>()->SetPosition(-242.0f, -90.f, -475.0f);
	_endingSpider->GetComponent<Transform>()->SetRotation(-10.0f, -207.f, -381.0f);
	_endingSpider->GetComponent<Transform>()->SetScale(20.0f, 20.f, 20.0f);
	size_t spiderTexSize = _endingSpider->GetComponent<MeshRenderer>()->GetTextures().size();
	for (int i = 0; i < spiderTexSize; i++)
	{
		_endingSpider->GetComponent<MeshRenderer>()->SetDiffuseTexture(i, "SpiderQueen/T_SpiderQueen_1_D.tga");
		_endingSpider->GetComponent<MeshRenderer>()->SetNormalTexture(i, "SpiderQueen/T_SpiderQueen_N.tga");
		_endingSpider->GetComponent<MeshRenderer>()->SetEmissiveTexture(i, "SpiderQueen/T_SpiderQueen_1_E.tga");
	}

	_endingSword = endingScene->CreateObject("EndingSword");
	_endingSword->_autoAwake = true;
	_endingSword->AddComponent<MeshRenderer>();
	_endingSword->GetComponent<MeshRenderer>()->SetMeshObject("KamenSword/KamenSword");
	_endingSword->GetComponent<Transform>()->SetPosition(-298.0f, -50.f, -397.0f);
	_endingSword->GetComponent<Transform>()->SetRotation(-86.0f, 94.f, -188.0f);
	_endingSword->GetComponent<Transform>()->SetScale(2.5f, 2.5f, 2.5f);
	auto swordTexSize = _endingSword->GetComponent<MeshRenderer>()->GetTextures().size();
	for (int i = 0; i < spiderTexSize; i++)
	{
		_endingSword->GetComponent<MeshRenderer>()->SetDiffuseTexture(i, "KamenSword/KamenSword_BaseColor.png");
		_endingSword->GetComponent<MeshRenderer>()->SetNormalTexture(i, "KamenSword/KamenSword_Normal.png");
		_endingSword->GetComponent<MeshRenderer>()->SetEmissiveTexture(i, "KamenSword/KamenSword_Emissive.png");
	}


	_endingKachujin = endingScene->CreateObject("EndingKachujin");
	_endingKachujin->_autoAwake = true;
	_endingKachujin->AddComponent<MeshRenderer>();
	_endingKachujin->GetComponent<MeshRenderer>()->SetMeshObject("Kachujin/Kachujin");
	_endingKachujin->GetComponent<Transform>()->SetPosition(-239.0f, -90.f, -475.0f);
	_endingKachujin->GetComponent<Transform>()->SetRotation(10.0f, -149.f, -21.0f);
	_endingKachujin->GetComponent<Transform>()->SetScale(0.3f, 0.3f, 0.3f);

	_endingKunho = endingScene->CreateObject("EndingKunho");
	_endingKunho->_autoAwake = true;
	_endingKunho->AddComponent<MeshRenderer>();
	_endingKunho->GetComponent<MeshRenderer>()->SetMeshObject("EndingPlayer/EndingPlayer");
	_endingKunho->GetComponent<Transform>()->SetPosition(-250.0f, -100.f, -475.0f);
	_endingKunho->GetComponent<Transform>()->SetRotation(25.0f, 140.f, -41.0f);
	_endingKunho->GetComponent<Transform>()->SetScale(0.5f, 0.5f, 0.5f);


	/// <summary>
	/// 엔딩 크레딧
	/// </summary>
	_endingCredit1 = endingScene->CreateObject("EndingCredit1");
	_endingCredit1->_autoAwake = true;
	auto credit1 = _endingCredit1->AddComponent<ImageRenderer>();
	credit1->SetImage("ui/Ending/Programmers.png");
	credit1->SetPosition(0.0f, 1460.f);

	_endingLee = endingScene->CreateObject("EndingCredit2");
	_endingLee->_autoAwake = true;
	auto credit2 = _endingLee->AddComponent<ImageRenderer>();
	credit2->SetImage("ui/Ending/Lee.png");
	credit2->SetPosition(0.0f, 1460.f);

	_endingYoon = endingScene->CreateObject("EndingCredit3");
	_endingYoon->_autoAwake = true;
	auto credit3 = _endingYoon->AddComponent<ImageRenderer>();
	credit3->SetImage("ui/Ending/Yoon.png");
	credit3->SetPosition(0.0f, 1460.f);

	_endingKim = endingScene->CreateObject("EndingCredit4");
	_endingKim->_autoAwake = true;
	auto credit4 = _endingKim->AddComponent<ImageRenderer>();
	credit4->SetImage("ui/Ending/Kim.png");
	credit4->SetPosition(0.0f, 1460.f);

	_endingBae = endingScene->CreateObject("EndingCredit5");
	_endingBae->_autoAwake = true;
	auto credit5 = _endingBae->AddComponent<ImageRenderer>();
	credit5->SetImage("ui/Ending/Bae.png");
	credit5->SetPosition(0.0f, 1460.f);

	_endingJung = endingScene->CreateObject("EndingCredit6");
	_endingJung->_autoAwake = true;
	auto credit6 = _endingJung->AddComponent<ImageRenderer>();
	credit6->SetImage("ui/Ending/Jeong.png");
	credit6->SetPosition(0.0f, 1460.f);

	_endingThankYou = endingScene->CreateObject("EndingThankYou");
	_endingThankYou->_autoAwake = true;
	auto credit7 = _endingThankYou->AddComponent<ImageRenderer>();
	credit7->SetImage("ui/Ending/Thanks.png");
	credit7->SetPosition(0.0f, 1460.f);

	_endingTheme = endingScene->CreateObject("EndingTheme");
	_endingTheme->_autoAwake = true;
	auto credit8 = _endingTheme->AddComponent<ImageRenderer>();
	credit8->SetImage("ui/Ending/endingTheme2.png");
	credit8->SetPosition(0.0f, 1620.f);

	_bossTheme = endingScene->CreateObject("BossTheme");
	_bossTheme->_autoAwake = true;
	auto credit9 = _bossTheme->AddComponent<ImageRenderer>();
	credit9->SetImage("ui/Ending/BossTheme2.png");
	credit9->SetPosition(0.0f, 1460.f);
}

void KunrealEngine::EngineCore::EndingSceneUpdate()
{
	_endingTimer -= TimeManager::GetInstance().GetDeltaTime();


	_endingMeteo1->GetComponent<Transform>()->SetPosition(153.7f,
		CountPlusLoop(_endingMeteo1->GetComponent<Transform>()->GetPosition().y, 200.f, 3.f), -255.0f);

	_endingMeteo2->GetComponent<Transform>()->SetPosition(-203.6f,
		CountPlusLoop(_endingMeteo2->GetComponent<Transform>()->GetPosition().y, 250.f, 3.f), -255.0f);

	_endingMeteo3->GetComponent<Transform>()->SetPosition(-300.0f,
		CountPlusLoop(_endingMeteo3->GetComponent<Transform>()->GetPosition().y, 400.f, 6.f), -462.0f);

	_endingMeteo4->GetComponent<Transform>()->SetPosition(-173.0f,
		CountPlusLoop(_endingMeteo4->GetComponent<Transform>()->GetPosition().y, 300.f, 4.f), -315.0f);

	_endingMeteo5->GetComponent<Transform>()->SetPosition(99.0f,
		CountPlusLoop(_endingMeteo5->GetComponent<Transform>()->GetPosition().y, 200.f, 5.f), -450.0f);

	_endingMeteo6->GetComponent<Transform>()->SetPosition(-77.f,
		CountPlusLoop(_endingMeteo6->GetComponent<Transform>()->GetPosition().y, 400.f, 3.f), -632.0f);

	_endingMeteo7->GetComponent<Transform>()->SetPosition(-329.0f,
		CountPlusLoop(_endingMeteo7->GetComponent<Transform>()->GetPosition().y, 200.f, 4.f), -255.0f);

	_endingMeteo8->GetComponent<Transform>()->SetPosition(-261.0f,
		CountPlusLoop(_endingMeteo8->GetComponent<Transform>()->GetPosition().y, 300.f, 5.f), -172.0f);


	if (_endingTimer <= -5.f)
	{
		_endingCredit1->GetComponent<ImageRenderer>()->SetPosition(0.0f,
			CountMinus(_endingCredit1->GetComponent<Transform>()->GetPosition().y, -1000.0f, 65.f));
	}
	if (_endingTimer <= -7.f)
	{
		_endingLee->GetComponent<ImageRenderer>()->SetPosition(0.0f,
			CountMinus(_endingLee->GetComponent<Transform>()->GetPosition().y, -1000.0f, 65.f));

		_endingKunho->GetComponent<Transform>()->SetPosition(-250.0f,
			CountPlus(_endingKunho->GetComponent<Transform>()->GetPosition().y, 300.f, 11.f), -475.0f);
	}
	if (_endingTimer <= -17.f)
	{
		_endingYoon->GetComponent<ImageRenderer>()->SetPosition(0.0f,
			CountMinus(_endingYoon->GetComponent<Transform>()->GetPosition().y, -1000.0f, 65.f));

		_endingSword->GetComponent<Transform>()->SetPosition(-298.0f,
			CountPlus(_endingSword->GetComponent<Transform>()->GetPosition().y, 300.f, 10.f), -397.0f);
	}
	if (_endingTimer <= -27.f)
	{
		_endingKim->GetComponent<ImageRenderer>()->SetPosition(0.0f,
			CountMinus(_endingKim->GetComponent<Transform>()->GetPosition().y, -1000.0f, 65.f));

		_endingKachujin->GetComponent<Transform>()->SetPosition(-239.0f,
			CountPlus(_endingKachujin->GetComponent<Transform>()->GetPosition().y, 300.f, 10.f), -475.0f);
	}
	if (_endingTimer <= -37.0f)
	{
		_endingBae->GetComponent<ImageRenderer>()->SetPosition(0.0f,
			CountMinus(_endingBae->GetComponent<Transform>()->GetPosition().y, -1000.0f, 65.f));

		_endingEnt->GetComponent<Transform>()->SetPosition(-273.0f,
			CountPlus(_endingEnt->GetComponent<Transform>()->GetPosition().y, 300.f, 10.f), -475.0f);
	}
	if (_endingTimer <= -47.f)
	{
		_endingJung->GetComponent<ImageRenderer>()->SetPosition(0.0f,
			CountMinus(_endingJung->GetComponent<Transform>()->GetPosition().y, -1000.0f, 65.f));

		_endingSpider->GetComponent<Transform>()->SetPosition(-242.0f,
			CountPlus(_endingSpider->GetComponent<Transform>()->GetPosition().y, 300.f, 11.f), -475.0f);
	}
	if (_endingTimer <= -55.f)
	{
		_bossTheme->GetComponent<ImageRenderer>()->SetPosition(0.f,
			CountMinus(_bossTheme->GetComponent<Transform>()->GetPosition().y, -1000.0f, 65.f));

		_endingTheme->GetComponent<ImageRenderer>()->SetPosition(0.f,
			CountMinus(_endingTheme->GetComponent<Transform>()->GetPosition().y, -1000.0f, 65.f));
	}
	if (_endingTimer <= -70.f)
	{
		_endingThankYou->GetComponent<ImageRenderer>()->SetPosition(0.f,
			CountMinus(_endingThankYou->GetComponent<Transform>()->GetPosition().y, 680.0f, 65.f));
	}
	_endingEnt->GetComponent<Animator>()->Play("Idle", 0.1f, true);
	_endingSpider->GetComponent<Animator>()->Play("Idle", 0.1f, true);
	_endingKachujin->GetComponent<Animator>()->Play("Idle", 0.1f, true);
	_endingKunho->GetComponent<Animator>()->Play("Idle", 0.1f, true);

	// boss
	if (_endingTimer <= 10.0f && _endingTimer > 5.0f)
	{
		_endingSoundComp->Play(endingSoundindex);
		endingBoss->GetComponent<Animator>()->Play("BattleCry", 0.5f, true);
	}
	else if (_endingTimer <= 5.0f && _endingTimer >= 0.0f)
	{
		endingBoss->GetComponent<Animator>()->Play("BattleCry", 10.f, true);

		endingBoss->GetComponent<MeshRenderer>()->SetIsDissolve(true);
		endingBoss->GetComponent<MeshRenderer>()->SetDissolve(_endingTimer / 5);
	}

	// player
	if (_endingTimer <= 10.0f && _endingTimer > 2.f)
	{
		endingPlayer->GetComponent<Animator>()->Play("Idle", 10.0f, true);
	}
	else if (_endingTimer <= 2.0f && _endingTimer > -1.5f)
	{
		endingRock->GetComponent<MeshRenderer>()->SetIsDissolve(true);
		endingRock->GetComponent<MeshRenderer>()->SetDissolve(_endingTimer);

		endingPlayer->GetComponent<Animator>()->Play("fallingEnding", 10.f, true);
		EventManager::GetInstance().ActivateEndingFadeTrigger();
	}

	if (_endingTimer <= -2.0f)
	{
		//_endingLight->GetComponent<Light>()->SetDirection(1.0f, 1.0f, -1.f);

		//_endingCamera->GetComponent<Transform>()->SetPosition({ -322.f, 100.0f,-148.130f });
		//_endingCamera->GetComponent<Transform>()->SetRotation(-182.f, 149.f, 0.f);
		EventManager::GetInstance().SetCamera("EndingCameraAfter");
		_endingCameraAfter->GetComponent<Transform>()->SetPosition({ -322.f, 100.0f,-148.130f });
		_endingCameraAfter->GetComponent<Transform>()->SetRotation(-182.f, 149.f, 0.0f);

		endingPlayer->GetComponent<Transform>()->SetPosition(-261.0f, 92.f, -255.0f);
		endingPlayer->GetComponent<Transform>()->SetRotation(3.0f, 158.f, 0.0f);
		endingPlayer->GetComponent<Particle>()->SetActive(true);

		_endingSoundComp->SetVolume(40.0f, endingSoundindex);

		endingPlayer->GetComponent<Animator>()->Play("endingFloatingfbx", 1.f, true);
	}

	if (sceneInstance.GetScene("Ending")->GetGameObject("EndingThankYou")->GetComponent<Transform>()->GetPosition().y <= 680.0f)
	{
		EventManager::GetInstance().ActivateFadeOutTrigger();
	}
}

float KunrealEngine::EngineCore::CountPlus(float start, float end, float speed)
{
	auto timer = TimeManager::GetInstance().GetDeltaTime();

	float incresment = timer * speed;

	start += incresment;

	if (start >= end)
	{
		return end;
	}
	else
	{
		return start;
	}
}

float KunrealEngine::EngineCore::CountPlusLoop(float start, float end, float speed)
{
	auto timer = TimeManager::GetInstance().GetDeltaTime();

	float incresment = timer * speed;

	start += incresment;

	if (start >= end)
	{
		return _originalYpos;
	}
	else
	{
		return start;
	}
}

float KunrealEngine::EngineCore::CountMinus(float start, float end, float speed)
{
	auto timer = TimeManager::GetInstance().GetDeltaTime();

	float decresement = timer * speed;

	start -= decresement;

	if (start <= end)
	{
		return end;
	}
	else
	{
		return start;
	}
}

void KunrealEngine::EngineCore::SmoothTransition(DirectX::XMFLOAT3 startPos, DirectX::XMFLOAT3 startRot,
	DirectX::XMFLOAT3 endPos, DirectX::XMFLOAT3 endRot, float duration, float deltaTime)
{
	//static float elapsedTime = 0.0f;


	// 보간 비율 계산 (0에서 1 사이의 값)
	float t = g_rageOfLee / duration;

	// 보간 위치 및 회전 계산
	DirectX::XMFLOAT3 newPos;
	newPos.x = startPos.x + t * (endPos.x - startPos.x);
	newPos.y = startPos.y + t * (endPos.y - startPos.y);
	newPos.z = startPos.z + t * (endPos.z - startPos.z);

	DirectX::XMFLOAT3 newRot;
	newRot.x = startRot.x + t * (endRot.x - startRot.x);
	newRot.y = startRot.y + t * (endRot.y - startRot.y);
	newRot.z = startRot.z + t * (endRot.z - startRot.z);

	// 새로운 위치 및 회전 설정
	titleCamera->GetComponent<Transform>()->SetPosition(newPos.x, newPos.y, newPos.z);
	//titleCamera->GetComponent<Transform>()->SetRotation(newRot.x, newRot.y, newRot.z);


	if (g_test >= -45.5f)
	{
		auto deltaTime = TimeManager::GetInstance().GetDeltaTime() * -11.f;
		titleCamera->GetComponent<Camera>()->CameraRotateY(deltaTime);
		g_test += deltaTime;
	}

	//g_rageOfLee += newRot.x;

	// 경과 시간 업데이트
	g_rageOfLee += deltaTime;
	if (g_rageOfLee > duration)
	{
		g_rageOfLee = duration; // 타이머를 끝으로 고정
	}
}

void KunrealEngine::EngineCore::BeforeStartTutorial()
{
	auto& scene = SceneManager::GetInstance();
	ImageRenderer* image;
	ButtonSystem* butt;

	GameObject* tutorialFade = scene.GetScene("Title")->CreateObject("tutorialFade");
	tutorialFade->GetComponent<Transform>()->SetPosition(0.0f, 0.0f, 0.0f);
	image = tutorialFade->AddComponent<ImageRenderer>();
	image->SetImage("ui/blackBackgroundLarge.png");
	image->SetAlpha(0.5f);
	tutorialFade->SetActive(false);

	GameObject* tutorialImage = scene.GetScene("Title")->CreateObject("tutorialImage");
	tutorialImage->GetComponent<Transform>()->SetPosition(320.0f, 180.0f, 0.0f);
	image = tutorialImage->AddComponent<ImageRenderer>();
	image->SetImage("ui/SkillExplain/control_tip.png");
	butt = tutorialImage->AddComponent<ButtonSystem>();
	butt->SetImage(image);
	butt->SetButtonFunc([this]()
		{
			Coroutine_Func(tutorialFadeout)
			{
				auto thisp = this;

				auto& scene = SceneManager::GetInstance();
				float speed = 10.0f;
				float alpha = 1.0f;
				while (1)
				{
					alpha -= TimeManager::GetInstance().GetDeltaTime() * speed;

					scene.GetCurrentScene()->GetGameObject("tutorialFade")->GetComponent<ImageRenderer>()->SetAlpha(alpha / 2);
					scene.GetCurrentScene()->GetGameObject("tutorialImage")->GetComponent<ImageRenderer>()->SetAlpha(alpha);

					if (alpha < 0)
					{
						scene.GetCurrentScene()->GetGameObject("tutorialFade")->GetComponent<ImageRenderer>()->SetAlpha(0.5f);
						scene.GetCurrentScene()->GetGameObject("tutorialImage")->GetComponent<ImageRenderer>()->SetAlpha(1.0f);
						scene.GetCurrentScene()->GetGameObject("tutorialFade")->SetActive(false);
						scene.GetCurrentScene()->GetGameObject("tutorialImage")->SetActive(false);
						break;
					}
					Return_null;
				}
				scene.GetCurrentScene()->GetGameObject("TitlePlayer")->GetComponent<Player>()->SetPlayerIdle();
			};
			Startcoroutine(tutorialFadeout);
		});
	tutorialImage->SetActive(false);
}