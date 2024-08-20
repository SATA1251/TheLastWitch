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
				// ķ �ȷθ� ����.
				_iscamfollow = false;
				// �ð��� 0.1�� ������Ų��
				// �÷��̾��� Ÿ�� �������� �޾ƿ´�.
				DirectX::XMFLOAT3 targetpos = _player->GetComponent<PlayerMove>()->GetTargetPosition();
				///GetCurrentScene()->CreateObject("targetpos")->GetComponent<Transform>()->SetPosition(targetpos);
				// �÷��̾��� ��ġ�� �޾ƿ´�.
				DirectX::XMFLOAT3 playerpos = _player->GetComponent<Transform>()->GetPosition();
				///GetCurrentScene()->CreateObject("playerpos")->GetComponent<Transform>()->SetPosition(playerpos);
				Waitforsecond(0.1f);

				// ī�޶��� ��ġ�� �޾ƿ´�
				DirectX::XMFLOAT3 campos = _mainCamera->GetComponent<Transform>()->GetPosition();
				DirectX::XMFLOAT3 fixedcampos = campos; // ���������� ���� ���纻�̴�.
				// �÷��̾��� ��ġ�� Ÿ�� �������� ��ġ�� ���� �̵��� ���͸� ���Ѵ�.
				DirectX::XMFLOAT3 movepos = ToolBox::XMFLOAT3Subtract(targetpos, playerpos);
				// �÷��̾��� ��ġ�� ī�޶��� ��ġ�� ���� ī�޶���� �Ÿ����̸� ���Ѵ�.
				DirectX::XMFLOAT3 movevec = ToolBox::XMFLOAT3Subtract(playerpos, campos);
				// �̵��� ���͸� ī�޶��� ��ġ�� ���� Ÿ���������� ���Ѵ�.
				DirectX::XMFLOAT3 camTarget = ToolBox::XMFLOAT3Add(campos, movepos);
				camTarget.y = campos.y;

				// ������ ī�޶� �� ������ �����Ѵ�. �׷����� �ϴ� ���⺤�͸� �����Ѵ�.
				DirectX::XMFLOAT3 basicvec(0, 0, 0);	// ���⺤�͸� ���ϱ� ���� ����
				DirectX::XMFLOAT3 dirvec;				// ���⺤�͸� ���� XMFLOAT3
				// ���⺤�͸� ���ؼ� ��´�.
				DirectX::XMStoreFloat3(&dirvec, ToolBox::GetDirectionVec(basicvec, movepos));
				// ������ �󸶳� �� ���ư� ������?
				float range = 3.0f;
				// �󸶳� �� ���ư����� ����Ѵ�.
				dirvec = DirectX::XMFLOAT3(dirvec.x * range, dirvec.y * range, dirvec.z * range);
				// �׸��� �װ� camtarget�� ���ؼ� ��ǥ������ ����� �ش�.
				DirectX::XMFLOAT3 camoverTarget = DirectX::XMFLOAT3 \
					(dirvec.x + camTarget.x, dirvec.y + camTarget.y, dirvec.z + camTarget.z);

				// �Ÿ����� ����
				float distancevalue = 0.1f;

				// ���������� �����ϰ� �����ϴ� t��
				float t = 0.0f;
				//while���� ������ ������ ī�޶�� Ÿ�� ��ġ������ �Ÿ�
				while (ToolBox::GetDistance(camoverTarget, campos) > distancevalue)
				{
					if (t > 1.0f)
					{
						break;
					}
					// t���� �����ش�.
					t += 0.1;
					// for�� ���� ������ ���� campos�� �޾ƿ´�.
					campos = _mainCamera->GetComponent<Transform>()->GetPosition();
					// ī�޶���� Ÿ�� �����Ǳ����� �Ÿ��� 0~1�� �ΰ� ���������� ���� �������Ӹ����� ��ġ�� ���Ѵ�.
					DirectX::XMFLOAT3 result = ToolBox::Lerp(fixedcampos, camoverTarget, t);
					// ���� ī�޶� �ű�� �ű���
					_mainCamera->GetComponent<Transform>()->SetPosition(result);

					Return_null; // �̹� ������ ����
				}

				// �̹��� ����������!
				t = 0.0f;
				while (t > 1.0f)
				{
					// t���� �����ش�.
					t += 0.1;
					// for�� ���� ������ ���� campos�� �޾ƿ´�.
					campos = _mainCamera->GetComponent<Transform>()->GetPosition();
					// ī�޶���� Ÿ�� �����Ǳ����� �Ÿ��� 0~1�� �ΰ� ���������� ���� �������Ӹ����� ��ġ�� ���Ѵ�.
					// �̰� ������ camTarget�� �������
					DirectX::XMFLOAT3 result = ToolBox::Lerp(camoverTarget, camTarget, t);
					// ���� ī�޶� �ű�� �ű���
					_mainCamera->GetComponent<Transform>()->SetPosition(result);

					Return_null; // �̹� ������ ����
				}
				// �� �������� ī�޶������� �ٽ� �Ҵ�.
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
	// ���� ī�޶� ȸ��
	// ���� �׵θ����� ������ numpoint ��ŭ �߷��� ��, �� ������ ��ȯ.
	// ������ ��ġ�� ���������� �����Ѵٸ� ī�޶�� ���� �׸��� ��鸰��.(��ǥ)
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
	// �յ�� ī�޶� ����
	// sigma�� ���� ��ŭ ī�޶� ����. �� sigma ���� ���� �۾�����.
	// �̶��� �¿�� ���� �ϴ� Ƚ���� numpoint ���̴�.
	std::vector<float> result;
	float cur_sigma = sigma;

	for (float a = 0; a < numPoints; a++)
	{
		// ���� ���� ������ ����Ʈ�� �߰� (���)
		result.push_back(static_cast<int>(cur_sigma));
		// ���� ���� ������ ����Ʈ�� �߰� (����)
		result.push_back(static_cast<int>(-cur_sigma));
		// ���� ���� ����
		cur_sigma -= sigma / numPoints;
	}
	result.push_back(static_cast<int>(0));

	return result;
}

