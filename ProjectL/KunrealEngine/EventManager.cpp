#include "GameObject.h"
#include "Player.h"
#include "PlayerAbility.h"
#include "PlayerMove.h"
#include "Kamen.h"
#include "ToolBox.h"
#include "EventManager.h"
#include "Camera.h"
#include "MeshCollider.h"
#include "ToolBox.h"
#include "Navigation.h"
#include "UIPackageFuntion.hpp"
#include "EngineCore.h"

KunrealEngine::EventManager::EventManager()
	:_player(nullptr), _boss(nullptr), _playerComp(nullptr), _bossComp(nullptr), _playerAbill(nullptr),
	_eventStart(false), _mainCamera(nullptr), _insideSafeCount(0), _iscamfollow(false)
	, _fadeObjectTitle(nullptr), _fadeObjectMain(nullptr), _fadeObjectEnding(nullptr), _fadeObjectWhiteMain(nullptr), _fadeObjectWhiteEnding(nullptr), _core(nullptr), _difficulty(eDifficulty::eEasy)
{

}

KunrealEngine::EventManager::~EventManager()
{

}

void KunrealEngine::EventManager::Initialize()
{
	SetBossObject();

	_eventStart = true;
}

void KunrealEngine::EventManager::Release()
{
	_eventStart = false;

	_playerAbill = nullptr;
	_bossComp = nullptr;
	_playerComp = nullptr;

	_boss = nullptr;
	_player = nullptr;
}

void KunrealEngine::EventManager::Update()
{
	if (_eventStart)
	{
		CalculateDamageToPlayer();

		CalculateDamageToPlayer2();
	}

	UIcontrol();

	if (_player != nullptr)
	{
		if (InputSystem::GetInstance()->KeyDown(KEY::SPACE) && _player->GetComponent<PlayerMove>()->GetisDashed())
		{
			_CoroutineIs(DashCamMove)
			{
				// 캠 팔로를 끈다.
				_iscamfollow = false;
				// 시간을 0.1초 지연시킨다
				// 플레이어의 타겟 포지션을 받아온다.
				DirectX::XMFLOAT3 targetpos = _player->GetComponent<PlayerMove>()->GetTargetPosition();
				///GetCurrentScene()->CreateObject("targetpos")->GetComponent<Transform>()->SetPosition(targetpos);
				// 플레이어의 위치를 받아온다.
				DirectX::XMFLOAT3 playerpos = _player->GetComponent<Transform>()->GetPosition();
				///GetCurrentScene()->CreateObject("playerpos")->GetComponent<Transform>()->SetPosition(playerpos);
				Waitforsecond(0.1f);

				// 카메라의 위치를 받아온다
				DirectX::XMFLOAT3 campos = _mainCamera->GetComponent<Transform>()->GetPosition();
				DirectX::XMFLOAT3 fixedcampos = campos; // 선형보간을 위한 복사본이다.
				// 플레이어의 위치와 타겟 포지션의 위치를 빼서 이동할 벡터를 구한다.
				DirectX::XMFLOAT3 movepos = ToolBox::XMFLOAT3Subtract(targetpos, playerpos);
				// 플레이어의 위치와 카메라의 위치를 빼서 카메라와의 거리차이를 구한다.
				DirectX::XMFLOAT3 movevec = ToolBox::XMFLOAT3Subtract(playerpos, campos);
				// 이동할 벡터를 카메라의 위치에 더해 타겟포지션을 구한다.
				DirectX::XMFLOAT3 camTarget = ToolBox::XMFLOAT3Add(campos, movepos);
				camTarget.y = campos.y;

				// 다음은 카메라를 더 앞으로 빼야한다. 그러려면 일단 방향벡터를 얻어야한다.
				DirectX::XMFLOAT3 basicvec(0, 0, 0);	// 방향벡터를 구하기 위한 원점
				DirectX::XMFLOAT3 dirvec;				// 방향벡터를 담을 XMFLOAT3
				// 방향벡터를 구해서 담는다.
				DirectX::XMStoreFloat3(&dirvec, ToolBox::GetDirectionVec(basicvec, movepos));
				// 앞으로 얼마나 더 나아갈 것인지?
				float range = 3.0f;
				// 얼마나 더 나아갈지를 계산한다.
				dirvec = DirectX::XMFLOAT3(dirvec.x * range, dirvec.y * range, dirvec.z * range);
				// 그리고 그걸 camtarget에 더해서 목표지점을 만들어 준다.
				DirectX::XMFLOAT3 camoverTarget = DirectX::XMFLOAT3 \
					(dirvec.x + camTarget.x, dirvec.y + camTarget.y, dirvec.z + camTarget.z);

				// 거리계산시 오차
				float distancevalue = 0.1f;

				// 선형보간시 일정하게 감소하는 t값
				float t = 0.0f;
				//while문을 돌리자 조건은 카메라와 타겟 위치사이의 거리
				while (ToolBox::GetDistance(camoverTarget, campos) > distancevalue)
				{
					if (t > 1.0f)
					{
						break;
					}
					// t값을 더해준다.
					t += 0.1;
					// for문 조건 연산을 위해 campos를 받아온다.
					campos = _mainCamera->GetComponent<Transform>()->GetPosition();
					// 카메라부터 타겟 포지션까지의 거리를 0~1로 두고 선형보간을 통해 한프레임마다의 위치를 구한다.
					DirectX::XMFLOAT3 result = ToolBox::Lerp(fixedcampos, camoverTarget, t);
					// 이제 카메라를 거기로 옮기자
					_mainCamera->GetComponent<Transform>()->SetPosition(result);

					Return_null; // 이번 루프를 종료
				}

				// 이번엔 역방향으로!
				t = 0.0f;
				while (t > 1.0f)
				{
					// t값을 더해준다.
					t += 0.1;
					// for문 조건 연산을 위해 campos를 받아온다.
					campos = _mainCamera->GetComponent<Transform>()->GetPosition();
					// 카메라부터 타겟 포지션까지의 거리를 0~1로 두고 선형보간을 통해 한프레임마다의 위치를 구한다.
					// 이거 때문에 camTarget을 살려뒀지
					DirectX::XMFLOAT3 result = ToolBox::Lerp(camoverTarget, camTarget, t);
					// 이제 카메라를 거기로 옮기자
					_mainCamera->GetComponent<Transform>()->SetPosition(result);

					Return_null; // 이번 루프를 종료
				}
				// 다 끝났으니 카메라추적을 다시 켠다.
				_iscamfollow = true;
			};
			Startcoroutine(DashCamMove);
		}
	}

	if (_iscamfollow && _mainCamera != nullptr && _player != nullptr)
	{
		DirectX::XMFLOAT3 plpos = _player->GetComponent<Transform>()->GetPosition();
		static DirectX::XMFLOAT3 capos = _mainCamera->GetComponent<Transform>()->GetPosition();

		plpos.x += capos.x + _camshakex;
		plpos.y += capos.y;
		plpos.z += capos.z + _camshakez;

		_mainCamera->GetComponent<Transform>()->SetPosition(plpos);
	}
}

