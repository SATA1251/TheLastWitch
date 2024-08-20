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
	// ����ü ������Ʈ�� �����ɶ� �浹������ �׷��� 3D�� �ʿ�
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
		// ����ü�� �Ҽ� �� ������Ʈ���� �ϰ������� �����ص� �������
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
	// mesh�� �ݵ�� �ʿ�
	_mesh->SetMeshObject(meshName);

	// �ؽ�ó�� �������� �ʾ����� �ѱ�
	if (textureName != "")
	{
		_mesh->SetDiffuseTexture(0, textureName);
	}

	// �븻�� �������� �ʾ����� �ѱ�
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
