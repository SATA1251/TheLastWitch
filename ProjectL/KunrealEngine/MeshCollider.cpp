#include "MeshCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "PhysicsSystem.h"

KunrealEngine::MeshCollider::MeshCollider()
	:_debugObject(nullptr), _meshName("")
{

}

KunrealEngine::MeshCollider::~MeshCollider()
{

}

void KunrealEngine::MeshCollider::Initialize()
{
	this->_ownerObj = this->GetOwner();
	this->_ownerObj->SetCollider(this);
	this->_transform = this->_ownerObj->GetComponent<Transform>();
	this->_position = this->_transform->GetPosition();

	_debugObject = GRAPHICS->CreateDebugCube(this->GetOwner()->GetObjectName().c_str(), this->_scale.x, this->_scale.y, this->_scale.z);

}

void KunrealEngine::MeshCollider::Release()
{
	_debugObject->Delete();
	PhysicsSystem::GetInstance().ReleasePhysxObject(this);
}

void KunrealEngine::MeshCollider::FixedUpdate()
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

void KunrealEngine::MeshCollider::Update()
{
	SetDebugMeshData();
}

void KunrealEngine::MeshCollider::LateUpdate()
{

}

void KunrealEngine::MeshCollider::OnTriggerEnter()
{

}

void KunrealEngine::MeshCollider::OnTriggerStay()
{

}

void KunrealEngine::MeshCollider::OnTriggerExit()
{

}

void KunrealEngine::MeshCollider::SetActive(bool active)
{
	PhysicsSystem::GetInstance().SetActorState(this, active);
	this->_debugObject->SetActive(active);

	this->_isActivated = active;

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

void KunrealEngine::MeshCollider::CreateMeshCollider(std::string meshName)
{
	PhysicsSystem::GetInstance().CreateMeshCollider(this, meshName);
	this->_meshName = meshName;
}

void KunrealEngine::MeshCollider::SetColliderScale(float x, float y, float z)
{
	this->_scale.x = x;
	this->_scale.y = y;
	this->_scale.z = z;

	this->_debugObject->SetScale(x, y, z);
	PhysicsSystem::GetInstance().SetMeshSize(this, this->_meshName);
}

void KunrealEngine::MeshCollider::SetColliderScale(const DirectX::XMFLOAT3& scale)
{
	this->_scale = scale;

	this->_debugObject->SetScale(scale.x, scale.y, scale.z);
	PhysicsSystem::GetInstance().SetMeshSize(this, this->_meshName);
}

void KunrealEngine::MeshCollider::SetDebugMeshData()
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