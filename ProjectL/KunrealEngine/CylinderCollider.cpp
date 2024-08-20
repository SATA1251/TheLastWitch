#include "CylinderCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "PhysicsSystem.h"

KunrealEngine::CylinderCollider::CylinderCollider()
	:_debugObject(nullptr)
{

}

void KunrealEngine::CylinderCollider::Initialize()
{
	this->_ownerObj = this->GetOwner();
	this->_ownerObj->SetCollider(this);
	this->_transform = this->_ownerObj->GetComponent<Transform>();
	this->_position = this->_transform->GetPosition();

	PhysicsSystem::GetInstance().CreateCylinderCollider(this);

	_debugObject = GRAPHICS->CreateDebugCube(this->GetOwner()->GetObjectName().c_str(), this->_scale.x, this->_scale.y, this->_scale.z);
	SetColliderScale(this->_transform->GetScale());
}

void KunrealEngine::CylinderCollider::Release()
{
	_debugObject->Delete();
	PhysicsSystem::GetInstance().ReleasePhysxObject(this);
}

void KunrealEngine::CylinderCollider::FixedUpdate()
{
	// 부모 오브젝트가 존재하는 경우
	if (this->GetOwner()->GetParent() != nullptr)
	{
		auto transform = this->_transform->GetWorldTM();
		DirectX::XMMATRIX worldTM = DirectX::XMLoadFloat4x4(&transform);

		DirectX::XMVECTOR scale;
		DirectX::XMVECTOR quat;
		DirectX::XMVECTOR translation;

		DirectX::XMMatrixDecompose(&scale, &quat, &translation, worldTM);

		DirectX::XMStoreFloat3(&this->_position, translation);
		this->_position.x += _offset.x;
		this->_position.y += _offset.y;
		this->_position.z += _offset.z;

		DirectX::XMStoreFloat4(&this->_quaternion, quat);
	}
	else
	{
		this->_position.x = this->GetOwner()->GetComponent<Transform>()->GetPosition().x + this->_offset.x;
		this->_position.y = this->GetOwner()->GetComponent<Transform>()->GetPosition().y + this->_offset.y;
		this->_position.z = this->GetOwner()->GetComponent<Transform>()->GetPosition().z + this->_offset.z;

		this->_quaternion = this->GetOwner()->GetComponent<Transform>()->GetQuaternion();
	}
}

void KunrealEngine::CylinderCollider::Update()
{
	SetDebugMeshData();
}

void KunrealEngine::CylinderCollider::LateUpdate()
{
	
}

void KunrealEngine::CylinderCollider::OnTriggerEnter()
{
	
}

void KunrealEngine::CylinderCollider::OnTriggerStay()
{
	
}

void KunrealEngine::CylinderCollider::OnTriggerExit()
{
	
}

void KunrealEngine::CylinderCollider::SetActive(bool active)
{
	PhysicsSystem::GetInstance().SetActorState(this, active);
	this->_debugObject->SetActive(active);

	this->_isActivated = active;
	this->_colliderActivated = active;

	if (!this->_isActivated)
	{
		if (this->_targetObj != nullptr)
		{
			// collider 비활성화 후 대상 object에 자신에 관한 정보가 남아있다면 비워준다
			if (this->_targetObj->GetCollider()->GetTargetObject() == this->_ownerObj)
			{
				this->_targetObj->GetCollider()->Clear();
			}

			Clear();
		}
		else
		{
			Clear();
		}
	}
}

KunrealEngine::CylinderCollider::~CylinderCollider()
{

}

void KunrealEngine::CylinderCollider::SetColliderScale(float x, float y, float z)
{
	this->_scale.x = x;
	this->_scale.y = y;
	this->_scale.z = z;

	this->_debugObject->SetScale(x, y, z);
	PhysicsSystem::GetInstance().SetCylinderSize(this);
}

void KunrealEngine::CylinderCollider::SetColliderScale(const DirectX::XMFLOAT3& scale)
{
	this->_scale = scale;

	this->_debugObject->SetScale(scale.x, scale.y, scale.z);
	PhysicsSystem::GetInstance().SetCylinderSize(this);
}

void KunrealEngine::CylinderCollider::SetDebugMeshData()
{
	if (!_transform->_haveParentBone)
	{
		_debugObject->SetPosition(_position.x, _position.y, _position.z);
		_debugObject->SetScale(this->_scale.x, this->_scale.y, this->_scale.z);
		_debugObject->SetRotation(_transform->GetRotation().x, _transform->GetRotation().y, _transform->GetRotation().z);
	}
	else
	{
		DirectX::XMFLOAT4X4 worldToDebug;

		DirectX::XMStoreFloat4x4(&worldToDebug, DirectX::XMMatrixScaling(this->_scale.x, this->_scale.y, this->_scale.z)
			* DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&this->_quaternion))
			* DirectX::XMMatrixTranslation(_position.x, _position.y, _position.z));

		_debugObject->SetTransform(worldToDebug);
	}
}
