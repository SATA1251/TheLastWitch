#include <DirectXMath.h>
#include "ResourceManager.h"
#include "CommonStruct.h"
#include "ArkBuffer.h"
#include "ArkDevice.h"
#include "ArkEffect.h"
#include "ArkTexture.h"
#include "d3dx11effect.h"
#include "Camera.h"
#include "GeometryGenerator.h"
#include "CubeMap.h"

ArkEngine::ArkDX11::CubeMap::CubeMap(const std::string& fileName, const std::string& textureName, bool isCube)
	: _fileName(fileName), _textureName("Resources/Textures/CubeMaps/"+textureName), _effectName("Resources/FX/Sky.fx"), _effect(nullptr), _tech(nullptr),
	_fxWorldViewProj(nullptr), _cubeMap(nullptr), _cubeMapSRV(nullptr),
	_vertexBuffer(nullptr), _indexBuffer(nullptr), _world(), _view(), _proj(),
	_arkDevice(nullptr), _arkEffect(nullptr), _totalIndexCount(0), _isCube(isCube)
{
	Initialize();
}

ArkEngine::ArkDX11::CubeMap::~CubeMap()
{
	Finailize();
}

void ArkEngine::ArkDX11::CubeMap::Initialize()
{
	BuildGeometryBuffers();

	_arkEffect = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkEffect>(_effectName);
	_effect = _arkEffect->GetEffect();

	_arkDevice = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkDevice>("Device");

	SetEffect();
	SetTexture(_textureName.c_str());
}

void ArkEngine::ArkDX11::CubeMap::Update(ArkEngine::ICamera* p_Camera)
{
	DirectX::XMStoreFloat4x4(&_world, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&_view, p_Camera->GetViewMatrix());
	DirectX::XMStoreFloat4x4(&_proj, p_Camera->GetProjMatrix());
}

void ArkEngine::ArkDX11::CubeMap::Render()
{
	auto deviceContext = _arkDevice->GetDeviceContext();

	deviceContext->IASetInputLayout(_arkEffect->GetInputLayOut());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->RSSetState(_arkDevice->GetSolidRS());

	UINT stride = sizeof(ArkEngine::ArkDX11::Vertex);
	UINT offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	_cubeMap->SetResource(_cubeMapSRV);

	D3DX11_TECHNIQUE_DESC techDesc;
	_tech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		DirectX::XMMATRIX world = XMLoadFloat4x4(&_world);
		DirectX::XMMATRIX view = XMLoadFloat4x4(&_view);
		DirectX::XMMATRIX proj = XMLoadFloat4x4(&_proj);
		DirectX::XMMATRIX WorldViewProj = world * view * proj;
		_fxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&WorldViewProj));

		_tech->GetPassByIndex(p)->Apply(0, deviceContext);
		deviceContext->DrawIndexed(_totalIndexCount, 0, 0);
	}
}

const std::string& ArkEngine::ArkDX11::CubeMap::GetName()
{
	return _fileName;
}

const std::string& ArkEngine::ArkDX11::CubeMap::GetTextureName()
{
	return _textureName;
}

void ArkEngine::ArkDX11::CubeMap::Finailize()
{
	_tech = nullptr;
	_effect = nullptr;

	_vertexBuffer = nullptr;
	_indexBuffer = nullptr;

	_cubeMapSRV = nullptr;
	_cubeMap->Release();

	_fxWorldViewProj->Release();

	_arkEffect = nullptr;
	_arkDevice = nullptr;
}

void ArkEngine::ArkDX11::CubeMap::BuildGeometryBuffers()
{
	auto buffer = ResourceManager::GetInstance()->GetResource<ArkBuffer>(_fileName);

	if (buffer == nullptr)
	{
		GeometryGenerator generator;
		if (_isCube)
		{
			generator.CreateBox(_fileName.c_str(), 5000.0f, 5000.0f, 5000.0f);
		}
		else
		{
			generator.CreateSphere(_fileName.c_str(), 2000.0f, 30, 30);
		}

		buffer = ResourceManager::GetInstance()->GetResource<ArkBuffer>(_fileName);
	}

	_vertexBuffer = buffer->GetVertexBuffer();
	_indexBuffer = buffer->GetIndexBuffer();

	_totalIndexCount = buffer->GetTotalIndexCount();
}

void ArkEngine::ArkDX11::CubeMap::SetEffect()
{
	_tech = _effect->GetTechniqueByName("SkyTech");

	_fxWorldViewProj = _effect->GetVariableByName("gWorldViewProj")->AsMatrix();

	_cubeMap = _effect->GetVariableByName("gCubeMap")->AsShaderResource();
}

void ArkEngine::ArkDX11::CubeMap::SetTexture(const char* textureName)
{
	auto texture = ResourceManager::GetInstance()->GetResource<ArkTexture>(textureName);

	if (texture != nullptr)
	{
		_cubeMapSRV = texture->GetDiffuseMapSRV();
	}

	// 텍스쳐 파일이 없을 경우 mokoko 텍스쳐 출력
	else if (texture == nullptr)
	{
		ArkTexture* newTexture = new ArkTexture(textureName);
		ResourceManager::GetInstance()->SetTextureNameList(textureName);

		_cubeMapSRV = newTexture->GetDiffuseMapSRV();
	}
}