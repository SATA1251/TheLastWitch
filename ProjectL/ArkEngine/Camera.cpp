#include <DirectXMath.h>
#include "ResourceManager.h"
#include "MathHeaders.h"
#include "Camera.h"

ArkEngine::ArkDX11::Camera::Camera(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, DirectX::XMFLOAT3& worldUp)
	: _positionVector(pos), _targetVector(target), _lookVector(), _rightVector(), _upVector(), _viewMatrix(), _projectionMatrix(),
	_isMain(false), _frustumPlanes()
{
	// ī�޶� �ʱ� ����
	LookAt(pos, target, worldUp);

	// Culling�� ���� 6���� �������� �־��ֱ� ���� �غ�
	for (int i = 0; i < 6; i++)
	{
		_frustumPlanes.emplace_back();
	}

	// ī�޶� �������� �������� ����
	UpdateFrustum();
}

ArkEngine::ArkDX11::Camera::~Camera()
{
	_frustumPlanes.clear();
}

void ArkEngine::ArkDX11::Camera::Update()
{
	// ī�޶� �̵��� ������ �������ҵ� �籸�� �ؾ���
	UpdateFrustum();
}

bool ArkEngine::ArkDX11::Camera::GetMain()
{
	return _isMain;
}

void ArkEngine::ArkDX11::Camera::SetMain(bool tf)
{
	_isMain = tf;
}

void ArkEngine::ArkDX11::Camera::Delete()
{
	ResourceManager::GetInstance()->DeleteCamera(this);
}

void ArkEngine::ArkDX11::Camera::SetProjectionMatrix(float fovY, float aspect, float nz, float fz, bool isPerspective)
{
	DirectX::XMMATRIX projMatrix;

	if (isPerspective)
	{
		projMatrix = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nz, fz);
	}
	else
	{
		projMatrix = DirectX::XMMatrixOrthographicLH(fovY, aspect, nz, fz);
	}

	DirectX::XMStoreFloat4x4(&_projectionMatrix, projMatrix);

	float zMinimum = -_projectionMatrix._43 / _projectionMatrix._33;

	_cameraSpaceDepth = 1000.0f / (1000.0f - zMinimum);
	_depthScalingFactor = -_cameraSpaceDepth * zMinimum;
}

void ArkEngine::ArkDX11::Camera::Strafe(float deltaTime)
{
	DirectX::XMVECTOR right = DirectX::XMLoadFloat3(&_rightVector);
	DirectX::XMVECTOR move = DirectX::XMVectorReplicate(deltaTime);
	DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&_positionVector);

	// ī�޶� �¿�� �̵���Ŵ
	DirectX::XMStoreFloat3(&_positionVector, DirectX::XMVectorMultiplyAdd(right, move, position));

	UpdateViewMatrix();
}

void ArkEngine::ArkDX11::Camera::Walk(float deltaTime)
{
	DirectX::XMVECTOR look = DirectX::XMLoadFloat3(&_lookVector);
	DirectX::XMVECTOR move = DirectX::XMVectorReplicate(deltaTime);
	DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&_positionVector);

	// ī�޶� �յڷ� �̵���Ŵ
	DirectX::XMStoreFloat3(&_positionVector, DirectX::XMVectorMultiplyAdd(look, move, position));

	UpdateViewMatrix();
}

void ArkEngine::ArkDX11::Camera::UpDown(float deltaTime)
{
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&_upVector);
	DirectX::XMVECTOR move = DirectX::XMVectorReplicate(deltaTime);
	DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&_positionVector);

	// ī�޶� ���Ʒ��� �̵���Ŵ
	DirectX::XMStoreFloat3(&_positionVector, DirectX::XMVectorMultiplyAdd(up, move, position));

	UpdateViewMatrix();
}

std::vector<DirectX::XMFLOAT4>& ArkEngine::ArkDX11::Camera::GetFrustum()
{
	return _frustumPlanes;
}


void ArkEngine::ArkDX11::Camera::SetCameraNewPosition(const DirectX::XMFLOAT3& cameraPos, const DirectX::XMFLOAT3& targetPos)
{
	auto worldUp = DirectX::XMFLOAT3{ 0.0f, 1.0f, 0.0f };
	LookAt(cameraPos, targetPos, worldUp);
}

DirectX::XMFLOAT3 ArkEngine::ArkDX11::Camera::GetCameraPosition()
{
	return _positionVector;
}

void ArkEngine::ArkDX11::Camera::SetCameraPosition(const DirectX::XMFLOAT3& position)
{
	_positionVector = position;

	UpdateViewMatrix();
}

void ArkEngine::ArkDX11::Camera::SetTargetPosition(const DirectX::XMFLOAT3& position)
{
	_targetVector = position;

	DirectX::XMFLOAT3 upVector = { 0.0f, 1.0f, 0.0f };

	LookAt(_positionVector, _targetVector, upVector);
}

