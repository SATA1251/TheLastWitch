#include "Transform.h"
#include "GameObject.h"
#include "ToolBox.h"

KunrealEngine::Transform::Transform()
	:_position({ 0.0f, 0.0f, 0.0f }), _rotation({ 0.0f, 0.0f, 0.0f }), _scale({ 1.0f, 1.0f, 1.0f }),
	_worldTM(), _quaternion({0.0f, 0.0f, 0.0f, 1.0f}), _haveParentBone(false),
	_posForBone({0.0f, 0.0f, 0.0f}), _quatForBone({0.0f, 0.0f, 0.0f, 1.0f})
{
	DirectX::XMStoreFloat4x4(&_worldTM, DirectX::XMMatrixIdentity());
}

KunrealEngine::Transform::~Transform()
{

}

void KunrealEngine::Transform::Initialize()
{

}

void KunrealEngine::Transform::Release()
{

}

void KunrealEngine::Transform::FixedUpdate()
{

}

void KunrealEngine::Transform::Update()
{
	CreateWorldTransformMatrix();
}

void KunrealEngine::Transform::LateUpdate()
{

}

void KunrealEngine::Transform::OnTriggerEnter()
{

}

void KunrealEngine::Transform::OnTriggerStay()
{

}

void KunrealEngine::Transform::OnTriggerExit()
{

}

void KunrealEngine::Transform::SetActive(bool active)
{
	
}

void KunrealEngine::Transform::SetRotation(float x, float y, float z)
{
	this->_rotation.x = x;
	this->_rotation.y = y;
	this->_rotation.z = z;

	// local의 회전값을 world로 변환해주기 위한 쿼터니언
	DirectX::XMVECTOR rotationPitchYaw = DirectX::XMQuaternionRotationRollPitchYaw(DirectX::XMConvertToRadians(_rotation.x), DirectX::XMConvertToRadians(_rotation.y), DirectX::XMConvertToRadians(_rotation.z));
	DirectX::XMStoreFloat4(&_quaternion, rotationPitchYaw);

	CreateWorldTransformMatrix();
}


void KunrealEngine::Transform::SetRotation(const DirectX::XMFLOAT3& rot)
{
	this->_rotation = rot;

	CreateWorldTransformMatrix();
}

void KunrealEngine::Transform::SetPosition(float x, float y, float z)
{
	this->_position.x = x;
	this->_position.y = y;
	this->_position.z = z;

	CreateWorldTransformMatrix();
}


void KunrealEngine::Transform::SetPosition(const DirectX::XMFLOAT3& pos)
{
	this->_position = pos;

	CreateWorldTransformMatrix();
}

void KunrealEngine::Transform::SetScale(float x, float y, float z)
{
	this->_scale.x = x;
	this->_scale.y = y;
	this->_scale.z = z;

	CreateWorldTransformMatrix();
}


void KunrealEngine::Transform::SetTotalScale(float scale)
{
	this->_scale.x = scale;
	this->_scale.y = scale;
	this->_scale.z = scale;

	CreateWorldTransformMatrix();
}

void KunrealEngine::Transform::SetScale(const DirectX::XMFLOAT3& scale)
{
	this->_scale = scale;

	CreateWorldTransformMatrix();
}

DirectX::XMFLOAT3 KunrealEngine::Transform::GetPosition()
{
	return this->_position;
}

DirectX::XMFLOAT3 KunrealEngine::Transform::GetRotation()
{
	return this->_rotation;
}

DirectX::XMFLOAT4 KunrealEngine::Transform::GetQuaternion()
{
	return this->_quaternion;
}

DirectX::XMFLOAT3 KunrealEngine::Transform::GetScale()
{
	return this->_scale;
}

DirectX::XMFLOAT4X4 KunrealEngine::Transform::CreateWorldTransformMatrix()
{
	DirectX::XMStoreFloat4x4(&_worldTM, DirectX::XMMatrixScaling(_scale.x, _scale.y, _scale.z)
		* DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&this->_quaternion))
		* DirectX::XMMatrixTranslation(_position.x, _position.y, _position.z));

	//DirectX::XMStoreFloat4x4(&_worldTM, DirectX::XMMatrixTranslation(_position.x, _position.y, _position.z)
	//	* DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&this->_quaternion))
	//	* DirectX::XMMatrixScaling(_scale.x, _scale.y, _scale.z));

	return _worldTM;
	
}

