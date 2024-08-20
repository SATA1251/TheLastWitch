#define NOMINMAX

#include <DirectXMath.h>
#include <algorithm>
#include "MathHeaders.h"
#include "GeometryGenerator.h"
#include "ArkBuffer.h"
#include "CommonStruct.h"
#include "ResourceManager.h"
#include "Transform.h"
#include "ArkEffect.h"
#include "ArkDevice.h"
#include "ICamera.h"
#include "Camera.h"
#include "d3dx11effect.h"
#include "DebugObject.h"

ArkEngine::ArkDX11::DebugObject::DebugObject(const std::string& objectName, eDebugType type, const DirectX::XMFLOAT4& color)
	: _objectName(objectName), _effectName("Resources/FX/color.fx"), _effect(nullptr), _tech(nullptr),
	_fxWorld(nullptr), _fxWorldViewProj(nullptr), _fxFrustum(nullptr),
	_world(), _view(), _proj(), _vertexBuffer(nullptr), _indexBuffer(nullptr), _arkDevice(nullptr), _arkEffect(nullptr),
	_totalIndexCount(0), _isRendering(true), _meshTransform(nullptr), _color(color), _halfWidth(0.0f), _halfHeight(0.0f), _halfDepth(0.0f), _minPos(0.0f), _type(type),
	_renderableState(true)
{
	const auto& buffers = ResourceManager::GetInstance()->GetArkBuffer(_objectName);

	DirectX::XMFLOAT3 maxPos = buffers[0]->GetMaxPos();
	DirectX::XMFLOAT3 minPos = buffers[0]->GetMinPos();

	for (const auto& index : buffers)
	{
		minPos.x = std::min(minPos.x, index->GetMinPos().x);
		minPos.y = std::min(minPos.y, index->GetMinPos().y);
		minPos.z = std::min(minPos.z, index->GetMinPos().z);

		maxPos.x = std::max(maxPos.x, index->GetMaxPos().x);
		maxPos.y = std::max(maxPos.y, index->GetMaxPos().y);
		maxPos.z = std::max(maxPos.z, index->GetMaxPos().z);
	}

	_halfWidth = std::abs(maxPos.x - minPos.x) / 2;
	_halfHeight = std::abs(maxPos.y - minPos.y) / 2;
	_halfDepth = std::abs(maxPos.z - minPos.z) / 2;
	_minPos = minPos.y;

	_centerPos.x = (maxPos.x + minPos.x) / 2;
	_centerPos.y = (maxPos.y + minPos.y) / 2 - _halfHeight;
	_centerPos.z = (maxPos.z + minPos.z) / 2;

	_objectName += "/Debug";

	Initialize();
}


ArkEngine::ArkDX11::DebugObject::DebugObject(const std::string& objectName, std::vector<DirectX::XMFLOAT3>& vertexVec, std::vector<unsigned int>& indexVec)
	: _objectName(objectName), _effectName("Resources/FX/color.fx"), _effect(nullptr), _tech(nullptr),
	_fxWorld(nullptr), _fxWorldViewProj(nullptr), _fxFrustum(nullptr),
	_world(), _view(), _proj(), _vertexBuffer(nullptr), _indexBuffer(nullptr), _arkDevice(nullptr), _arkEffect(nullptr),
	_totalIndexCount(0), _isRendering(true), _meshTransform(nullptr), _halfWidth(0.0f), _halfHeight(0.0f), _halfDepth(0.0f), _minPos(0.0f), _type(eDebugType::Map),
	_renderableState(true)
{
	_arkEffect = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkEffect>(_effectName);
	_effect = _arkEffect->GetEffect();

	_arkDevice = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkDevice>("Device");

	BuildMapBuffers(vertexVec, indexVec);
	SetEffect();

	ResourceManager::GetInstance()->AddDebugObject(this);

	_meshTransform = new Transform();
}

ArkEngine::ArkDX11::DebugObject::DebugObject(const std::string& objectName, float width, float height, float depth, const DirectX::XMFLOAT4& color)
	: _objectName(objectName), _effectName("Resources/FX/color.fx"), _effect(nullptr), _tech(nullptr),
	_fxWorld(nullptr), _fxWorldViewProj(nullptr), _fxFrustum(nullptr),
	_world(), _view(), _proj(), _vertexBuffer(nullptr), _indexBuffer(nullptr), _arkDevice(nullptr), _arkEffect(nullptr),
	_totalIndexCount(0), _isRendering(true), _meshTransform(nullptr), _color(color), _halfWidth(width / 2), _halfHeight(height / 2), _halfDepth(depth / 2), _minPos(0.0f - _halfHeight), _type(eDebugType::PhysXBox),
	_renderableState(true)
{
	auto buffer = ResourceManager::GetInstance()->GetResource<ArkBuffer>(_objectName);

	_objectName += "/Debug";

	Initialize();
}