void KunrealEngine::EventManager::SetCamera(std::string name)
{
	_mainCamera = SceneManager::GetInstance().GetCurrentScene()->GetGameObject(name);
	_mainCamera->GetComponent<Camera>()->SetMainCamera();
}

std::vector<DirectX::XMFLOAT2> KunrealEngine::EventManager::CamShake(float radius, int numPoints)
{
	// 원형 카메라 회전
	// 원의 테두리에서 정점을 numpoint 만큼 추려낸 후, 그 점들을 반환.
	// 원들의 위치를 순차적으로 적용한다면 카메라는 원을 그리며 흔들린다.(목표)
#define M_PI       3.14159265358979323846
	std::vector<DirectX::XMFLOAT2> result;

	for (int i = 0; i < numPoints; ++i) {
		float random = ToolBox::GetRandomFloat(0.0f, radius * M_PI);
		float theta = random;
		float x = radius * std::cos(theta);
		float y = radius * std::sin(theta);
		result.push_back(DirectX::XMFLOAT2(x, y));
	}

	return result;
}

std::vector<float> KunrealEngine::EventManager::CamShakeLinear(float sigma, int numPoints)
{
	// 균등감소 카메라 흔들기
	// sigma값 범위 만큼 카메라를 흔든다. 이 sigma 값은 점점 작아진다.
	// 이때의 좌우로 진동 하는 횟수는 numpoint 값이다.
	std::vector<float> result;
	float cur_sigma = sigma;

	for (float a = 0; a < numPoints; a++)
	{
		// 현재 진동 강도를 리스트에 추가 (양수)
		result.push_back(static_cast<int>(cur_sigma));
		// 현재 진동 강도를 리스트에 추가 (음수)
		result.push_back(static_cast<int>(-cur_sigma));
		// 진동 강도 감소
		cur_sigma -= sigma / numPoints;
	}
	result.push_back(static_cast<int>(0));

	return result;
}

void KunrealEngine::EventManager::CamShake(float harder)
{
	// 무작위 카메라 흔들기. 
	// 그냥 지정된 범위 내 랜덤값을 때려박는다.
	_camshakex = ToolBox::GetRandomFloat(0.0f, harder);
	_camshakez = ToolBox::GetRandomFloat(0.0f, harder);
}


void KunrealEngine::EventManager::BattleUIHide(float speed)
{
	SceneManager& scene = SceneManager::GetInstance();
	auto* booshpbar_pack = scene.GetCurrentScene()->GetGameObject("boss_pack");
	auto* battleui_pack = scene.GetCurrentScene()->GetGameObject("battle_pack");

	float bosstarget = -120.0f;
	float battleuitarget = 600.0f;

	DirectX::XMFLOAT3 bosspos = booshpbar_pack->GetComponent<Transform>()->GetPosition();
	DirectX::XMFLOAT3 uipos = battleui_pack->GetComponent<Transform>()->GetPosition();

	if (bosspos.y > bosstarget)
	{
		bosspos.y -= speed * TimeManager::GetInstance().GetDeltaTime();
		booshpbar_pack->GetComponent<Transform>()->SetPosition(bosspos);
	}

	if (uipos.y < battleuitarget)
	{
		uipos.y += speed * TimeManager::GetInstance().GetDeltaTime();
		battleui_pack->GetComponent<Transform>()->SetPosition(uipos);
	}
}

void KunrealEngine::EventManager::BattleUIOpen(float speed)
{
	SceneManager& scene = SceneManager::GetInstance();

	scene.GetInstance().GetScene("Main")->GetGameObject("BattleUI")->SetActive(true);

	auto* booshpbar_pack = scene.GetCurrentScene()->GetGameObject("boss_pack");
	auto* battleui_pack = scene.GetCurrentScene()->GetGameObject("battle_pack");

	float bosstarget = 0.0f;
	float battleuitarget = 365.0f;

	DirectX::XMFLOAT3 bosspos = booshpbar_pack->GetComponent<Transform>()->GetPosition();
	DirectX::XMFLOAT3 uipos = battleui_pack->GetComponent<Transform>()->GetPosition();

	if (bosspos.y < bosstarget)
	{
		bosspos.y += speed * TimeManager::GetInstance().GetDeltaTime();
		booshpbar_pack->GetComponent<Transform>()->SetPosition(bosspos);
	}

	if (uipos.y > battleuitarget)
	{
		uipos.y -= speed * TimeManager::GetInstance().GetDeltaTime();
		battleui_pack->GetComponent<Transform>()->SetPosition(uipos);
	}
}

void KunrealEngine::EventManager::ActiveVbutton(float speed, float xpos, float ypos)
{
	static GameObject* Vbutton = SceneManager::GetInstance().GetCurrentScene()->CreateObject("vbutton");

	Vbutton->SetActive(true);
	
	if (_Vbutton == nullptr)
	{
		_Vbutton = Vbutton;
		_Vbutton->AddComponent<ImageRenderer>()->SetImage("ui/FinalV.png");
		_Vbutton->GetComponent<ImageRenderer>()->SetPosition(xpos, ypos);
	}
}

void KunrealEngine::EventManager::ActiveSpaceButton(float time, float xpos, float ypos)
{
	static GameObject* Spacebutton = SceneManager::GetInstance().GetCurrentScene()->CreateObject("spacebutton");

	Spacebutton->SetActive(true);
	if (_Spacebutton == nullptr)
	{
		_Spacebutton = Spacebutton;
		_Spacebutton->AddComponent<ImageRenderer>()->SetImage("ui/EndingSpace1.png");
		_Spacebutton->GetComponent<ImageRenderer>()->SetPosition(xpos, ypos);
	}
	_Vbutton->SetActive(false);

	static float timer = 0.0f;
	static bool first = true;
	timer += TimeManager::GetInstance().GetDeltaTime();
	if(timer > time)
	{
		timer = 0.0f;
		first = first ? false : true;
	}

	if (first)
	{
		_Spacebutton->GetComponent<ImageRenderer>()->ChangeImage("ui/EndingSpace1.png");
	}
	else if (!first)
	{
		_Spacebutton->GetComponent<ImageRenderer>()->ChangeImage("ui/EndingSpace2.png");
	}
}