DirectX::XMFLOAT4X4 KunrealEngine::Transform::GetWorldTM()
{
	if (this->GetOwner()->GetParent() != nullptr)
	{
		DirectX::XMFLOAT4X4 temp = this->GetOwner()->GetParent()->GetComponent<Transform>()->GetWorldTM();
		DirectX::XMMATRIX parentMatrix = DirectX::XMLoadFloat4x4(&temp);
		DirectX::XMMATRIX matrix = DirectX::XMLoadFloat4x4(&this->_worldTM);
	
		//DirectX::XMMATRIX mult = parentMatrix * matrix;
		DirectX::XMMATRIX mult = matrix * parentMatrix;
	
		DirectX::XMFLOAT4X4 result;
		DirectX::XMStoreFloat4x4(&result, mult);
	
		return result;
	}

	return this->_worldTM;
}

void KunrealEngine::Transform::SetPositionWithGizmo(float x, float y, float z)
{
	this->_position.x = x;
	this->_position.y = y;
	this->_position.z = z;

	CreateWorldTransformMatrix();

	DirectX::XMFLOAT4X4 temp = this->GetOwner()->GetParent()->GetComponent<Transform>()->GetWorldTM();
	DirectX::XMMATRIX parentMatrix = DirectX::XMLoadFloat4x4(&temp);
	DirectX::XMMATRIX localMatrix = DirectX::XMLoadFloat4x4(&this->_worldTM);

	DirectX::XMMATRIX inverse = DirectX::XMMatrixInverse(nullptr, parentMatrix);
	DirectX::XMMATRIX recalcMatrix = localMatrix * inverse;

	DirectX::XMVECTOR scale, quaternion, translation;

	DirectX::XMMatrixDecompose(&scale, &quaternion, &translation, recalcMatrix);
	DirectX::XMStoreFloat3(&this->_position, translation);
}

void KunrealEngine::Transform::RecalculateTransform()
{
	DirectX::XMFLOAT4X4 temp = this->GetOwner()->GetParent()->GetComponent<Transform>()->GetWorldTM();
	DirectX::XMMATRIX parentMatrix = DirectX::XMLoadFloat4x4(&temp);
	DirectX::XMMATRIX localMatrix = DirectX::XMLoadFloat4x4(&this->_worldTM);

	DirectX::XMMATRIX inverse = DirectX::XMMatrixInverse(nullptr, parentMatrix);
	DirectX::XMMATRIX recalcMatrix = localMatrix * inverse;

	DirectX::XMVECTOR scale, quaternion, translation;

	DirectX::XMMatrixDecompose(&scale, &quaternion, &translation, recalcMatrix);
	DirectX::XMStoreFloat3(&this->_position, translation);
	DirectX::XMStoreFloat4(&this->_quaternion, quaternion);
	DirectX::XMStoreFloat3(&this->_scale, scale);

	this->_rotation = ToolBox::QuaternionToEulerAngles(quaternion);
}

void KunrealEngine::Transform::RevertToOriginal()
{
	DirectX::XMFLOAT4X4 temp = this->GetOwner()->GetParent()->GetComponent<Transform>()->GetWorldTM();
	DirectX::XMMATRIX parentMatrix = DirectX::XMLoadFloat4x4(&temp);
	DirectX::XMMATRIX localMatrix = DirectX::XMLoadFloat4x4(&this->_worldTM);

	DirectX::XMMATRIX inverse = DirectX::XMMatrixInverse(nullptr, parentMatrix);
	DirectX::XMMATRIX recalcMatrix = localMatrix * inverse;

	DirectX::XMVECTOR scale, quaternion, translation;

	DirectX::XMMatrixDecompose(&scale, &quaternion, &translation, recalcMatrix);
	DirectX::XMStoreFloat3(&this->_position, translation);
	DirectX::XMStoreFloat4(&this->_quaternion, quaternion);
	DirectX::XMStoreFloat3(&this->_scale, scale);

	this->_rotation = ToolBox::QuaternionToEulerAngles(quaternion);
}

