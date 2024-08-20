#include "Projectile.h"
#include "BoxCollider.h"
#include "MeshRenderer.h"
#include "Particle.h"
#include "Player.h"

KunrealEngine::Projectile::Projectile()
	:_collider(nullptr), _direction(), _mesh(nullptr), _movedRange(0.0f)
{
	_direction = DirectX::XMVectorZero();
}

KunrealEngine::Projectile::~Projectile()
{

}

void KunrealEngine::Projectile::Initialize()
{
	// 투사체 오브젝트가 생성될때 충돌감지와 그려줄 3D가 필요
	this->GetOwner()->AddComponent<BoxCollider>();
	this->GetOwner()->AddComponent<MeshRenderer>();

	_collider = this->GetOwner()->GetComponent<BoxCollider>();
	_mesh = this->GetOwner()->GetComponent<MeshRenderer>();
}

void KunrealEngine::Projectile::Release()
{
	
}

void KunrealEngine::Projectile::FixedUpdate()
{
	
}

void KunrealEngine::Projectile::Update()
{
	
}

void KunrealEngine::Projectile::LateUpdate()
{
	if (_condition())
	{
		this->GetOwner()->SetActive(false);
		this->GetOwner()->SetTotalComponentState(false);
	}
	else
	{
		// 투사체에 소속 된 컴포넌트들은 일괄적으로 관리해도 상관없다
		this->GetOwner()->SetActive(true);
		this->GetOwner()->SetTotalComponentState(true);
	}
}

void KunrealEngine::Projectile::OnTriggerEnter()
{

}
void KunrealEngine::Projectile::OnTriggerStay()
{
	
}
void KunrealEngine::Projectile::OnTriggerExit()
{
	
}

void KunrealEngine::Projectile::SetActive(bool active)
{
	//this->_isActivated = active;
}

void KunrealEngine::Projectile::SetDirection(DirectX::XMVECTOR direction)
{
	_direction = direction;
}

void KunrealEngine::Projectile::SetMeshObject(const char* meshName, const char* textureName /*= ""*/, const char* normalName /*= ""*/)
{
	// mesh는 반드시 필요
	_mesh->SetMeshObject(meshName);

	// 텍스처를 정해주지 않았으면 넘김
	if (textureName != "")
	{
		_mesh->SetDiffuseTexture(0, textureName);
	}

	// 노말을 정해주지 않았으면 넘김
	if (normalName != "")
	{
		_mesh->SetNormalTexture(0, normalName);
	}
}

KunrealEngine::BoxCollider* KunrealEngine::Projectile::GetCollider()
{
	return this->_collider;
}

DirectX::XMVECTOR KunrealEngine::Projectile::GetDirection()
{
	return this->_direction;
}

void KunrealEngine::Projectile::SetDestoryCondition(std::function<bool()> cond)
{
	this->_condition = cond;
}

void KunrealEngine::Projectile::ResetCondition()
{
	
}
