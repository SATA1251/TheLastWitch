#include "BoxCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "PhysicsSystem.h"

KunrealEngine::BoxCollider::BoxCollider()
	: _debugObject(nullptr), _parentObject(nullptr), _parentBoneName()
{

}

KunrealEngine::BoxCollider::~BoxCollider()
{

}

void KunrealEngine::BoxCollider::Initialize()
{
	this->_ownerObj = this->GetOwner();
	this->_ownerObj->SetCollider(this);
	this->_transform = this->_ownerObj->GetComponent<Transform>();
	this->_position = this->_transform->GetPosition();
	
	PhysicsSystem::GetInstance().CreateDynamicBoxCollider(this);

	_debugObject = GRAPHICS->CreateDebugCube(this->GetOwner()->GetObjectName().c_str(), this->_scale.x, this->_scale.y, this->_scale.z);
}

void KunrealEngine::BoxCollider::Release()
{
	_debugObject->Delete();
	PhysicsSystem::GetInstance().ReleasePhysxObject(this);
}

void KunrealEngine::BoxCollider::FixedUpdate()
{
	// 메쉬가 존재하지 않고 콜라이더만 존재하는 경우
	if (this->_parentObject != nullptr)
	{
		CalculateParentBone();
	}

	// 특정 본을 부모로 하지 않을 경우
	if (!_transform->_haveParentBone)
	{
		// 부모 오브젝트는 존재하는 경우
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

	// 특정 본을 부모로 받는 경우
	else
	{
		this->_position = this->GetOwner()->GetComponent<Transform>()->_posForBone;
		this->_position.x += _offset.x;
		this->_position.y += _offset.y;
		this->_position.z += _offset.z;

		this->_quaternion = this->GetOwner()->GetComponent<Transform>()->_quatForBone;
	}
}

void KunrealEngine::BoxCollider::Update()
{
	SetDebugMeshData();
}

void KunrealEngine::BoxCollider::LateUpdate()
{
	
}

void KunrealEngine::BoxCollider::OnTriggerEnter()
{

}

void KunrealEngine::BoxCollider::OnTriggerStay()
{

}

void KunrealEngine::BoxCollider::OnTriggerExit()
{

}

void KunrealEngine::BoxCollider::SetActive(bool active)
{
	PhysicsSystem::GetInstance().SetActorState(this, active);
	this->_debugObject->SetActive(active);

	this->_isActivated = active;
	this->_colliderActivated = active;

	if (!this->_isActivated)
	{
		if (_targetObj != nullptr)
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

void KunrealEngine::BoxCollider::SetDebugMeshData()
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


void KunrealEngine::BoxCollider::SetColliderScale(float x, float y, float z)
{
	this->_scale.x = x;
	this->_scale.y = y;
	this->_scale.z = z;

	this->_debugObject->SetScale(x, y, z);
	PhysicsSystem::GetInstance().SetBoxSize(this);		// 다른 collider에선 다른 함수 호출
}


void KunrealEngine::BoxCollider::SetColliderScale(const DirectX::XMFLOAT3& scale)
{
	this->_scale = scale;

	this->_debugObject->SetScale(scale.x, scale.y, scale.z);
	PhysicsSystem::GetInstance().SetBoxSize(this);		// 다른 collider에선 다른 함수 호출
}

void KunrealEngine::BoxCollider::SetTransform(GameObject* renderable, std::string boneName)
{
	_parentObject = renderable;
	_parentBoneName = boneName;

	_transform->_haveParentBone = true;

	if (renderable != this->GetOwner())
	{
		this->GetOwner()->SetParent(renderable);
	}
}

void KunrealEngine::BoxCollider::SeperateFromParent()
{
	if (_parentObject != nullptr)
	{
		_parentObject = nullptr;
		_parentBoneName = "";

		_transform->_haveParentBone = false;
	}
}

void KunrealEngine::BoxCollider::CalculateParentBone()
{
	auto mat = GRAPHICS->GetTransform(_parentObject->GetComponent<MeshRenderer>()->GetMeshObject(), _parentBoneName);

	if (_transform->_haveParentBone)
	{
		DirectX::XMMATRIX worldTM = DirectX::XMLoadFloat4x4(&mat);

		DirectX::XMVECTOR scale;
		DirectX::XMVECTOR quat;
		DirectX::XMVECTOR translation;

		DirectX::XMMatrixDecompose(&scale, &quat, &translation, worldTM);

		DirectX::XMStoreFloat3(&this->_transform->_posForBone, translation);
		DirectX::XMStoreFloat4(&this->_transform->_quatForBone, quat);
	}
}