void ArkEngine::ArkDX11::Camera::RotateCamera(const DirectX::XMFLOAT2& angle)
{
	float Pitch_X = DirectX::XMConvertToRadians(angle.x);
	float Yaw_y = DirectX::XMConvertToRadians(angle.y);

	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(Pitch_X, Yaw_y, 0.0f);

	DirectX::XMVECTOR rotatedDirection = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&_lookVector), rotationMatrix);

	DirectX::XMStoreFloat3(&_lookVector, rotatedDirection);

	DirectX::XMVECTOR lookV = DirectX::XMLoadFloat3(&_lookVector);

	DirectX::XMFLOAT3 worldUp = { 0.0f, 1.0f, 0.0f };
	DirectX::XMVECTOR worldUpV = DirectX::XMLoadFloat3(&worldUp);

	DirectX::XMVECTOR rightV = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(worldUpV, lookV));
	DirectX::XMStoreFloat3(&_rightVector, rightV);

	DirectX::XMVECTOR upV = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(lookV, rightV));
	DirectX::XMStoreFloat3(&_upVector, upV);

	UpdateViewMatrix();
}

void ArkEngine::ArkDX11::Camera::SetCameraPos(const DirectX::XMFLOAT3& targetPos, const DirectX::XMFLOAT3& direction, float scaleFactor)
{
	// Ÿ�� �����ǿ��� ������ ���� ���� ī�޶��� ��ġ�� ���
	DirectX::XMVECTOR myPos = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&targetPos), DirectX::XMLoadFloat3(&direction));

	// ���� �� �����ǿ��� sclaeFactor��ŭ, �� ���ϴ� �Ÿ���ŭ ������ ���ο� �������� ���
	auto newVec = DirectX::XMVectorScale(myPos, scaleFactor);

	DirectX::XMFLOAT3 myPosition;
	DirectX::XMStoreFloat3(&myPosition, newVec);

	// ī�޶��� ��ġ�� ���� ����� ���ͷ� ������Ʈ
	_positionVector = myPosition;

	// Ÿ�� �������� �״��
	_targetVector = targetPos;

	DirectX::XMFLOAT3 upVector = { 0.0f, 1.0f, 0.0f };

	// ���ο� ��ġ�� ���� view ��� �籸��
	LookAt(_positionVector, _targetVector, upVector);
}

void ArkEngine::ArkDX11::Camera::LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, DirectX::XMFLOAT3& worldUp)
{
	// ī�޶��� ������
	DirectX::XMVECTOR posVector = DirectX::XMLoadFloat3(&pos);
	// ī�޶� �ٶ󺸴� ���
	DirectX::XMVECTOR targetVector = DirectX::XMLoadFloat3(&target);
	// ���� ���������� �� ���� (y up�̹Ƿ� 0,1,0)
	DirectX::XMVECTOR worldUpVector = DirectX::XMLoadFloat3(&worldUp);
	
	// ī�޶� ���� ���͵� ����
	// Look Vector : Ÿ�� - �������� �ؼ� ī�޶� -> Ÿ������ �ٶ󺸴� ����
	DirectX::XMVECTOR lookVector = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(targetVector, posVector));
	DirectX::XMStoreFloat3(&_lookVector, lookVector);

	// Right Vector : world Up�� Look ���͸� ����
	DirectX::XMVECTOR rightVector = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(worldUpVector, lookVector));
	DirectX::XMStoreFloat3(&_rightVector, rightVector);

	// Up Vector : Right ���Ϳ� Look ���͸� ����
	DirectX::XMVECTOR upVector = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(lookVector, rightVector));
	DirectX::XMStoreFloat3(&_upVector, upVector);

	// ����� ����� ���� ī�޶� ������ view ��� ������Ʈ
	UpdateViewMatrix();
}

void ArkEngine::ArkDX11::Camera::UpdateViewMatrix()
{
	DirectX::XMVECTOR posV = DirectX::XMLoadFloat3(&_positionVector);
	DirectX::XMVECTOR rightV = DirectX::XMLoadFloat3(&_rightVector);
	DirectX::XMVECTOR upV = DirectX::XMLoadFloat3(&_upVector);
	DirectX::XMVECTOR lookV = DirectX::XMLoadFloat3(&_lookVector);

	float right = -(DirectX::XMVectorGetZ(DirectX::XMVector3Dot(posV, rightV)));
	float up = -(DirectX::XMVectorGetY(DirectX::XMVector3Dot(posV, upV)));
	float look = -(DirectX::XMVectorGetX(DirectX::XMVector3Dot(posV, lookV)));

	// ����� ���͸� ���� ī�޶� ������ view ��� ����
	_viewMatrix._11 = _rightVector.x;    _viewMatrix._12 = _upVector.x;    _viewMatrix._13 = _lookVector.x;    _viewMatrix._14 = 0.0f;
	_viewMatrix._21 = _rightVector.y;    _viewMatrix._22 = _upVector.y;    _viewMatrix._23 = _lookVector.y;    _viewMatrix._24 = 0.0f;
	_viewMatrix._31 = _rightVector.z;    _viewMatrix._32 = _upVector.z;    _viewMatrix._33 = _lookVector.z;    _viewMatrix._34 = 0.0f;
	_viewMatrix._41 = right;			 _viewMatrix._42 = up;			   _viewMatrix._43 = look;			    _viewMatrix._44 = 1.0f;
}

