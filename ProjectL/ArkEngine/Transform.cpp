#include <DirectXMath.h>
#include "Transform.h"

ArkEngine::ArkDX11::Transform::Transform()
	: _transform(), _translation(), _rotationX(), _rotationY(), _rotationZ(), _scale()
{
	Initialize();
}

ArkEngine::ArkDX11::Transform::~Transform()
{

}

void ArkEngine::ArkDX11::Transform::Initialize()
{
	SetTranslationMatrix();

	_rotationX._11 = 1;				_rotationX._12 = 0;						_rotationX._13 = 0;						_rotationX._14 = 0;
	_rotationX._21 = 0;				_rotationX._22 = 1;						_rotationX._23 = 0;						_rotationX._24 = 0;
	_rotationX._31 = 0;				_rotationX._32 = 0;						_rotationX._33 = 1;						_rotationX._34 = 0;
	_rotationX._41 = 0;				_rotationX._42 = 0;						_rotationX._43 = 0;						_rotationX._44 = 1;

	_rotationY._11 = 1;				_rotationY._12 = 0;						_rotationY._13 = 0;						_rotationY._14 = 0;
	_rotationY._21 = 0;				_rotationY._22 = 1;						_rotationY._23 = 0;						_rotationY._24 = 0;
	_rotationY._31 = 0;				_rotationY._32 = 0;						_rotationY._33 = 1;						_rotationY._34 = 0;
	_rotationY._41 = 0;				_rotationY._42 = 0;						_rotationY._43 = 0;						_rotationY._44 = 1;

	_rotationZ._11 = 1;				_rotationZ._12 = 0;						_rotationZ._13 = 0;						_rotationZ._14 = 0;
	_rotationZ._21 = 0;				_rotationZ._22 = 1;						_rotationZ._23 = 0;						_rotationZ._24 = 0;
	_rotationZ._31 = 0;				_rotationZ._32 = 0;						_rotationZ._33 = 1;						_rotationZ._34 = 0;
	_rotationZ._41 = 0;				_rotationZ._42 = 0;						_rotationZ._43 = 0;						_rotationZ._44 = 1;

	SetScaleMatrix();
}

const DirectX::XMMATRIX ArkEngine::ArkDX11::Transform::GetTransformMatrix()
{
	return DirectX::XMLoadFloat4x4(&_transform);
}

void ArkEngine::ArkDX11::Transform::CaculateTransform()
{
	DirectX::XMMATRIX translationMatirx = GetTranslationMatrix();
	DirectX::XMMATRIX rotationMatirx = GetRotatinMatrix();
	DirectX::XMMATRIX scaleMatirx = GetScaleMatrix();

	DirectX::XMMATRIX resultMatrix = DirectX::XMMatrixMultiply(scaleMatirx, rotationMatirx);
	resultMatrix = DirectX::XMMatrixMultiply(resultMatrix, translationMatirx);

	DirectX::XMStoreFloat4x4(&_transform, resultMatrix);
}

const DirectX::XMMATRIX ArkEngine::ArkDX11::Transform::GetTranslationMatrix() const
{
	DirectX::XMMATRIX translationMatirx = DirectX::XMLoadFloat4x4(&_translation);

	return translationMatirx;
}

const DirectX::XMVECTOR ArkEngine::ArkDX11::Transform::GetTranslationVector() const
{
	DirectX::XMVECTOR translationVector = DirectX::XMVectorSet(_translation._41, _translation._42, _translation._43, _translation._44);
	
	return translationVector;
}

const DirectX::XMMATRIX ArkEngine::ArkDX11::Transform::GetRotatinMatrix() const
{
	DirectX::XMMATRIX XrotationMatirx = DirectX::XMLoadFloat4x4(&_rotationX);
	DirectX::XMMATRIX YrotationMatirx = DirectX::XMLoadFloat4x4(&_rotationY);
	DirectX::XMMATRIX ZrotationMatirx = DirectX::XMLoadFloat4x4(&_rotationZ);

	DirectX::XMMATRIX resultMatrix = DirectX::XMMatrixMultiply(XrotationMatirx, YrotationMatirx);
	resultMatrix = DirectX::XMMatrixMultiply(resultMatrix, ZrotationMatirx);

	return resultMatrix;
}

const DirectX::XMMATRIX ArkEngine::ArkDX11::Transform::GetScaleMatrix() const
{
	DirectX::XMMATRIX scaleMatirx = DirectX::XMLoadFloat4x4(&_scale);

	return scaleMatirx;
}

