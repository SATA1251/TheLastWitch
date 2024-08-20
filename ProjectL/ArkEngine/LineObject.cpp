#include <DirectXMath.h>
#include "d3dx11effect.h"
#include "CommonStruct.h"
#include "ResourceManager.h"
#include "ICamera.h"
#include "ArkEffect.h"
#include "ArkDevice.h"
#include "LineObject.h"

ArkEngine::ArkDX11::LineObject::LineObject()
	: _vertex1(), _vertex2(), _color(),
	_arkEffect(nullptr), _effect(nullptr), _arkDevice(nullptr),
	_effectName("Resources/FX/color.fx"), _world(), _view(), _proj(), _vertexBuffer(nullptr), _indexBuffer(nullptr),
	_tech(nullptr), _fxWorld(nullptr), _fxWorldViewProj(nullptr)
{

}

ArkEngine::ArkDX11::LineObject::~LineObject()
{
	Release();
}

void ArkEngine::ArkDX11::LineObject::Initialize(const DirectX::XMFLOAT3& vertex1, const DirectX::XMFLOAT3& vertex2, const DirectX::XMFLOAT4& color)
{
	_vertex1 = vertex1;
	_vertex2 = vertex2;
	_color = color;

	_arkEffect = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkEffect>(_effectName);
	_effect = _arkEffect->GetEffect();

	_arkDevice = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkDevice>("Device");

	BuildGeometryBuffers();
	SetEffect();
}

void ArkEngine::ArkDX11::LineObject::Update(ArkEngine::ICamera* p_Camera)
{
	DirectX::XMStoreFloat4x4(&_world, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&_view, p_Camera->GetViewMatrix());
	DirectX::XMStoreFloat4x4(&_proj, p_Camera->GetProjMatrix());
}

void ArkEngine::ArkDX11::LineObject::Render()
{
	auto deviceContext = _arkDevice->GetDeviceContext();

	deviceContext->IASetInputLayout(_arkEffect->GetInputLayOut());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	deviceContext->RSSetState(_arkDevice->GetWireRS());

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
		deviceContext->DrawIndexed(2, 0, 0);
	}
}

void ArkEngine::ArkDX11::LineObject::Release()
{
	_indexBuffer->Release();
	_vertexBuffer->Release();

	_fxWorldViewProj->Release();
	_fxWorld->Release();
	_tech->Release();
}

const DirectX::XMFLOAT3& ArkEngine::ArkDX11::LineObject::GetVertex1()
{
	return _vertex1;
}

const DirectX::XMFLOAT3& ArkEngine::ArkDX11::LineObject::GetVertex2()
{
	return _vertex2;
}

void ArkEngine::ArkDX11::LineObject::BuildGeometryBuffers()
{
	std::vector<PosColor> vertexList;
	
	PosColor v1;
	v1.pos = _vertex1;
	v1.color = _color;

	vertexList.emplace_back(v1);

	PosColor v2;
	v2.pos = _vertex2;
	v2.color = _color;

	vertexList.emplace_back(v2);

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(ArkEngine::ArkDX11::PosColor) * vertexList.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertexList[0];

	_arkDevice->GetDevice()->CreateBuffer(&vertexBufferDesc, &vinitData, &_vertexBuffer);

	
	std::vector<unsigned int> indexList;

	indexList.emplace_back(0);
	indexList.emplace_back(1);

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof(UINT) * 2;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indexList[0];

	_arkDevice->GetDevice()->CreateBuffer(&indexBufferDesc, &iinitData, &_indexBuffer);
}

void ArkEngine::ArkDX11::LineObject::SetEffect()
{
	_tech = _effect->GetTechniqueByIndex(0);
	_fxWorld = _effect->GetVariableByName("gWorld")->AsMatrix();
	_fxWorldViewProj = _effect->GetVariableByName("gWorldViewProj")->AsMatrix();
}