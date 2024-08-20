#include "ResourceManager.h"
#include "ArkEffect.h"
#include "ArkDevice.h"
#include "ArkBuffer.h"
#include "ArkTexture.h"
#include "d3dx11effect.h"
#include "FBXMesh.h"
#include "ICamera.h"
#include "Transform.h"
#include "GeometryGenerator.h"
#include "CommonStruct.h"
#include "TransparentMesh.h"

ArkEngine::ArkDX11::TransparentMesh::TransparentMesh(const std::string& objectName, const std::string& textureName, float transParency, bool isCircle)
	: _objectName(objectName), _textureName(textureName), _transParency(transParency),
	_arkEffect(nullptr), _effect(nullptr), _arkDevice(nullptr),
	_meshTransform(nullptr), _vertexBuffer(nullptr), _indexBuffer(nullptr), _totalIndexCount(0),
	_tech(nullptr), _fxWorld(nullptr), _fxWorldViewProj(nullptr), _world(), _view(), _proj(),
	_fxTransParency(nullptr), _isCircle(isCircle),
	_timer(0.0f), _renderType(0),
	_renderTime(0.0f), _isRenderFinsh(true), _isRenderStart(false),
	_fxDonutCenter(nullptr), _fxDonutRange(nullptr), _donutCenter(), _donutRange(0.0f), _index(-1), _isApplyDecal(false), _isInfinite(false), _isApplyPattern(false)
{
	Initialize();
}

ArkEngine::ArkDX11::TransparentMesh::~TransparentMesh()
{
	Finalize();
}

void ArkEngine::ArkDX11::TransparentMesh::Initialize()
{
	_arkEffect = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkEffect>("Resources/FX/TransparentObject.fx");
	_effect = _arkEffect->GetEffect();

	_arkDevice = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkDevice>("Device");

	BuildGeomtryBuffers();
	SetEffect();
	SetTexture(_textureName);

	_meshTransform = new Transform();

	ResourceManager::GetInstance()->AddTransParentMesh(this);
}

void ArkEngine::ArkDX11::TransparentMesh::Update(ArkEngine::ICamera* p_Camera)
{
	if (!_isRenderFinsh)
	{
		DirectX::XMStoreFloat4x4(&_world, _meshTransform->GetTransformMatrix());
		DirectX::XMStoreFloat4x4(&_view, p_Camera->GetViewMatrix());
		DirectX::XMStoreFloat4x4(&_proj, p_Camera->GetProjMatrix());
	}
}

void ArkEngine::ArkDX11::TransparentMesh::Render(std::vector<DirectX::XMFLOAT4X4>& worldVec)
{
	if (!_isRenderFinsh && _isRenderStart || _isInfinite && _isRenderStart)
	{
		auto deviceContext = _arkDevice->GetDeviceContext();

		deviceContext->IASetInputLayout(_arkEffect->GetInputLayOut());
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		deviceContext->RSSetState(_arkDevice->GetSolidRS());

		UINT stride = sizeof(ArkEngine::ArkDX11::Postex);
		UINT offset = 0;

		deviceContext->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
		deviceContext->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		D3DX11_TECHNIQUE_DESC techDesc;
		_tech->GetDesc(&techDesc);

		DirectX::XMMATRIX world = XMLoadFloat4x4(&_world);

		// 데칼을 위함
		if (_isApplyDecal)
		{
			auto invWorldMat = DirectX::XMMatrixInverse(nullptr, world);
			DirectX::XMFLOAT4X4 invWorld;
			DirectX::XMStoreFloat4x4(&invWorld, invWorldMat);

			worldVec.emplace_back(invWorld);
			_index = worldVec.size() - 1;

			ResourceManager::GetInstance()->AddDecalMesh(this);
		}

		DirectX::XMMATRIX view = XMLoadFloat4x4(&_view);
		DirectX::XMMATRIX proj = XMLoadFloat4x4(&_proj);
		DirectX::XMMATRIX WorldViewProj = world * view * proj;

		_fxWorld->SetMatrix(reinterpret_cast<float*>(&world));
		_fxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&WorldViewProj));
		_texture->SetResource(_diffuseMapSRV);

		_fxTransParency->SetFloat(_transParency);

		_fxTime->SetFloat(_renderTime);

		_fxIndex->SetInt(_index);

		if (_renderType >= 5 && _renderType < 7)
		{
			_fxDonutCenter->SetFloatVector(reinterpret_cast<float*>(&_donutCenter));
			_fxDonutRange->SetFloat(_donutRange);
		}

		if (!_isApplyDecal)
		{
			_tech->GetPassByIndex(_renderType)->Apply(0, deviceContext);
		}
		else
		{
			_tech->GetPassByIndex(7)->Apply(0, deviceContext);
		}

		deviceContext->DrawIndexed(_totalIndexCount, 0, 0);
	}
}