ArkEngine::ArkDX11::DebugObject::DebugObject(const std::string& objectName, float centerPosY, float range, const DirectX::XMFLOAT4& color)
	: _objectName(objectName), _effectName("Resources/FX/color.fx"), _effect(nullptr), _tech(nullptr),
	_fxWorld(nullptr), _fxWorldViewProj(nullptr), _fxFrustum(nullptr),
	_world(), _view(), _proj(), _vertexBuffer(nullptr), _indexBuffer(nullptr), _arkDevice(nullptr), _arkEffect(nullptr),
	_totalIndexCount(0), _isRendering(true), _meshTransform(nullptr), _color(color), _halfWidth(0.0f), _halfHeight(0.0f), _halfDepth(0.0f), _minPos(0.0f), _type(eDebugType::Shpere),
	_renderableState(true)
{
	_halfWidth = std::abs(range / 2);
	_halfHeight = std::abs(range / 2);
	_halfDepth = std::abs(range / 2);
	_minPos = centerPosY - (range / 2);

	_objectName += "/Debug";

	Initialize();
}

ArkEngine::ArkDX11::DebugObject::~DebugObject()
{
	Finalize();
}


bool ArkEngine::ArkDX11::DebugObject::GetActive()
{
	return _renderableState;
}

void ArkEngine::ArkDX11::DebugObject::SetActive(bool tf)
{
	_renderableState = tf;
}

void ArkEngine::ArkDX11::DebugObject::Initialize()
{
	_arkEffect = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkEffect>(_effectName);
	_effect = _arkEffect->GetEffect();

	_arkDevice = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkDevice>("Device");

	BuildGeomtryBuffers();
	SetEffect();

	ResourceManager::GetInstance()->AddDebugObject(this);

	_meshTransform = new Transform();
}

void ArkEngine::ArkDX11::DebugObject::Update(ArkEngine::ICamera* p_Camera)
{
	DirectX::XMStoreFloat4x4(&_world, _meshTransform->GetTransformMatrix());
	DirectX::XMStoreFloat4x4(&_view, p_Camera->GetViewMatrix());
	DirectX::XMStoreFloat4x4(&_proj, p_Camera->GetProjMatrix());

	_fxFrustum->SetFloatVectorArray(reinterpret_cast<float*>(&p_Camera->GetFrustum()[0]), 0, static_cast<uint32_t>(p_Camera->GetFrustum().size()));

	CheckFrustum(p_Camera);
}

void ArkEngine::ArkDX11::DebugObject::Render()
{
	auto deviceContext = _arkDevice->GetDeviceContext();

	deviceContext->IASetInputLayout(_arkEffect->GetInputLayOut());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->RSSetState(_arkDevice->GetWireRS());

// 	if (_objectName.find("navmesh") != std::string::npos)
// 	{
// 		deviceContext->RSSetState(_arkDevice->GetSolidRS());
// 	}

	UINT stride = sizeof(ArkEngine::ArkDX11::PosColor);
	UINT offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	D3DX11_TECHNIQUE_DESC techDesc;
	_tech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		DirectX::XMMATRIX world = XMLoadFloat4x4(&_world);

		_fxWorld->SetMatrix(reinterpret_cast<float*>(&world));
		DirectX::XMMATRIX view = XMLoadFloat4x4(&_view);
		DirectX::XMMATRIX proj = XMLoadFloat4x4(&_proj);
		DirectX::XMMATRIX WorldViewProj = world * view * proj;
		_fxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&WorldViewProj));

		_tech->GetPassByIndex(p)->Apply(0, deviceContext);
		deviceContext->DrawIndexed(_totalIndexCount, 0, 0);
	}
}

void ArkEngine::ArkDX11::DebugObject::Finalize()
{
	ResourceManager::GetInstance()->DeleteDebugObject(this);

	delete _meshTransform;

	_arkEffect = nullptr;
	_arkDevice = nullptr;

	_indexBuffer = nullptr;
	_vertexBuffer = nullptr;

	_fxFrustum->Release();
	_fxWorldViewProj->Release();
	_fxWorld->Release();

	_tech = nullptr;
	_effect = nullptr;
}


void ArkEngine::ArkDX11::DebugObject::ReleaseWithBuffer()
{
	ResourceManager::GetInstance()->DeleteResource(_objectName);

	Finalize();
}

const std::string ArkEngine::ArkDX11::DebugObject::GetName()
{
	return _objectName;
}

bool ArkEngine::ArkDX11::DebugObject::GetRenderingState()
{
	return _isRendering;
}

void ArkEngine::ArkDX11::DebugObject::SetRenderingState(bool tf)
{
	_isRendering = tf;
}

void ArkEngine::ArkDX11::DebugObject::SetTransform(const DirectX::XMFLOAT4X4& matrix)
{
	_meshTransform->SetTransformMatrix(matrix);
}

void ArkEngine::ArkDX11::DebugObject::SetTransformMatrix(const DirectX::XMFLOAT4X4& matrix)
{
	_meshTransform->SetTransformMatrix(matrix);
}

void ArkEngine::ArkDX11::DebugObject::SetPosition(float x, float y, float z)
{
	_meshTransform->SetTranslationMatrix(x, y, z);
}