void KunrealEngine::EventManager::CreateFadeObject()
{
	this->_fadeObjectTitle = SceneManager::GetInstance().GetScene("Title")->CreateObject("FadeObject");
	this->_fadeObjectTitle->AddComponent<ImageRenderer>();
	this->_fadeObjectTitle->GetComponent<ImageRenderer>()->SetImage("ui/blackBackgroundLarge.png");
	this->_fadeObjectTitle->GetComponent<ImageRenderer>()->SetAlpha(0.0f);
	this->_fadeObjectTitle->SetActive(false);

	this->_fadeObjectMain = SceneManager::GetInstance().GetScene("Main")->CreateObject("FadeObject");
	this->_fadeObjectMain->AddComponent<ImageRenderer>();
	this->_fadeObjectMain->GetComponent<ImageRenderer>()->SetImage("ui/blackBackgroundLarge.png");
	this->_fadeObjectMain->GetComponent<ImageRenderer>()->SetAlpha(0.0f);
	this->_fadeObjectMain->SetActive(false);

	this->_fadeObjectEnding = SceneManager::GetInstance().GetScene("Ending")->CreateObject("FadeObject");
	this->_fadeObjectEnding->AddComponent<ImageRenderer>();
	this->_fadeObjectEnding->GetComponent<ImageRenderer>()->SetImage("ui/blackBackgroundLarge.png");
	this->_fadeObjectEnding->GetComponent<ImageRenderer>()->SetAlpha(0.0f);
	this->_fadeObjectEnding->SetActive(false);

	/// 하얀 fade는 불가 alpha 값이 먹질 않음
	this->_fadeObjectWhiteMain = SceneManager::GetInstance().GetScene("Main")->CreateObject("FadeObjectWhite");
	this->_fadeObjectWhiteMain->AddComponent<ImageRenderer>();
	this->_fadeObjectWhiteMain->GetComponent<ImageRenderer>()->SetImage("ui/blackBackgroundLarge.png");
	this->_fadeObjectWhiteMain->GetComponent<ImageRenderer>()->SetAlpha(0.0f);
	this->_fadeObjectWhiteMain->SetActive(false);

	this->_fadeObjectWhiteEnding = SceneManager::GetInstance().GetScene("Ending")->CreateObject("FadeObjectWhite");
	this->_fadeObjectWhiteEnding->AddComponent<ImageRenderer>();
	this->_fadeObjectWhiteEnding->GetComponent<ImageRenderer>()->SetImage("ui/blackBackgroundLarge.png");
	this->_fadeObjectWhiteEnding->GetComponent<ImageRenderer>()->SetAlpha(0.0f);
	this->_fadeObjectWhiteEnding->SetActive(false);
}

void KunrealEngine::EventManager::CalculateDamageToBoss(Ability* abil)
{
	auto damage = abil->GetDamage() * _playerComp->GetPlayerData()._spellPower;

	float finalDamage = 1 / _bossComp->GetBossInfo()._armor;

	this->_bossComp->GetBossInfo()._hp -= damage * (1 / _bossComp->GetBossInfo()._armor);
}