void ArkEngine::ArkDX11::TransparentMesh::Reset()
{
	_isRenderStart = false;
	_timer = 0.0f;
	_renderTime = 0.0f;
	_isRenderFinsh = false;
}

void ArkEngine::ArkDX11::TransparentMesh::SetRenderType(unsigned int index)
{
	D3DX11_TECHNIQUE_DESC techDesc;
	_tech->GetDesc(&techDesc);

	if (index < techDesc.Passes)
	{
		_renderType = index;
	}
	else
	{
		_renderType = 0;
	}
}

void ArkEngine::ArkDX11::TransparentMesh::Finalize()
{
	delete _meshTransform;

	_arkEffect = nullptr;
	_arkDevice = nullptr;

	_indexBuffer = nullptr;
	_vertexBuffer = nullptr;

	_fxWorldViewProj->Release();
	_fxWorld->Release();

	_texture->Release();

	_fxTransParency->Release();

	_fxIndex->Release();

	_fxTime->Release();

	_fxDonutCenter->Release();

	_fxDonutRange->Release();

	_tech = nullptr;
	_effect = nullptr;
}


float ArkEngine::ArkDX11::TransparentMesh::GetTransParency()
{
	return _timer;
}


void ArkEngine::ArkDX11::TransparentMesh::UpPosition(float up)
{
	auto transform = _meshTransform->GetTransformMatrix();

	transform.r[3].m128_f32[1] += up;

	DirectX::XMFLOAT4X4 result;
	DirectX::XMStoreFloat4x4(&result, transform);

	_meshTransform->SetTransformMatrix(result);
}


ID3D11ShaderResourceView* ArkEngine::ArkDX11::TransparentMesh::GetTexture()
{
	return _diffuseMapSRV;
}


int ArkEngine::ArkDX11::TransparentMesh::GetIndex()
{
	return _index;
}


float ArkEngine::ArkDX11::TransparentMesh::GetTimer()
{
	return _renderTime;
}

void ArkEngine::ArkDX11::TransparentMesh::SetTransform(const DirectX::XMFLOAT4X4& matrix)
{
	_meshTransform->SetTransformMatrix(matrix);
}

void ArkEngine::ArkDX11::TransparentMesh::SetExceptRange(const DirectX::XMFLOAT3& center, float range)
{
	_donutCenter = center;
	_donutRange = range;
}


void ArkEngine::ArkDX11::TransparentMesh::SetStartFlag(bool tf)
{
	_isRenderStart = tf;
}

void ArkEngine::ArkDX11::TransparentMesh::Delete()
{
	ResourceManager::GetInstance()->DeleteTransParentMesh(this);
}


void ArkEngine::ArkDX11::TransparentMesh::SetDecal(bool tf)
{
	_isApplyDecal = tf;
}


void ArkEngine::ArkDX11::TransparentMesh::SetInfinite(bool tf)
{
	_isInfinite = tf;
}


