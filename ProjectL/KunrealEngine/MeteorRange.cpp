#include "MeteorRange.h"
#include "Transform.h"
#include "TransparentMesh.h"
#include "GraphicsSystem.h"
#include "InputSystem.h"
#include "Navigation.h"

KunrealEngine::MeteorRange::MeteorRange()
	:_transform(nullptr), _tMesh(nullptr), _onCast(false)
{

}

void KunrealEngine::MeteorRange::Initialize()
{
	this->_transform = this->GetOwner()->GetComponent<Transform>();
	this->_transform->SetScale(80.0f, 200.0f, 80.0f);

	//this->_mesh = this->GetOwner()->AddComponent<MeshRenderer>();
	//this->_mesh->SetMeshObject("MagicCircle/MagicCircle");

	this->_tMesh = this->GetOwner()->AddComponent<TransparentMesh>();
	this->_tMesh->CreateTMesh("MeteorRange", "Resources/Textures/Decal/MeteorRangeLBlue", 0.6f);
	//this->_tMesh->CreateTMesh("MeteorRange", "Resources/Textures/Decal/Decal.png", 0.6f);
	this->_tMesh->SetDecal(true);
	this->_tMesh->SetInfinite(true);
	this->_tMesh->Reset();
	this->_tMesh->SetActive(true);
}

void KunrealEngine::MeteorRange::Release()
{
	
}

void KunrealEngine::MeteorRange::FixedUpdate()
{
	
}

void KunrealEngine::MeteorRange::Update()
{
	// 마우스 방향에 계속 생성되도록
	if (!_onCast)
	{
		DirectX::XMFLOAT3 pos = GRAPHICS->ScreenToWorldPoint(InputSystem::GetInstance()->GetEditorMousePos().x, InputSystem::GetInstance()->GetEditorMousePos().y);
		pos.y = 20.0f;

		this->_transform->SetPosition(pos);
	}
	else
	{
		this->_transform->SetRotation(this->_transform->GetRotation().x, this->_transform->GetRotation().y + 0.3f, this->_transform->GetRotation().z);
	}

	this->_tMesh->PlayOnce();
	SetTextureByStatus(CheckisOnMap());
}

void KunrealEngine::MeteorRange::LateUpdate()
{
	
}

void KunrealEngine::MeteorRange::OnTriggerEnter()
{
	
}

void KunrealEngine::MeteorRange::OnTriggerStay()
{
	
}

void KunrealEngine::MeteorRange::OnTriggerExit()
{
	
}

void KunrealEngine::MeteorRange::SetActive(bool active)
{
	if (active)
	{
		this->_tMesh->Reset();
		this->_tMesh->PlayInfinite();
		this->_tMesh->SetActive(true);
	}
	else
	{
		this->_tMesh->StopPlayingInfinite();
		this->_tMesh->SetActive(false);
	}

	this->_isActivated = active;

}

KunrealEngine::MeteorRange::~MeteorRange()
{

}

void KunrealEngine::MeteorRange::SetTextureByStatus(bool onMap)
{
	// 캐스팅 중이라면 붉은색 원
	if (_onCast)
	{
		this->_tMesh->SetTexture("Resources/Textures/Decal/MeteorRange.png");
		//this->_tMesh->SetTexture("Resources/Textures/Decal/Decal.png");
	}
	// 캐스팅 중이 아니고 시전 가능한 범위에 있을 때 하늘색 원
	else if (!_onCast && onMap)
	{
		this->_tMesh->SetTexture("Resources/Textures/Decal/MeteorRangeLBlue.png");
		//this->_tMesh->SetTexture("Resources/Textures/Decal/Decal.png");
	}
	// 캐스팅 중도 아니고 범위에도 없을 때 회색 원
	else
	{
		this->_tMesh->SetTexture("Resources/Textures/Decal/MeteorRangeGray.png");
		//this->_tMesh->SetTexture("Resources/Textures/Decal/Decal.png");
	}
}

bool KunrealEngine::MeteorRange::CheckisOnMap()
{
	DirectX::XMFLOAT3 mousePos = GRAPHICS->ScreenToWorldPoint(InputSystem::GetInstance()->GetEditorMousePos().x, InputSystem::GetInstance()->GetEditorMousePos().y);

	bool isOnmap = Navigation::GetInstance().GetPositionOnMap(0, mousePos.x, mousePos.y, mousePos.z);

	return isOnmap;
}