void KunrealEngine::EventManager::CalculateDamageToPlayer()
{
	auto nowPattern = _bossComp->GetNowPlayingPattern();

	// 보스가 공격중일때만?
	if (nowPattern != nullptr)
	{
		// 콜라이더가 없을 경우
		if (nowPattern->_colliderType == BossPattern::eColliderType::eNone)
		{
			return;
		}

		// 박스 콜라이더나 실린더 콜라이더일 경우
		else if (nowPattern->_colliderType == BossPattern::eColliderType::eBox)
		{
			auto subObjectList = nowPattern->_subObject;

			for (int i = 0; i < subObjectList.size(); i++)
			{
				auto collider = subObjectList[i]->GetComponent<BoxCollider>();

				if (collider != nullptr && !nowPattern->_isColliderActive[i])
				{
					collider->SetActive(false);
				}

				// 콜라이더가 켜졌지만 맞지 않은 상태
				if (nowPattern->_isColliderActive[i] && !nowPattern->_isColliderHit[i])
				{
					collider->FixedUpdate();

					collider->SetActive(true);

					if (collider->IsCollided() && collider->GetTargetObject() == _player)
					{
						// 보스와 서브 오브젝트 사이의 디렉션으로 넘어뜨림
						if (nowPattern->_attackState == BossPattern::eAttackState::ePush)
						{
							if (subObjectList[i]->GetObjectName().find("Ego") == std::string::npos)
							{
								auto colliderDirVec = SetBossAttackDirection(subObjectList[i]);

								_playerComp->CalculateSweep(colliderDirVec);
							}
							else
							{
								auto colliderDirVec = SetEgoAttackDirection(subObjectList[i]);

								_playerComp->CalculateSweep(colliderDirVec);
							}
						}

						auto damage = nowPattern->_damage;

						// 플레이어의 hp에서 패턴의 데미지만큼 차감시킴
						_playerComp->GetPlayerData()._hp -= damage;
						_playerComp->SetHitState(static_cast<int> (nowPattern->_attackState));

						// 데미지가 들어간 후 메쉬를 꺼야한다면
						if (!nowPattern->_isRemainMesh && subObjectList[i]->GetComponent<MeshRenderer>() != nullptr)
						{
							// 메쉬를 꺼버림
							subObjectList[i]->GetComponent<MeshRenderer>()->SetActive(false);
						}

						// 맞았다고 체크
						nowPattern->_isColliderHit[i] = true;
					}
				}

				// 콜라이더가 켜지고 맞은 상태
				if (nowPattern->_isColliderActive[i] && nowPattern->_isColliderHit[i])
				{
					// 맞았으니 콜라이더 끄기
					nowPattern->_isColliderActive[i] = false;

					collider->SetActive(false);
				}
			}
		}

		else if (nowPattern->_colliderType == BossPattern::eColliderType::eCylinder)
		{
			auto subObjectList = nowPattern->_subObject;

			for (int i = 0; i < subObjectList.size(); i++)
			{
				auto collider = subObjectList[i]->GetComponent<CylinderCollider>();

				if (collider != nullptr && !nowPattern->_isColliderActive[i])
				{
					collider->SetActive(false);
				}

				// 콜라이더가 켜졌지만 맞지 않은 상태
				if (nowPattern->_isColliderActive[i] && !nowPattern->_isColliderHit[i])
				{
					collider->FixedUpdate();

					collider->SetActive(true);

					if (collider->IsCollided() && collider->GetTargetObject() == _player)
					{
						if (nowPattern->_attackState == BossPattern::eAttackState::ePush)
						{
							auto colliderDirVec = SetWarningAttackDirection(subObjectList[i]);
							_playerComp->CalculateSweep(colliderDirVec);
						}

						auto damage = nowPattern->_damage;

						// 플레이어의 hp에서 패턴의 데미지만큼 차감시킴
						_playerComp->GetPlayerData()._hp -= damage;
						_playerComp->SetHitState(static_cast<int> (nowPattern->_attackState));

						// 데미지가 들어간 후 메쉬를 꺼야한다면
						if (!nowPattern->_isRemainMesh && subObjectList[i]->GetComponent<MeshRenderer>() != nullptr)
						{
							// 메쉬를 꺼버림
							subObjectList[i]->GetComponent<MeshRenderer>()->SetActive(false);
						}

						// 맞았다고 체크
						nowPattern->_isColliderHit[i] = true;
					}
				}

				// 콜라이더가 켜지고 맞은 상태
				if (nowPattern->_isColliderActive[i] && nowPattern->_isColliderHit[i])
				{
					// 맞았으니 콜라이더 끄기
					nowPattern->_isColliderActive[i] = false;

					collider->SetActive(false);
				}
			}
		}


		else if (nowPattern->_colliderType == BossPattern::eColliderType::eSwordMultiple)
		{
			auto subObjectList = nowPattern->_subObject;

			for (int i = 0; i < subObjectList.size(); i++)
			{
				auto collider = subObjectList[i]->GetComponent<BoxCollider>();

				if (collider != nullptr && !nowPattern->_isColliderActive[i])
				{
					collider->SetActive(false);
				}

				// 콜라이더가 켜졌지만 맞지 않은 상태
				if (nowPattern->_isColliderActive[i] && !nowPattern->_isColliderHit[i])
				{
					collider->FixedUpdate();

					collider->SetActive(true);

					if (collider->IsCollided() && collider->GetTargetObject() == _player)
					{
						auto colliderDirVec = _bossComp->GetMultipleSwordDirVec(i);
						colliderDirVec.y = 0.0f;
						_playerComp->CalculateSweep(colliderDirVec);

						auto damage = nowPattern->_damage;

						// 플레이어의 hp에서 패턴의 데미지만큼 차감시킴
						_playerComp->GetPlayerData()._hp -= damage;
						_playerComp->SetHitState(static_cast<int> (nowPattern->_attackState));


						// 맞았다고 체크
						nowPattern->_isColliderHit[i] = true;
					}
				}

				// 콜라이더가 켜지고 맞은 상태
				if (nowPattern->_isColliderActive[i] && nowPattern->_isColliderHit[i])
				{
					// 맞았으니 콜라이더 끄기
					nowPattern->_isColliderActive[i] = false;

					collider->SetActive(false);
				}
			}
		}


		else if (nowPattern->_colliderType == BossPattern::eColliderType::eSwordDirection)
		{
			auto subObjectList = nowPattern->_subObject;

			for (int i = 0; i < subObjectList.size(); i++)
			{
				auto collider = subObjectList[i]->GetComponent<BoxCollider>();

				if (collider != nullptr && !nowPattern->_isColliderActive[i])
				{
					collider->SetActive(false);
				}

				// 콜라이더가 켜졌지만 맞지 않은 상태
				if (nowPattern->_isColliderActive[i] && !nowPattern->_isColliderHit[i])
				{
					collider->FixedUpdate();

					collider->SetActive(true);

					if (collider->IsCollided() && collider->GetTargetObject() == _player)
					{
						if (nowPattern->_attackState == BossPattern::eAttackState::ePush)
						{
							auto playerPos = _player->GetComponent<Transform>()->GetPosition();
							auto bossPos = _boss->GetComponent<Transform>()->GetPosition();

							auto colliderDirVec = ToolBox::GetDirectionVec(bossPos, playerPos);

							colliderDirVec.m128_f32[1] = 0.0f;

							_playerComp->CalculateSweep(colliderDirVec);
						}

						auto damage = nowPattern->_damage;

						// 플레이어의 hp에서 패턴의 데미지만큼 차감시킴
						_playerComp->GetPlayerData()._hp -= damage;
						_playerComp->SetHitState(static_cast<int> (nowPattern->_attackState));


						// 맞았다고 체크
						nowPattern->_isColliderHit[i] = true;
					}
				}

				// 콜라이더가 켜지고 맞은 상태
				if (nowPattern->_isColliderActive[i] && nowPattern->_isColliderHit[i])
				{
					// 맞았으니 콜라이더 끄기
					nowPattern->_isColliderActive[i] = false;

					collider->SetActive(false);
				}
			}
		}

		else if (nowPattern->_colliderType == BossPattern::eColliderType::eSwordColliderWithMeshCollider)
		{
			auto subObjectList = nowPattern->_subObject;

			for (int i = 0; i < subObjectList.size(); i++)
			{
				auto collider = subObjectList[i]->GetComponent<MeshCollider>();

				if (collider == nullptr)
				{
					auto collider = subObjectList[i]->GetComponent<BoxCollider>();

					if (collider != nullptr && !nowPattern->_isColliderActive[i])
					{
						collider->SetActive(false);
					}

					// 콜라이더가 켜졌지만 맞지 않은 상태
					if (nowPattern->_isColliderActive[i] && !nowPattern->_isColliderHit[i])
					{
						collider->FixedUpdate();

						collider->SetActive(true);

						if (collider->IsCollided() && collider->GetTargetObject() == _player)
						{
							auto playerPos = _player->GetComponent<Transform>()->GetPosition();
							auto bossPos = _boss->GetComponent<Transform>()->GetPosition();

							auto colliderDirVec = ToolBox::GetDirectionVec(bossPos, playerPos);

							colliderDirVec.m128_f32[1] = 0.0f;

							_playerComp->CalculateSweep(colliderDirVec);


							auto damage = nowPattern->_damage;

							// 플레이어의 hp에서 패턴의 데미지만큼 차감시킴
							_playerComp->GetPlayerData()._hp -= damage;
							_playerComp->SetHitState(static_cast<int> (nowPattern->_attackState));


							// 맞았다고 체크
							nowPattern->_isColliderHit[i] = true;
						}
					}

					// 콜라이더가 켜지고 맞은 상태
					if (nowPattern->_isColliderActive[i] && nowPattern->_isColliderHit[i])
					{
						// 맞았으니 콜라이더 끄기
						nowPattern->_isColliderActive[i] = false;

						collider->SetActive(false);
					}
				}

				else
				{
					if (collider != nullptr && !nowPattern->_isColliderActive[i])
					{
						collider->SetActive(false);
					}

					// 콜라이더가 켜졌지만 맞지 않은 상태
					if (nowPattern->_isColliderActive[i] && !nowPattern->_isColliderHit[i])
					{
						collider->FixedUpdate();

						collider->SetActive(true);

						if (collider->IsCollided() && collider->GetTargetObject() == _player)
						{
							auto playerPos = _player->GetComponent<Transform>()->GetPosition();
							auto bossPos = _boss->GetComponent<Transform>()->GetPosition();

							auto colliderDirVec = SetBossAttackDirection(subObjectList[i]);

							_playerComp->CalculateSweep(colliderDirVec);


							auto damage = nowPattern->_damage;

							// 플레이어의 hp에서 패턴의 데미지만큼 차감시킴
							_playerComp->GetPlayerData()._hp -= damage;
							_playerComp->SetHitState(static_cast<int> (nowPattern->_attackState));


							// 맞았다고 체크
							nowPattern->_isColliderHit[i] = true;
						}
					}

					// 콜라이더가 켜지고 맞은 상태
					if (nowPattern->_isColliderActive[i] && nowPattern->_isColliderHit[i])
					{
						// 맞았으니 콜라이더 끄기
						nowPattern->_isColliderActive[i] = false;

						collider->SetActive(false);
					}
				}
			}
		}


		else if (nowPattern->_colliderType == BossPattern::eColliderType::eSwordColliderWithCylinderCollider)
		{
			auto subObjectList = nowPattern->_subObject;

			for (int i = 0; i < subObjectList.size(); i++)
			{
				auto collider = subObjectList[i]->GetComponent<CylinderCollider>();

				if (collider == nullptr)
				{
					auto collider = subObjectList[i]->GetComponent<BoxCollider>();

					if (collider != nullptr && !nowPattern->_isColliderActive[i])
					{
						collider->SetActive(false);
					}

					// 콜라이더가 켜졌지만 맞지 않은 상태
					if (nowPattern->_isColliderActive[i] && !nowPattern->_isColliderHit[i])
					{
						collider->FixedUpdate();

						collider->SetActive(true);

						if (collider->IsCollided() && collider->GetTargetObject() == _player)
						{
							auto playerPos = _player->GetComponent<Transform>()->GetPosition();
							auto bossPos = _boss->GetComponent<Transform>()->GetPosition();

							auto colliderDirVec = ToolBox::GetDirectionVec(bossPos, playerPos);

							colliderDirVec.m128_f32[1] = 0.0f;

							_playerComp->CalculateSweep(colliderDirVec);


							auto damage = nowPattern->_damage;

							// 플레이어의 hp에서 패턴의 데미지만큼 차감시킴
							_playerComp->GetPlayerData()._hp -= damage;
							_playerComp->SetHitState(static_cast<int> (nowPattern->_attackState));


							// 맞았다고 체크
							nowPattern->_isColliderHit[i] = true;
						}
					}

					// 콜라이더가 켜지고 맞은 상태
					if (nowPattern->_isColliderActive[i] && nowPattern->_isColliderHit[i])
					{
						// 맞았으니 콜라이더 끄기
						nowPattern->_isColliderActive[i] = false;

						collider->SetActive(false);
					}
				}

				else
				{
					if (collider != nullptr && !nowPattern->_isColliderActive[i])
					{
						collider->SetActive(false);
					}

					// 콜라이더가 켜졌지만 맞지 않은 상태
					if (nowPattern->_isColliderActive[i] && !nowPattern->_isColliderHit[i])
					{
						collider->FixedUpdate();

						collider->SetActive(true);

						if (collider->IsCollided() && collider->GetTargetObject() == _player)
						{

							auto playerPos = _player->GetComponent<Transform>()->GetPosition();
							auto colliderPos = subObjectList[i]->GetComponent<Transform>()->GetPosition();

							auto distance = ToolBox::GetDistance(colliderPos, playerPos);

							if (20 <= distance && distance <= 50)
							{
								return;
							}

							auto colliderDirVec = SetWarningAttackDirection(subObjectList[i]);
							_playerComp->CalculateSweep(colliderDirVec);

							auto damage = nowPattern->_damage;

							// 플레이어의 hp에서 패턴의 데미지만큼 차감시킴
							_playerComp->GetPlayerData()._hp -= damage;
							_playerComp->SetHitState(static_cast<int> (nowPattern->_attackState));

							// 데미지가 들어간 후 메쉬를 꺼야한다면
							if (!nowPattern->_isRemainMesh && subObjectList[i]->GetComponent<MeshRenderer>() != nullptr)
							{
								// 메쉬를 꺼버림
								subObjectList[i]->GetComponent<MeshRenderer>()->SetActive(false);
							}

							//맞았다고 체크
							nowPattern->_isColliderHit[i] = true;
						}
					}

					// 콜라이더가 켜지고 맞은 상태
					if (nowPattern->_isColliderActive[i] && nowPattern->_isColliderHit[i])
					{
						// 맞았으니 콜라이더 끄기
						nowPattern->_isColliderActive[i] = false;

						collider->SetActive(false);
					}
				}
			}
		}


	}
}