void ArkEngine::ArkDX11::Transform::SetTransformMatrix(const DirectX::XMFLOAT4X4& matrix)
{
	_transform = matrix;
}

void ArkEngine::ArkDX11::Transform::SetTranslationMatrix(float translateX, float translateY, float translateZ)
{
	_translation._11 = 1;			_translation._12 = 0;			_translation._13 = 0;				_translation._14 = 0;
	_translation._21 = 0;			_translation._22 = 1;			_translation._23 = 0;				_translation._24 = 0;
	_translation._31 = 0;			_translation._32 = 0;			_translation._33 = 1;				_translation._34 = 0;
	_translation._41 = translateX;  _translation._42 = translateY;   _translation._43 = translateZ;	    _translation._44 = 1;

	CaculateTransform();
}

void ArkEngine::ArkDX11::Transform::SetRotationMatrix(float x, float y, float z)
{
	if (x != 0)
	{
		SetXRotationMatrix(x);
	}
	if (y != 0)
	{
		SetYRotationMatrix(y);
	}
	if (z != 0)
	{
		SetZRotationMatrix(z);
	}

	CaculateTransform();
}

void ArkEngine::ArkDX11::Transform::SetScaleMatrix(float scaleX /*= 1*/, float scaleY /*= 1*/, float scaleZ /*= 1*/)
{
	_scale._11 = scaleX;		_scale._12 = 0;				_scale._13 = 0;					_scale._14 = 0;
	_scale._21 = 0;				_scale._22 = scaleY;		_scale._23 = 0;					_scale._24 = 0;
	_scale._31 = 0;				_scale._32 = 0;				_scale._33 = scaleZ;			_scale._34 = 0;
	_scale._41 = 0;				_scale._42 = 0;				_scale._43 = 0;					_scale._44 = 1;

	CaculateTransform();
}

void ArkEngine::ArkDX11::Transform::SetXRotationMatrix(float rotationSize)
{
	float angle = rotationSize * (DirectX::XM_PI / 180.0f);
	
	//_rotationX._11 = 1;				_rotationX._12 = 0;						_rotationX._13 = 0;						_rotationX._14 = 0;
	//_rotationX._21 = 0;				_rotationX._22 = (float)cos(angle);		_rotationX._23 = (float)-sin(angle);	_rotationX._24 = 0;
	//_rotationX._31 = 0;				_rotationX._32 = (float)sin(angle);		_rotationX._33 = (float)cos(angle);		_rotationX._34 = 0;
	//_rotationX._41 = 0;				_rotationX._42 = 0;						_rotationX._43 = 0;						_rotationX._44 = 1;

	DirectX::XMStoreFloat4x4(&_rotationX, DirectX::XMMatrixRotationX(angle));

	CaculateTransform();
}

void ArkEngine::ArkDX11::Transform::SetYRotationMatrix(float rotationSize)
{
	float angle = rotationSize * (DirectX::XM_PI / 180.0f);
	
	//_rotationY._11 = (float)cos(angle);		_rotationY._12 = 0;				_rotationY._13 = (float)sin(angle);		_rotationY._14 = 0;
	//_rotationY._21 = 0;						_rotationY._22 = 1;				_rotationY._23 = 0;						_rotationY._24 = 0;
	//_rotationY._31 = (float)-sin(angle);		_rotationY._32 = 0;				_rotationY._33 = (float)cos(angle);		_rotationY._34 = 0;
	//_rotationY._41 = 0;						_rotationY._42 = 0;				_rotationY._43 = 0;						_rotationY._44 = 1;

	DirectX::XMStoreFloat4x4(&_rotationY, DirectX::XMMatrixRotationY(angle));

	CaculateTransform();
}

void ArkEngine::ArkDX11::Transform::SetZRotationMatrix(float rotationSize)
{
	float angle = rotationSize * (DirectX::XM_PI / 180.0f);
	
	//_rotationZ._11 = (float)cos(angle);		_rotationZ._12 = (float)-sin(angle);		_rotationZ._13 = 0;				_rotationZ._14 = 0;
	//_rotationZ._21 = (float)sin(angle);		_rotationZ._22 = (float)cos(angle);		_rotationZ._23 = 0;				_rotationZ._24 = 0;
	//_rotationZ._31 = 0;						_rotationZ._32 = 0;						_rotationZ._33 = 1;				_rotationZ._34 = 0;
	//_rotationZ._41 = 0;						_rotationZ._42 = 0;						_rotationZ._43 = 0;				_rotationZ._44 = 1;

	DirectX::XMStoreFloat4x4(&_rotationZ, DirectX::XMMatrixRotationZ(angle));

	CaculateTransform();
}