void KunrealEngine::EventManager::CamShake(float harder)
{
	// ������ ī�޶� ����. 
	// �׳� ������ ���� �� �������� �����ڴ´�.
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

	/// �Ͼ� fade�� �Ұ� alpha ���� ���� ����
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

	// ������ �������϶���?
	if (nowPattern != nullptr)
	{
		// �ݶ��̴��� ���� ���
		if (nowPattern->_colliderType == BossPattern::eColliderType::eNone)
		{
			return;
		}

		// �ڽ� �ݶ��̴��� �Ǹ��� �ݶ��̴��� ���
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

				// �ݶ��̴��� �������� ���� ���� ����
				if (nowPattern->_isColliderActive[i] && !nowPattern->_isColliderHit[i])
				{
					collider->FixedUpdate();

					collider->SetActive(true);

					if (collider->IsCollided() && collider->GetTargetObject() == _player)
					{
						// ������ ���� ������Ʈ ������ �𷺼����� �Ѿ�߸�
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

						// �÷��̾��� hp���� ������ ��������ŭ ������Ŵ
						_playerComp->GetPlayerData()._hp -= damage;
						_playerComp->SetHitState(static_cast<int> (nowPattern->_attackState));

						// �������� �� �� �޽��� �����Ѵٸ�
						if (!nowPattern->_isRemainMesh && subObjectList[i]->GetComponent<MeshRenderer>() != nullptr)
						{
							// �޽��� ������
							subObjectList[i]->GetComponent<MeshRenderer>()->SetActive(false);
						}

						// �¾Ҵٰ� üũ
						nowPattern->_isColliderHit[i] = true;
					}
				}

				// �ݶ��̴��� ������ ���� ����
				if (nowPattern->_isColliderActive[i] && nowPattern->_isColliderHit[i])
				{
					// �¾����� �ݶ��̴� ����
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

				// �ݶ��̴��� �������� ���� ���� ����
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

						// �÷��̾��� hp���� ������ ��������ŭ ������Ŵ
						_playerComp->GetPlayerData()._hp -= damage;
						_playerComp->SetHitState(static_cast<int> (nowPattern->_attackState));

						// �������� �� �� �޽��� �����Ѵٸ�
						if (!nowPattern->_isRemainMesh && subObjectList[i]->GetComponent<MeshRenderer>() != nullptr)
						{
							// �޽��� ������
							subObjectList[i]->GetComponent<MeshRenderer>()->SetActive(false);
						}

						// �¾Ҵٰ� üũ
						nowPattern->_isColliderHit[i] = true;
					}
				}

				// �ݶ��̴��� ������ ���� ����
				if (nowPattern->_isColliderActive[i] && nowPattern->_isColliderHit[i])
				{
					// �¾����� �ݶ��̴� ����
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

				// �ݶ��̴��� �������� ���� ���� ����
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

						// �÷��̾��� hp���� ������ ��������ŭ ������Ŵ
						_playerComp->GetPlayerData()._hp -= damage;
						_playerComp->SetHitState(static_cast<int> (nowPattern->_attackState));


						// �¾Ҵٰ� üũ
						nowPattern->_isColliderHit[i] = true;
					}
				}

				// �ݶ��̴��� ������ ���� ����
				if (nowPattern->_isColliderActive[i] && nowPattern->_isColliderHit[i])
				{
					// �¾����� �ݶ��̴� ����
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

				// �ݶ��̴��� �������� ���� ���� ����
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

						// �÷��̾��� hp���� ������ ��������ŭ ������Ŵ
						_playerComp->GetPlayerData()._hp -= damage;
						_playerComp->SetHitState(static_cast<int> (nowPattern->_attackState));


						// �¾Ҵٰ� üũ
						nowPattern->_isColliderHit[i] = true;
					}
				}

				// �ݶ��̴��� ������ ���� ����
				if (nowPattern->_isColliderActive[i] && nowPattern->_isColliderHit[i])
				{
					// �¾����� �ݶ��̴� ����
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

					// �ݶ��̴��� �������� ���� ���� ����
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

							// �÷��̾��� hp���� ������ ��������ŭ ������Ŵ
							_playerComp->GetPlayerData()._hp -= damage;
							_playerComp->SetHitState(static_cast<int> (nowPattern->_attackState));


							// �¾Ҵٰ� üũ
							nowPattern->_isColliderHit[i] = true;
						}
					}

					// �ݶ��̴��� ������ ���� ����
					if (nowPattern->_isColliderActive[i] && nowPattern->_isColliderHit[i])
					{
						// �¾����� �ݶ��̴� ����
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

					// �ݶ��̴��� �������� ���� ���� ����
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

							// �÷��̾��� hp���� ������ ��������ŭ ������Ŵ
							_playerComp->GetPlayerData()._hp -= damage;
							_playerComp->SetHitState(static_cast<int> (nowPattern->_attackState));


							// �¾Ҵٰ� üũ
							nowPattern->_isColliderHit[i] = true;
						}
					}

					// �ݶ��̴��� ������ ���� ����
					if (nowPattern->_isColliderActive[i] && nowPattern->_isColliderHit[i])
					{
						// �¾����� �ݶ��̴� ����
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

					// �ݶ��̴��� �������� ���� ���� ����
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

							// �÷��̾��� hp���� ������ ��������ŭ ������Ŵ
							_playerComp->GetPlayerData()._hp -= damage;
							_playerComp->SetHitState(static_cast<int> (nowPattern->_attackState));


							// �¾Ҵٰ� üũ
							nowPattern->_isColliderHit[i] = true;
						}
					}

					// �ݶ��̴��� ������ ���� ����
					if (nowPattern->_isColliderActive[i] && nowPattern->_isColliderHit[i])
					{
						// �¾����� �ݶ��̴� ����
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

					// �ݶ��̴��� �������� ���� ���� ����
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

							// �÷��̾��� hp���� ������ ��������ŭ ������Ŵ
							_playerComp->GetPlayerData()._hp -= damage;
							_playerComp->SetHitState(static_cast<int> (nowPattern->_attackState));

							// �������� �� �� �޽��� �����Ѵٸ�
							if (!nowPattern->_isRemainMesh && subObjectList[i]->GetComponent<MeshRenderer>() != nullptr)
							{
								// �޽��� ������
								subObjectList[i]->GetComponent<MeshRenderer>()->SetActive(false);
							}

							//�¾Ҵٰ� üũ
							nowPattern->_isColliderHit[i] = true;
						}
					}

					// �ݶ��̴��� ������ ���� ����
					if (nowPattern->_isColliderActive[i] && nowPattern->_isColliderHit[i])
					{
						// �¾����� �ݶ��̴� ����
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
		// �ݶ��̴��� ���� ���
		if (nowPattern->_colliderType == BossPattern::eColliderType::eNone)
		{
			return;
		}

		// �ڽ� �ݶ��̴��� ���
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

				// �ݶ��̴��� �������� ���� ���� ����
				if (nowPattern->_isColliderActive[i] && !nowPattern->_isColliderHit[i])
				{
					collider->FixedUpdate();

					collider->SetActive(true);

					if (collider->IsCollided() && collider->GetTargetObject() == _player)
					{
						// �ҵ��� ���� ���ͷ� �Ѿ�߸�
						_playerComp->CalculateSweep(_bossComp->GetSwordDirection());

						auto damage = nowPattern->_damage;

						// �÷��̾��� hp���� ������ ��������ŭ ������Ŵ
						_playerComp->GetPlayerData()._hp -= damage;
						_playerComp->SetHitState(static_cast<int> (nowPattern->_attackState));

						// �������� �� �� �޽��� �����Ѵٸ�
						if (!nowPattern->_isRemainMesh && subObjectList[i]->GetComponent<MeshRenderer>() != nullptr)
						{
							// �޽��� ������
							subObjectList[i]->GetComponent<MeshRenderer>()->SetActive(false);
						}

						// �¾Ҵٰ� üũ
						nowPattern->_isColliderHit[i] = true;
					}
				}

				// �ݶ��̴��� ������ ���� ����
				if (nowPattern->_isColliderActive[i] && nowPattern->_isColliderHit[i])
				{
					// �¾����� �ݶ��̴� ����
					nowPattern->_isColliderActive[i] = false;

					collider->SetActive(false);
				}
			}
		}

		// ���� ���� ����
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

				// �ݶ��̴��� �������� ���� ���� ����
				if (nowPattern->_isColliderActive[i] && !nowPattern->_isColliderHit[i])
				{
					collider->FixedUpdate();

					collider->SetActive(true);

					if (collider->IsCollided() && collider->GetTargetObject() == _player)
					{
						auto colliderDirVec = SetWarningAttackDirection(subObjectList[i]);
						_playerComp->CalculateSweep(colliderDirVec);

						auto damage = nowPattern->_damage;

						// �÷��̾��� hp���� ������ ��������ŭ ������Ŵ
						_playerComp->GetPlayerData()._hp -= damage;
						_playerComp->SetHitState(static_cast<int> (nowPattern->_attackState));

						// �������� �� �� �޽��� �����Ѵٸ�
						if (!nowPattern->_isRemainMesh && subObjectList[i]->GetComponent<MeshRenderer>() != nullptr)
						{
							// �޽��� ������
							subObjectList[i]->GetComponent<MeshRenderer>()->SetActive(false);
						}

						// �¾Ҵٰ� üũ
						nowPattern->_isColliderHit[i] = true;
					}
				}

				// �ݶ��̴��� ������ ���� ����
				if (nowPattern->_isColliderActive[i] && nowPattern->_isColliderHit[i])
				{
					// �¾����� �ݶ��̴� ����
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

				// �ݶ��̴��� �������� ���� ���� ����
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
								// �ҵ�� �������Ʈ�� �ݶ��̴�
								auto colliderDirVec = SetWarningAttackDirection(subObjectList[i]);

								_playerComp->CalculateSweep(colliderDirVec);

								auto damage = nowPattern->_damage;

								// �÷��̾��� hp���� ������ ��������ŭ ������Ŵ
								_playerComp->GetPlayerData()._hp -= damage;
								_playerComp->SetHitState(static_cast<int> (nowPattern->_attackState));

								nowPattern->_isColliderHit[i] = true;
							}
						}
					}
				}

				// �ݶ��̴��� ������ ���� ����
				if (nowPattern->_isColliderActive[i] && nowPattern->_isColliderHit[i])
				{
					// �¾����� �ݶ��̴� ����
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

				// �ݶ��̴��� �������� ���� ���� ����
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

						// �÷��̾��� hp���� ������ ��������ŭ ������Ŵ
						_playerComp->GetPlayerData()._hp -= damage;
						_playerComp->SetHitState(static_cast<int> (nowPattern->_attackState));
						nowPattern->_isColliderHit[i] = true;

						// �������� �� �� �޽��� �����Ѵٸ�
						if (!nowPattern->_isRemainMesh && subObjectList[i]->GetComponent<MeshRenderer>() != nullptr)
						{
							// �޽��� ������
							subObjectList[i]->GetComponent<MeshRenderer>()->SetActive(false);
						}

						// �¾Ҵٰ� üũ
						//nowPattern->_isColliderHit[i] = true;
					}
				}

				// �ݶ��̴��� ������ ���� ����
				if (nowPattern->_isColliderActive[i] && nowPattern->_isColliderHit[i])
				{
					// �¾����� �ݶ��̴� ����
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

	// ī�޶� �缳�� ��
	Camera* titleCamera = scene->GetGameObject("TitleCamera")->GetComponent<Camera>();
	scene->GetGameObject("TitleCamera")->GetComponent<Transform>()->SetPosition(-155.0f, 135.0f, -45.130f);

	// ī�޶� ���� ����
	this->_iscamfollow = false;

	// �÷��̾� ��ġ �ʱ�ȭ
	scene->GetGameObject("TitlePlayer")->GetComponent<Transform>()->SetPosition(-156.0f, 66.0f, 0.0f);
	scene->GetGameObject("TitlePlayer")->GetComponent<Transform>()->SetRotation(0.0f, 45.0f, 0.0f);
	scene->GetGameObject("TitlePlayer")->GetComponent<PlayerMove>()->SetPlayerY(66.0f);
	scene->GetGameObject("TitlePlayer")->GetComponent<PlayerMove>()->StopPlayer();
	scene->GetGameObject("TitlePlayer")->GetComponent<BoxCollider>()->FixedUpdate();

	// Ÿ��Ʋ UI ��ġ �ʱ�ȭ
	scene->GetGameObject("Title_Image")->GetComponent<ImageRenderer>()->SetPosition(525.0f, 20.0f);
	scene->GetGameObject("button_Start")->GetComponent<ImageRenderer>()->SetPosition(0.0f, 400.0f);

	// �� ����
	Light* titleLight = scene->GetGameObject("DirectionalLight")->GetComponent<Light>();
	titleLight->SetDirection(1.0f, -1.0f, 0.1f);
	titleLight->SetDiffuse(0.3f, 0.3f, 0.430f, 0.3f);
	titleLight->SetAmbient(0.0f, 0.06f, 0.410f, 0.2f);
	titleLight->SetSpecular(1.0f, 1.0f, 1.0f, 1.0f);

	// ���� �� scene ����
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

	// �������� �÷��̾� �ʱ�ȭ
	SceneManager::GetInstance().GetScene("Main")->GetGameObject("Player")->GetComponent<Player>()->ResetPlayerStatus();

	_bossComp->Reset();

	// �׺���̼� �缳��
	Navigation::GetInstance().HandleBuild(0, "bridge_mapmesh.obj");

	// Ÿ��Ʋ ���� ��ƼŬ ��Ȱ��ȭ
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

	// ť��� ����
	GRAPHICS->SetMainCubeMap("TitleBackground");

	// ī�޶� �缳�� �� fadeout fadein
	SetCamera("TitleCamera");
	Startcoroutine(fadein);
}