void KunrealEngine::EventManager::CalculateDamageToPlayer2()
{
	auto nowPattern = _bossComp->GetNowSpecialPattern();

	if (nowPattern != nullptr)
	{
		// 콜라이더가 없을 경우
		if (nowPattern->_colliderType == BossPattern::eColliderType::eNone)
		{
			return;
		}

		// 박스 콜라이더일 경우
		else if (nowPattern->_colliderType == BossPattern::eColliderType::eBox)
		{
			auto subObjectList = nowPattern->_subObject;

			for (int i = 0; i < subObjectList.size(); i++)
			{
				auto collider = subObjectList[i]->GetComponent<BoxCollider>();

				if (collider != nullptr && !nowPattern->_isColliderActive[i])
				{
					collider->SetActive(false);
				}

				// 콜라이더가 켜졌지만 맞지 않은 상태
				if (nowPattern->_isColliderActive[i] && !nowPattern->_isColliderHit[i])
				{
					collider->FixedUpdate();

					collider->SetActive(true);

					if (collider->IsCollided() && collider->GetTargetObject() == _player)
					{
						// 소드의 방향 벡터로 넘어뜨림
						_playerComp->CalculateSweep(_bossComp->GetSwordDirection());

						auto damage = nowPattern->_damage;

						// 플레이어의 hp에서 패턴의 데미지만큼 차감시킴
						_playerComp->GetPlayerData()._hp -= damage;
						_playerComp->SetHitState(static_cast<int> (nowPattern->_attackState));

						// 데미지가 들어간 후 메쉬를 꺼야한다면
						if (!nowPattern->_isRemainMesh && subObjectList[i]->GetComponent<MeshRenderer>() != nullptr)
						{
							// 메쉬를 꺼버림
							subObjectList[i]->GetComponent<MeshRenderer>()->SetActive(false);
						}

						// 맞았다고 체크
						nowPattern->_isColliderHit[i] = true;
					}
				}

				// 콜라이더가 켜지고 맞은 상태
				if (nowPattern->_isColliderActive[i] && nowPattern->_isColliderHit[i])
				{
					// 맞았으니 콜라이더 끄기
					nowPattern->_isColliderActive[i] = false;

					collider->SetActive(false);
				}
			}
		}

		// 추후 수정 예정
		else if (nowPattern->_colliderType == BossPattern::eColliderType::eCylinder)
		{
			auto subObjectList = nowPattern->_subObject;

			for (int i = 0; i < subObjectList.size(); i++)
			{
				auto collider = subObjectList[i]->GetComponent<CylinderCollider>();

				if (collider != nullptr && !nowPattern->_isColliderActive[i])
				{
					collider->SetActive(false);
				}

				// 콜라이더가 켜졌지만 맞지 않은 상태
				if (nowPattern->_isColliderActive[i] && !nowPattern->_isColliderHit[i])
				{
					collider->FixedUpdate();

					collider->SetActive(true);

					if (collider->IsCollided() && collider->GetTargetObject() == _player)
					{
						auto colliderDirVec = SetWarningAttackDirection(subObjectList[i]);
						_playerComp->CalculateSweep(colliderDirVec);

						auto damage = nowPattern->_damage;

						// 플레이어의 hp에서 패턴의 데미지만큼 차감시킴
						_playerComp->GetPlayerData()._hp -= damage;
						_playerComp->SetHitState(static_cast<int> (nowPattern->_attackState));

						// 데미지가 들어간 후 메쉬를 꺼야한다면
						if (!nowPattern->_isRemainMesh && subObjectList[i]->GetComponent<MeshRenderer>() != nullptr)
						{
							// 메쉬를 꺼버림
							subObjectList[i]->GetComponent<MeshRenderer>()->SetActive(false);
						}

						// 맞았다고 체크
						nowPattern->_isColliderHit[i] = true;
					}
				}

				// 콜라이더가 켜지고 맞은 상태
				if (nowPattern->_isColliderActive[i] && nowPattern->_isColliderHit[i])
				{
					// 맞았으니 콜라이더 끄기
					nowPattern->_isColliderActive[i] = false;

					collider->SetActive(false);
				}
			}
		}

		else if (nowPattern->_colliderType == BossPattern::eColliderType::eCircleSafe)
		{
			auto subObjectList = nowPattern->_subObject;

			for (int i = 0; i < subObjectList.size(); i++)
			{
				auto collider = subObjectList[i]->GetComponent<CylinderCollider>();

				if (collider != nullptr && !nowPattern->_isColliderActive[i])
				{
					collider->SetActive(false);

					_insideSafeCount = 0;
				}

				// 콜라이더가 켜졌지만 맞지 않은 상태
				if (nowPattern->_isColliderActive[i] && !nowPattern->_isColliderHit[i])
				{
					collider->FixedUpdate();

					collider->SetActive(true);

					if (!collider->IsCollided())
					{
						_insideSafeCount++;

						if (_insideSafeCount > 1)
						{
							auto playerPos = _player->GetComponent<Transform>()->GetPosition();
							playerPos.y = 0.0f;

							auto swordPos = _bossComp->GetGetSwordInsideCenterPos();
							swordPos.y = 0.0f;

							auto distance = ToolBox::GetDistance(swordPos, playerPos);

							if (distance > 30)
							{
								// 소드와 서브오젝트의 콜라이더
								auto colliderDirVec = SetWarningAttackDirection(subObjectList[i]);

								_playerComp->CalculateSweep(colliderDirVec);

								auto damage = nowPattern->_damage;

								// 플레이어의 hp에서 패턴의 데미지만큼 차감시킴
								_playerComp->GetPlayerData()._hp -= damage;
								_playerComp->SetHitState(static_cast<int> (nowPattern->_attackState));

								nowPattern->_isColliderHit[i] = true;
							}
						}
					}
				}

				// 콜라이더가 켜지고 맞은 상태
				if (nowPattern->_isColliderActive[i] && nowPattern->_isColliderHit[i])
				{
					// 맞았으니 콜라이더 끄기
					nowPattern->_isColliderActive[i] = false;

					collider->SetActive(false);
				}
			}
		}

		else if (nowPattern->_colliderType == BossPattern::eColliderType::eDonut)
		{
			auto subObjectList = nowPattern->_subObject;

			for (int i = 0; i < subObjectList.size(); i++)
			{
				auto collider = subObjectList[i]->GetComponent<CylinderCollider>();

				if (collider != nullptr && !nowPattern->_isColliderActive[i])
				{
					collider->SetActive(false);
				}

				// 콜라이더가 켜졌지만 맞지 않은 상태
				if (nowPattern->_isColliderActive[i] && !nowPattern->_isColliderHit[i])
				{
					collider->FixedUpdate();

					collider->SetActive(true);

					if (collider->IsCollided() && collider->GetTargetObject() == _player)
					{

						auto playerPos = _player->GetComponent<Transform>()->GetPosition();
						auto swordPos = _bossComp->GetSwordPos();

						auto distance = ToolBox::GetDistance(swordPos, playerPos);

						if (distance <= _bossComp->GetDonutSize())
						{
							int a = _bossComp->GetDonutSize();
							return;
						}

						auto colliderDirVec = SetWarningAttackDirection(subObjectList[i]);
						_playerComp->CalculateSweep(colliderDirVec);

						auto damage = nowPattern->_damage;

						// 플레이어의 hp에서 패턴의 데미지만큼 차감시킴
						_playerComp->GetPlayerData()._hp -= damage;
						_playerComp->SetHitState(static_cast<int> (nowPattern->_attackState));
						nowPattern->_isColliderHit[i] = true;

						// 데미지가 들어간 후 메쉬를 꺼야한다면
						if (!nowPattern->_isRemainMesh && subObjectList[i]->GetComponent<MeshRenderer>() != nullptr)
						{
							// 메쉬를 꺼버림
							subObjectList[i]->GetComponent<MeshRenderer>()->SetActive(false);
						}

						// 맞았다고 체크
						//nowPattern->_isColliderHit[i] = true;
					}
				}

				// 콜라이더가 켜지고 맞은 상태
				if (nowPattern->_isColliderActive[i] && nowPattern->_isColliderHit[i])
				{
					// 맞았으니 콜라이더 끄기
					nowPattern->_isColliderActive[i] = false;

					collider->SetActive(false);
				}
			}
		}
	}
}