void ArkEngine::ArkDX11::DebugObject::SetRotation(float x, float y, float z)
{
	_meshTransform->SetRotationMatrix(x, y, z);
}

void ArkEngine::ArkDX11::DebugObject::SetScale(float x, float y, float z)
{
	_meshTransform->SetScaleMatrix(x, y, z);
}

void ArkEngine::ArkDX11::DebugObject::Delete()
{
	delete this;
}

void ArkEngine::ArkDX11::DebugObject::SetWorld(const DirectX::XMFLOAT4X4& matrix)
{
	_world = matrix;
}

float ArkEngine::ArkDX11::DebugObject::GetRadius()
{
	return _halfWidth;
}

void ArkEngine::ArkDX11::DebugObject::CheckFrustum(ArkEngine::ICamera* p_Camera)
{
	auto buffer = ResourceManager::GetInstance()->GetResource<ArkBuffer>(_objectName);
	const auto& vertexPosList = buffer->GetVertexPosList();

	const auto& frustum = p_Camera->GetFrustum();

	int outFrustum = 0;

	// 카메라 기준으로 만들어진 6개의 프러스텀을 검사
	for (int i = 0; i < 6; i++)
	{
		DirectX::XMFLOAT3 planeNormal = { frustum[i].x, frustum[i].y, frustum[i].z };
		float planeD = frustum[i].w;
		float distance = 0.0f;

		for (int i = 0; i < vertexPosList.size(); i++)
		{
			// 버텍스 버퍼에 들어있던 버텍스의 좌표를 실제 월드상의 좌표로 변환
			auto finalPos = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&vertexPosList[i]), _meshTransform->GetTransformMatrix());
			auto tempResult = DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&planeNormal), finalPos);
			distance = DirectX::XMVectorGetX(tempResult) + planeD;

			// 프러스텀 (시야) 내부에 들어올 경우
			if (distance > 0.0f)
			{
				outFrustum++;
				break;
			}
		}
	}

	// 버텍스가 하나라도 들어온 경우
	if (outFrustum == 6)
	{
		_isRendering = true;
	}
	// 버텍스 중 하나도 들어오지 않은 경우
	else
	{
		_isRendering = false;
	}
}

void ArkEngine::ArkDX11::DebugObject::BuildGeomtryBuffers()
{
	// 만들어진 버퍼가 있다면 가져옴
	auto buffer = ResourceManager::GetInstance()->GetResource<ArkBuffer>(_objectName);

	GeometryGenerator geomGenerator;

	// 버퍼가 없다면 타입에 맞는 버퍼를 생성
	if (buffer == nullptr)
	{
		if (_type == eDebugType::Cube)
		{
			geomGenerator.CreateDebugBox(_objectName.c_str(), _centerPos, _halfWidth * 2, _halfHeight * 2, _halfDepth * 2, _color);
		}
		else if (_type == eDebugType::PhysXBox)
		{
			geomGenerator.CreateDebugPhysXBox(_objectName.c_str(), _centerPos, _halfWidth * 2, _halfHeight * 2, _halfDepth * 2, _color);
		}
		else if (_type == eDebugType::Shpere)
		{
			geomGenerator.CreateDebugSphere(_objectName.c_str(), _minPos, _halfWidth * 2, _color);
		}

		buffer = ResourceManager::GetInstance()->GetResource<ArkBuffer>(_objectName);
	}

	_vertexBuffer = buffer->GetVertexBuffer();
	_indexBuffer = buffer->GetIndexBuffer();

	_totalIndexCount = buffer->GetTotalIndexCount();
}


void ArkEngine::ArkDX11::DebugObject::BuildMapBuffers(std::vector<DirectX::XMFLOAT3>& vertexVec, std::vector<unsigned int>& indexVec)
{
	auto buffer = ResourceManager::GetInstance()->GetResource<ArkBuffer>(_objectName);

	if (buffer == nullptr)
	{
		std::vector<PosColor> vertexList;
		std::vector<unsigned int> indexList;

		for (const auto& index : vertexVec)
		{
			PosColor newPosColor;
			newPosColor.color = { 0.0f, 0.4f, 0.5f, 0.9f };
			newPosColor.pos = index;

			vertexList.emplace_back(newPosColor);
		}

		indexList = indexVec;

		buffer = new ArkBuffer(_objectName.c_str(), vertexList.size(), vertexList, indexList.size(), indexList);
	}

	_vertexBuffer = buffer->GetVertexBuffer();
	_indexBuffer = buffer->GetIndexBuffer();

	_totalIndexCount = buffer->GetTotalIndexCount();
}

void ArkEngine::ArkDX11::DebugObject::SetEffect()
{
	_tech = _effect->GetTechniqueByIndex(0);
	_fxWorld = _effect->GetVariableByName("gWorld")->AsMatrix();
	_fxWorldViewProj = _effect->GetVariableByName("gWorldViewProj")->AsMatrix();

	_fxFrustum = _effect->GetVariableByName("gFrustum")->AsVector();
}