void KunrealEngine::EventManager::ResetEndingSceneObjects()
{
	Scene* scene = SceneManager::GetInstance().GetScene("Ending");

	// ī�޶� ����
	DirectX::XMFLOAT3 cameraPos = { 0.0f, 0.0f, 1.0f };

	DirectX::XMFLOAT3 targetPos = { 0.0f, 0.0f, 0.0f };
	scene->GetGameObject("EndingCamera")->GetComponent<Camera>()->SetCameraPosition(cameraPos.x, cameraPos.y, cameraPos.z);
	scene->GetGameObject("EndingCamera")->GetComponent<Camera>()->SetTargetPosition(targetPos.x, targetPos.y, targetPos.z);

	scene->GetGameObject("EndingCamera")->GetComponent<Transform>()->SetPosition(-322.f, 100.0f, -148.130f);
	//scene->GetGameObject("EndingCamera")->GetComponent<Transform>()->SetRotation(-182.f, 168.f, 0.0f);

	// �� ����
	DirectX::XMFLOAT4 diffuse = { 1.f, 1.f, 1.f, 0.3f };
	DirectX::XMFLOAT4 ambient = { 1.f, 1.f, 1.f, 0.2f };
	DirectX::XMFLOAT4 specular = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 direction = { 0.18f, -0.16f, -1.0f };

	scene->GetGameObject("DirectionalLight (1)")->GetComponent<Light>()->SetDirection(direction.x, direction.y, direction.z);

	// ��ƼŬ ��Ȱ��ȭ
	scene->GetGameObject("EndingMeteo1")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("EndingMeteo2")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("EndingMeteo3")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("EndingMeteo4")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("EndingMeteo5")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("EndingMeteo6")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("EndingMeteo7")->GetComponent<Particle>()->SetActive(true);
	scene->GetGameObject("EndingMeteo8")->GetComponent<Particle>()->SetActive(true);

	// ������Ʈ�� ��ġ���� �ʱ�ȭ
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
	// ī�޶� ���� ����
	this->_iscamfollow = false;
	Scene* scene = SceneManager::GetInstance().GetScene("Ending");

	// ������Ʈ�� ��ġ ���� �ʱ�ȭ
	ResetEndingSceneObjects();

	// �����ھ �ִ� ���� Ÿ�̸� �ʱ�ȭ
	this->_core->_endingTimer = 10.0f;

	// ť��� ����
	GRAPHICS->SetMainCubeMap("EndingBackground");

	// scene ���� �� ī�޶� Ȯ��
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