void KunrealEngine::EventManager::SetBossObject()
{
	_player = SceneManager::GetInstance().GetScene("Main")->GetObjectWithTag("Player");
	_boss = SceneManager::GetInstance().GetScene("Main")->GetObjectWithTag("Boss");

	_playerComp = _player->GetComponent<Player>();
	_playerAbill = _player->GetComponent<PlayerAbility>();
	_bossComp = _boss->GetComponent<Kamen>();
}

void KunrealEngine::EventManager::UIcontrol()
{
	static bool premeteorActive = false;
	GameObject* pausebox = SceneManager::GetInstance().GetCurrentScene()->GetGameObject("pauseuibox");
	if (InputSystem::GetInstance()->KeyDown(KEY::ESCAPE) && pausebox != nullptr)
	{
		auto* meteorRange = SceneManager::GetInstance().GetCurrentScene()->GetGameObject("MeteorRange");
		if (meteorRange != nullptr)
		{

			if (!premeteorActive)
			{
				bool active = pausebox->GetActivated() ? false : true;
				pausebox->SetActive(active);
			}

		}
		else
		{
			bool active = pausebox->GetActivated() ? false : true;
			pausebox->SetActive(active);
		}

	}
	premeteorActive = _playerAbill->GetisMeteorBefore();
}


const DirectX::XMVECTOR& KunrealEngine::EventManager::SetWarningAttackDirection(GameObject* subObject)
{
	auto colliderPos = subObject->GetComponent<Transform>()->GetPosition();
	auto playerPos = _player->GetComponent<Transform>()->GetPosition();
	colliderPos.y = playerPos.y;

	auto colliderDirVec = ToolBox::GetDirectionVec(colliderPos, playerPos);

	return colliderDirVec;
}

const DirectX::XMVECTOR& KunrealEngine::EventManager::SetBossAttackDirection(GameObject* subObject)
{
	auto colliderPos = subObject->GetComponent<Transform>()->GetPosition();
	auto bossPos = _boss->GetComponent<Transform>()->GetPosition();
	auto playerPos = _player->GetComponent<Transform>()->GetPosition();

	colliderPos.y = playerPos.y;
	bossPos.y = playerPos.y;

	auto colliderDirVec = ToolBox::GetDirectionVec(bossPos, colliderPos);

	return colliderDirVec;
}

const DirectX::XMVECTOR& KunrealEngine::EventManager::SetEgoAttackDirection(GameObject* subObject)
{
	auto colliderPos = subObject->GetComponent<Transform>()->GetPosition();
	auto egoPos = _bossComp->GetEgoPos();
	auto playerPos = _player->GetComponent<Transform>()->GetPosition();

	colliderPos.y = playerPos.y;
	egoPos.y = playerPos.y;

	auto colliderDirVec = ToolBox::GetDirectionVec(egoPos, colliderPos);

	return colliderDirVec;
}

