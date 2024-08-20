#include <d3d11.h>
#include "ResourceManager.h"
#include "ArkDevice.h"
#include "ShadowBuffer.h"

ArkEngine::ArkDX11::ShadowBuffer::ShadowBuffer(int width, int height)
	:_textureWidth(width), _textureHeight(height), _depthTexture(nullptr), _depthStencilView(nullptr), _shaderResourceView(nullptr)
{
	for (int i = 0; i < 4; i++)
	{
		_color[i] = 1.0f;
	}

	Initialize();
}

ArkEngine::ArkDX11::ShadowBuffer::~ShadowBuffer()
{
	Finalize();
}

void ArkEngine::ArkDX11::ShadowBuffer::Initialize()
{
	auto arkDevice = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkDevice>("Device");

	CreateDepthTexture(arkDevice);
	CreateDepthStencilView(arkDevice);
	CreateShaderResourceView(arkDevice);
}

void ArkEngine::ArkDX11::ShadowBuffer::Finalize()
{
	_shaderResourceView->Release();
	_depthStencilView->Release();
	_depthTexture->Release();
}

void ArkEngine::ArkDX11::ShadowBuffer::SetDepthStencilView()
{
	auto arkDevice = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkDevice>("Device");
	
	arkDevice->GetDeviceContext()->OMSetRenderTargets(0, nullptr, _depthStencilView);

	arkDevice->GetDeviceContext()->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0F, 0);
}

void ArkEngine::ArkDX11::ShadowBuffer::ClearDepthStencilView()
{
	auto arkDevice = ResourceManager::GetInstance()->GetResource<ArkEngine::ArkDX11::ArkDevice>("Device");

	arkDevice->GetDeviceContext()->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0F, 0);
}

ID3D11ShaderResourceView* ArkEngine::ArkDX11::ShadowBuffer::GetDepthSRV()
{
	return _shaderResourceView;
}

ID3D11DepthStencilView* ArkEngine::ArkDX11::ShadowBuffer::GetDepthStencilView()
{
	return _depthStencilView;
}

ID3D11Texture2D* ArkEngine::ArkDX11::ShadowBuffer::GetTexture2D()
{
	return _depthTexture;
}

void ArkEngine::ArkDX11::ShadowBuffer::CreateDepthTexture(ArkEngine::ArkDX11::ArkDevice* pDeivce)
{
	D3D11_TEXTURE2D_DESC textureDesc;

	textureDesc.Height = _textureHeight;
	textureDesc.Width = _textureWidth;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	HRESULT result = pDeivce->GetDevice()->CreateTexture2D(&textureDesc, NULL, &_depthTexture);
}

void ArkEngine::ArkDX11::ShadowBuffer::CreateDepthStencilView(ArkEngine::ArkDX11::ArkDevice* pDeivce)
{
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	depthStencilViewDesc.Flags = 0;

	HRESULT result = pDeivce->GetDevice()->CreateDepthStencilView(_depthTexture, &depthStencilViewDesc, &_depthStencilView);
}

void ArkEngine::ArkDX11::ShadowBuffer::CreateShaderResourceView(ArkEngine::ArkDX11::ArkDevice* pDeivce)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

	HRESULT result = pDeivce->GetDevice()->CreateShaderResourceView(_depthTexture, &shaderResourceViewDesc, &_shaderResourceView);
}