bool ArkEngine::ArkDX11::TransparentMesh::GetInfiniteState()
{
	return _isInfinite;
}


void ArkEngine::ArkDX11::TransparentMesh::ApplyPattern(bool tf)
{
	_isApplyPattern = tf;
}


bool ArkEngine::ArkDX11::TransparentMesh::GetIsApplyPattern()
{
	return _isApplyPattern;
}


float ArkEngine::ArkDX11::TransparentMesh::GetRenderTimer()
{
	return _timer;
}

void ArkEngine::ArkDX11::TransparentMesh::BuildGeomtryBuffers()
{
	ArkBuffer* buffer = nullptr;

	// 만들어진 버퍼가 있다면 가져옴
	if (_isCircle == true)
	{
		buffer = ResourceManager::GetInstance()->GetResource<ArkBuffer>("Circle");

		if (buffer == nullptr)
		{
			GeometryGenerator generator;

			generator.CreateCircle("Circle");

			buffer = ResourceManager::GetInstance()->GetResource<ArkBuffer>("Circle");
		}
	}
	else
	{
		if (_isApplyDecal)
		{
			buffer = ResourceManager::GetInstance()->GetResource<ArkBuffer>("3dQuad");

			if (buffer == nullptr)
			{
				GeometryGenerator generator;

				generator.Create3dQuad();

				buffer = ResourceManager::GetInstance()->GetResource<ArkBuffer>("3dQuad");
			}
		}
		else
		{
			buffer = ResourceManager::GetInstance()->GetResource<ArkBuffer>("FlatQuad");

			if (buffer == nullptr)
			{
				GeometryGenerator generator;

				generator.CreateFlatQuad();

				buffer = ResourceManager::GetInstance()->GetResource<ArkBuffer>("FlatQuad");
			}
		}
	}

	_vertexBuffer = buffer->GetVertexBuffer();
	_indexBuffer = buffer->GetIndexBuffer();

	_totalIndexCount = buffer->GetTotalIndexCount();
}

void ArkEngine::ArkDX11::TransparentMesh::SetEffect()
{
	_tech = _effect->GetTechniqueByIndex(0);
	_fxWorld = _effect->GetVariableByName("gWorld")->AsMatrix();
	_fxWorldViewProj = _effect->GetVariableByName("gWorldViewProj")->AsMatrix();

	_texture = _effect->GetVariableByName("gDiffuseMap")->AsShaderResource();

	_fxTransParency = _effect->GetVariableByName("gTransParency")->AsScalar();

	_fxTime = _effect->GetVariableByName("gTime")->AsScalar();

	_fxIndex = _effect->GetVariableByName("gIndex")->AsScalar();

	_fxDonutCenter = _effect->GetVariableByName("gDonutCenter")->AsVector();

	_fxDonutRange = _effect->GetVariableByName("gDonutRange")->AsScalar();
}

void ArkEngine::ArkDX11::TransparentMesh::SetTexture(const std::string& textureName)
{
	auto texture = ResourceManager::GetInstance()->GetResource<ArkTexture>(textureName);

	if (texture != nullptr)
	{
		_diffuseMapSRV = texture->GetDiffuseMapSRV();
	}
	else
	{
		ArkTexture* newTexture = new ArkTexture(textureName.c_str());
		_diffuseMapSRV = newTexture->GetDiffuseMapSRV();
	}
}

bool ArkEngine::ArkDX11::TransparentMesh::RenderWithTimer(float deltaTime, float timer)
{
	_timer += deltaTime;

	if (_isInfinite)
	{
		return false;
	}

	if (_isRenderFinsh)
	{
		return _isRenderFinsh;
	}

	if (_timer >= timer)
	{
		_isRenderFinsh = true;
	
		return _isRenderFinsh;
	}

 	_renderTime = (_timer / timer) * 2;

	return _isRenderFinsh;
}