void KunrealEngine::EventManager::MoveToTitle()
{
	Scene* scene = SceneManager::GetInstance().GetScene("Title");

	// 카메라 재설정 후
	Camera* titleCamera = scene->GetGameObject("TitleCamera")->GetComponent<Camera>();
	scene->GetGameObject("TitleCamera")->GetComponent<Transform>()->SetPosition(-155.0f, 135.0f, -45.130f);

	// 카메라 고정 해제
	this->_iscamfollow = false;

	// 플레이어 위치 초기화
	scene->GetGameObject("TitlePlayer")->GetComponent<Transform>()->SetPosition(-156.0f, 66.0f, 0.0f);
	scene->GetGameObject("TitlePlayer")->GetComponent<Transform>()->SetRotation(0.0f, 45.0f, 0.0f);
	scene->GetGameObject("TitlePlayer")->GetComponent<PlayerMove>()->SetPlayerY(66.0f);
	scene->GetGameObject("TitlePlayer")->GetComponent<PlayerMove>()->StopPlayer();
	scene->GetGameObject("TitlePlayer")->GetComponent<BoxCollider>()->FixedUpdate();

	// 타이틀 UI 위치 초기화
	scene->GetGameObject("Title_Image")->GetComponent<ImageRenderer>()->SetPosition(525.0f, 20.0f);
	scene->GetGameObject("button_Start")->GetComponent<ImageRenderer>()->SetPosition(0.0f, 400.0f);

	// 빛 세팅
	Light* titleLight = scene->GetGameObject("DirectionalLight")->GetComponent<Light>();
	titleLight->SetDirection(1.0f, -1.0f, 0.1f);
	titleLight->SetDiffuse(0.3f, 0.3f, 0.430f, 0.3f);
	titleLight->SetAmbient(0.0f, 0.06f, 0.410f, 0.2f);
	titleLight->SetSpecular(1.0f, 1.0f, 1.0f, 1.0f);

	// 세팅 후 scene 변경
	SceneManager::GetInstance().ChangeScene("Title");

	auto* pauseui = SceneManager::GetInstance().GetScene("Main")->GetGameObject("pauseuibox");
	auto* oPtion = SceneManager::GetInstance().GetScene("Main")->GetGameObject("Option");
	if (pauseui != nullptr)
	{
		ResetMenuUIPack(pauseui, "Main", "Title");
	}

	if (oPtion != nullptr)
	{
		ResetMenuUIPack(oPtion, "Main", "Title");
	}
	GameObject* soundManager = SceneManager::GetInstance().GetCurrentScene()->GetGameObject("TitleSceneSound");
	SoundPlayer* soundComp = soundManager->GetComponent<SoundPlayer>();
	int portal = soundComp->FindIndex("Resources/Sound/TitleMap.mp3");
	soundComp->Play(portal);

	// 전투중인 플레이어 초기화
	SceneManager::GetInstance().GetScene("Main")->GetGameObject("Player")->GetComponent<Player>()->ResetPlayerStatus();

	_bossComp->Reset();

	// 네비게이션 재설정
	Navigation::GetInstance().HandleBuild(0, "bridge_mapmesh.obj");

	// 타이틀 씬의 파티클 재활성화
	scene->GetGameObject("TitleRock1")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("TitleRock2Particle")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("TitleRock3")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("particlePortal1")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("particlePortal2")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("particlePortal3")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("particleMoon")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("Particle18")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("Particle18 (1)")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("Particle18 (2)")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("Particle18 (3)")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("Particle18 (4)")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("Particle18 (5)")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("Particle18 (6)")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("Particle18 (7)")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("Particle18 (8)")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("Particle18 (9)")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("kamenEye1")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("kamenEye2")->GetComponent<Particle>()->SetActive(true);

	// 큐브맵 설정
	GRAPHICS->SetMainCubeMap("TitleBackground");

	// 카메라 재설정 후 fadeout fadein
	SetCamera("TitleCamera");
	Startcoroutine(fadein);
}

void KunrealEngine::EventManager::ResetEndingSceneObjects()
{
	Scene* scene = SceneManager::GetInstance().GetScene("Ending");

	// 카메라 세팅
	DirectX::XMFLOAT3 cameraPos = { 0.0f, 0.0f, 1.0f };

	DirectX::XMFLOAT3 targetPos = { 0.0f, 0.0f, 0.0f };
	scene->GetGameObject("EndingCamera")->GetComponent<Camera>()->SetCameraPosition(cameraPos.x, cameraPos.y, cameraPos.z);
	scene->GetGameObject("EndingCamera")->GetComponent<Camera>()->SetTargetPosition(targetPos.x, targetPos.y, targetPos.z);

	scene->GetGameObject("EndingCamera")->GetComponent<Transform>()->SetPosition(-322.f, 100.0f, -148.130f);
	//scene->GetGameObject("EndingCamera")->GetComponent<Transform>()->SetRotation(-182.f, 168.f, 0.0f);

	// 빛 세팅
	DirectX::XMFLOAT4 diffuse = { 1.f, 1.f, 1.f, 0.3f };
	DirectX::XMFLOAT4 ambient = { 1.f, 1.f, 1.f, 0.2f };
	DirectX::XMFLOAT4 specular = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 direction = { 0.18f, -0.16f, -1.0f };

	scene->GetGameObject("DirectionalLight (1)")->GetComponent<Light>()->SetDirection(direction.x, direction.y, direction.z);

	// 파티클 재활성화
	scene->GetGameObject("EndingMeteo1")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("EndingMeteo2")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("EndingMeteo3")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("EndingMeteo4")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("EndingMeteo5")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("EndingMeteo6")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("EndingMeteo7")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("EndingMeteo8")->GetComponent<Particle>()->SetActive(true);

	// 오브젝트들 위치각도 초기화
	scene->GetGameObject("EndingRock")->GetComponent<Transform>()->SetPosition(-302.0f, 92.f, -190.0f);
	scene->GetGameObject("EndingRock")->GetComponent<MeshRenderer>()->SetIsDissolve(false);

	scene->GetGameObject("endingBoss")->GetComponent<Transform>()->SetPosition(-340.0f, 100.f, -267.0f);
	scene->GetGameObject("endingBoss")->GetComponent<Transform>()->SetRotation(0.f, 212.f, 0.f);
	scene->GetGameObject("endingBoss")->GetComponent<Animator>()->SetCurrentFrame(0);
	scene->GetGameObject("endingBoss")->GetComponent<MeshRenderer>()->SetIsDissolve(false);

	scene->GetGameObject("EndingPlayer")->GetComponent<Transform>()->SetPosition(-302.0f, 92.f, -190.0f);
	scene->GetGameObject("EndingPlayer")->GetComponent<Transform>()->SetRotation(3.0f, 9.f, 0.0f);

	scene->GetGameObject("EndingMeteo1")->GetComponent<Transform>()->SetPosition(153.7f, -60.f, -255.0f);
	scene->GetGameObject("EndingMeteo2")->GetComponent<Transform>()->SetPosition(-203.6f, -50.f, -255.0f);
	scene->GetGameObject("EndingMeteo3")->GetComponent<Transform>()->SetPosition(-300.0f, -70.f, -462.0f);
	scene->GetGameObject("EndingMeteo4")->GetComponent<Transform>()->SetPosition(-173.0f, -50.f, -315.0f);
	scene->GetGameObject("EndingMeteo5")->GetComponent<Transform>()->SetPosition(99.0f, -100.f, -450.0f);
	scene->GetGameObject("EndingMeteo6")->GetComponent<Transform>()->SetPosition(77.f, -200.f, -632.0f);
	scene->GetGameObject("EndingMeteo7")->GetComponent<Transform>()->SetPosition(-329.0f, -150.f, -255.0f);
	scene->GetGameObject("EndingMeteo8")->GetComponent<Transform>()->SetPosition(-261.0f, -200.f, -172.0f);

	scene->GetGameObject("EndingEnt")->GetComponent<Transform>()->SetPosition(-273.0f, -80.f, -475.0f);
	scene->GetGameObject("EndingEnt")->GetComponent<Transform>()->SetRotation(0.0f, 170.f, 56.0f);

	scene->GetGameObject("EndingSpider")->GetComponent<Transform>()->SetPosition(-242.0f, -90.f, -475.0f);
	scene->GetGameObject("EndingSpider")->GetComponent<Transform>()->SetRotation(-10.0f, -207.f, -381.0f);

	scene->GetGameObject("EndingSword")->GetComponent<Transform>()->SetPosition(-298.0f, -50.f, -397.0f);
	scene->GetGameObject("EndingSword")->GetComponent<Transform>()->SetRotation(-86.0f, 94.f, -188.0f);

	scene->GetGameObject("EndingKachujin")->GetComponent<Transform>()->SetPosition(-239.0f, -90.f, -475.0f);
	scene->GetGameObject("EndingKachujin")->GetComponent<Transform>()->SetRotation(10.0f, -149.f, -21.0f);

	scene->GetGameObject("EndingKunho")->GetComponent<Transform>()->SetPosition(-250.0f, -100.f, -475.0f);
	scene->GetGameObject("EndingKunho")->GetComponent<Transform>()->SetRotation(25.0f, 140.f, -41.0f);

	// Ending UI
	scene->GetGameObject("EndingCredit1")->GetComponent<ImageRenderer>()->SetPosition(0.0f, 1460.0f);
	scene->GetGameObject("EndingCredit2")->GetComponent<ImageRenderer>()->SetPosition(0.0f, 1460.0f);
	scene->GetGameObject("EndingCredit3")->GetComponent<ImageRenderer>()->SetPosition(0.0f, 1460.0f);
	scene->GetGameObject("EndingCredit4")->GetComponent<ImageRenderer>()->SetPosition(0.0f, 1460.0f);
	scene->GetGameObject("EndingCredit5")->GetComponent<ImageRenderer>()->SetPosition(0.0f, 1460.0f);
	scene->GetGameObject("EndingCredit6")->GetComponent<ImageRenderer>()->SetPosition(0.0f, 1460.0f);

	scene->GetGameObject("EndingThankYou")->GetComponent<ImageRenderer>()->SetPosition(0.0f, 1460.0f);
	scene->GetGameObject("EndingTheme")->GetComponent<ImageRenderer>()->SetPosition(0.0f, 1620.0f);
	scene->GetGameObject("BossTheme")->GetComponent<ImageRenderer>()->SetPosition(0.0f, 1460.0f);
}