void ArkEngine::ArkDX11::Camera::UpdateFrustum()
{
	DirectX::XMFLOAT4X4 view, proj;
	DirectX::XMStoreFloat4x4(&view, this->GetViewMatrix());
	DirectX::XMStoreFloat4x4(&proj, this->GetProjMatrix());

	proj._33 = _cameraSpaceDepth;
	proj._43 = _depthScalingFactor;

	DirectX::XMMATRIX tempMatrix = DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&view), DirectX::XMLoadFloat4x4(&proj));

	DirectX::XMFLOAT4X4 matrix;
	DirectX::XMStoreFloat4x4(&matrix, tempMatrix);

	// near plane�� �������� ����
	_frustumPlanes[0].x = matrix._14 + matrix._13;
	_frustumPlanes[0].y = matrix._24 + matrix._23;
	_frustumPlanes[0].z = matrix._34 + matrix._33;
	_frustumPlanes[0].w = matrix._44 + matrix._43;

	DirectX::XMVECTOR nearPlane = DirectX::XMLoadFloat4(&_frustumPlanes[0]);
	DirectX::XMPlaneNormalize(nearPlane);
	DirectX::XMStoreFloat4(&_frustumPlanes[0], nearPlane);

	// far plane�� �������� ����
	_frustumPlanes[1].x = matrix._14 - matrix._13;
	_frustumPlanes[1].y = matrix._24 - matrix._23;
	_frustumPlanes[1].z = matrix._34 - matrix._33;
	_frustumPlanes[1].w = matrix._44 - matrix._43;

	DirectX::XMVECTOR farPlane = DirectX::XMLoadFloat4(&_frustumPlanes[1]);
	DirectX::XMPlaneNormalize(farPlane);
	DirectX::XMStoreFloat4(&_frustumPlanes[1], farPlane);

	// left plane�� �������� ����
	_frustumPlanes[2].x = matrix._14 + matrix._11;
	_frustumPlanes[2].y = matrix._24 + matrix._21;
	_frustumPlanes[2].z = matrix._34 + matrix._31;
	_frustumPlanes[2].w = matrix._44 + matrix._41;

	DirectX::XMVECTOR leftPlane = DirectX::XMLoadFloat4(&_frustumPlanes[2]);
	DirectX::XMPlaneNormalize(leftPlane);
	DirectX::XMStoreFloat4(&_frustumPlanes[2], leftPlane);

	// right plane�� �������� ����
	_frustumPlanes[3].x = matrix._14 - matrix._11;
	_frustumPlanes[3].y = matrix._24 - matrix._21;
	_frustumPlanes[3].z = matrix._34 - matrix._31;
	_frustumPlanes[3].w = matrix._44 - matrix._41;

	DirectX::XMVECTOR rightPlane = DirectX::XMLoadFloat4(&_frustumPlanes[3]);
	DirectX::XMPlaneNormalize(rightPlane);
	DirectX::XMStoreFloat4(&_frustumPlanes[3], rightPlane);

	// top plane�� �������� ����
	_frustumPlanes[4].x = matrix._14 - matrix._12;
	_frustumPlanes[4].y = matrix._24 - matrix._22;
	_frustumPlanes[4].z = matrix._34 - matrix._32;
	_frustumPlanes[4].w = matrix._44 - matrix._42;

	DirectX::XMVECTOR topPlane = DirectX::XMLoadFloat4(&_frustumPlanes[4]);
	DirectX::XMPlaneNormalize(topPlane);
	DirectX::XMStoreFloat4(&_frustumPlanes[4], topPlane);

	// bottom plane�� �������� ����
	_frustumPlanes[5].x = matrix._14 + matrix._12;
	_frustumPlanes[5].y = matrix._24 + matrix._22;
	_frustumPlanes[5].z = matrix._34 + matrix._32;
	_frustumPlanes[5].w = matrix._44 + matrix._42;

	DirectX::XMVECTOR bottomPlane = DirectX::XMLoadFloat4(&_frustumPlanes[5]);
	DirectX::XMPlaneNormalize(bottomPlane);
	DirectX::XMStoreFloat4(&_frustumPlanes[5], bottomPlane);
}

const DirectX::XMMATRIX ArkEngine::ArkDX11::Camera::GetViewMatrix()
{
	DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&_viewMatrix);

	return view;
}

const DirectX::XMMATRIX ArkEngine::ArkDX11::Camera::GetProjMatrix()
{
	DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&_projectionMatrix);

	return proj;
}

const DirectX::XMFLOAT3 ArkEngine::ArkDX11::Camera::GetCameraPos()
{
	return _positionVector;
}

const DirectX::XMFLOAT3 ArkEngine::ArkDX11::Camera::GetRightVector()
{
	return _rightVector;
}

const DirectX::XMFLOAT3 ArkEngine::ArkDX11::Camera::GetLookVector()
{
	return _lookVector;
}

const DirectX::XMFLOAT3 ArkEngine::ArkDX11::Camera::GetUpVector()
{
	return _upVector;
}