void KunrealEngine::EventManager::MoveToEnding()
{
	// 카메라 고정 해제
	this->_iscamfollow = false;
	Scene* scene = SceneManager::GetInstance().GetScene("Ending");

	// 오브젝트들 위치 각도 초기화
	ResetEndingSceneObjects();

	// 엔진코어에 있는 엔딩 타이머 초기화
	this->_core->_endingTimer = 10.0f;

	// 큐브맵 세팅
	GRAPHICS->SetMainCubeMap("EndingBackground");

	// scene 변경 후 카메라 확정
	SceneManager::GetInstance().ChangeScene("Ending");
	SetCamera("EndingCamera");

	Startcoroutine(WhiteFadeIn);
}

void KunrealEngine::EventManager::ActivateEndingSceneTrigger()
{
	Startcoroutine(WhiteFadeOut);
}

void KunrealEngine::EventManager::ActivateFadeOutTrigger()
{
	Startcoroutine(fadeout);
}

void KunrealEngine::EventManager::ActivateEndingFadeTrigger()
{
	Startcoroutine(endingSceneFade);
}

void KunrealEngine::EventManager::GetEngineCore(EngineCore* core)
{
	this->_core = core;
}

void KunrealEngine::EventManager::SetDifficulty(eDifficulty dif)
{
	if (dif == eDifficulty::eEasy)
	{
		_playerComp->_playerInfo._hp = 300.0f;
		_playerComp->_playerInfo._maxhp = 300.0f;
		_playerComp->_playerInfo._speedScale = 1.8f;
		_playerComp->_playerInfo._moveSpeed = 17.0f;
		_playerComp->_playerInfo._dashCooldown = 8.0f;

		_player->GetComponent<PlayerAbility>()->_shotAbil->SetDamage(13.0f);
		_player->GetComponent<PlayerAbility>()->_circleAbil->SetCoolDown(12.0f);
		_player->GetComponent<PlayerAbility>()->_laserAbil->SetDamage(10.0f);
		_player->GetComponent<PlayerAbility>()->_meteorAbil->SetDamage(70.0f);
		_player->GetComponent<PlayerAbility>()->_meteorAbil->SetCoolDown(15.0f);
	}
	else if (dif == eDifficulty::eNormal)
	{
		_playerComp->_playerInfo._hp = 250.0f;
		_playerComp->_playerInfo._maxhp = 250.0f;
		_playerComp->_playerInfo._speedScale = 1.5f;
		_playerComp->_playerInfo._moveSpeed = 17.0f;
		_playerComp->_playerInfo._dashCooldown = 8.0f;

		_player->GetComponent<PlayerAbility>()->_shotAbil->SetDamage(8.0f);
		_player->GetComponent<PlayerAbility>()->_circleAbil->SetCoolDown(14.0f);
		_player->GetComponent<PlayerAbility>()->_laserAbil->SetDamage(7.0f);
		_player->GetComponent<PlayerAbility>()->_meteorAbil->SetDamage(50.0f);
		_player->GetComponent<PlayerAbility>()->_meteorAbil->SetCoolDown(20.0f);
	}
	else if (dif == eDifficulty::eHardCore)
	{
		_playerComp->_playerInfo._hp = 1.0f;
		_playerComp->_playerInfo._maxhp = 1.0f;
		_playerComp->_playerInfo._speedScale = 1.5f;
		_playerComp->_playerInfo._moveSpeed = 17.0f;
		_playerComp->_playerInfo._dashCooldown = 8.0f;

		_player->GetComponent<PlayerAbility>()->_shotAbil->SetDamage(6.0f);
		_player->GetComponent<PlayerAbility>()->_circleAbil->SetCoolDown(14.0f);
		_player->GetComponent<PlayerAbility>()->_laserAbil->SetDamage(7.0f);
		_player->GetComponent<PlayerAbility>()->_meteorAbil->SetDamage(50.0f);
		_player->GetComponent<PlayerAbility>()->_meteorAbil->SetCoolDown(20.0f);
	}
}
