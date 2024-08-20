#include "ResourceManager.h"
#include "ArkDevice.h"

ArkEngine::ArkDX11::ArkDevice::ArkDevice(ID3D11Device* p_device, ID3D11DeviceContext* p_deviceContext, ID3D11DepthStencilView* p_depthStencilView, ID3D11RasterizerState* p_SolidRS, ID3D11RasterizerState* P_WireRS, ID3D11RasterizerState* P_ShadowRS)
	: _device(p_device), _deviceContext(p_deviceContext), _depthStencilView(p_depthStencilView), _solidRS(p_SolidRS), _wireRS(P_WireRS), _shadowRS(P_ShadowRS), _randomTexSRV(nullptr)
{

}

ArkEngine::ArkDX11::ArkDevice::~ArkDevice()
{
	_randomTexSRV->Release();

	_shadowRS = nullptr;
	_solidRS = nullptr;
	_wireRS = nullptr;
	_depthStencilView = nullptr;
	_deviceContext = nullptr;
	_device = nullptr;
}

void ArkEngine::ArkDX11::ArkDevice::Load(const char* fileName)
{
	ArkEngine::ResourceManager::GetInstance()->SetResource(fileName, this);
}

void ArkEngine::ArkDX11::ArkDevice::UnLoad(const char* fileName)
{
	ArkEngine::ResourceManager::GetInstance()->DeleteResource(fileName);
}

ID3D11Device* ArkEngine::ArkDX11::ArkDevice::GetDevice()
{
	return _device;
}

ID3D11DeviceContext* ArkEngine::ArkDX11::ArkDevice::GetDeviceContext()
{
	return _deviceContext;
}

ID3D11DepthStencilView* ArkEngine::ArkDX11::ArkDevice::GetDepthStencilView()
{
	return _depthStencilView;
}

ID3D11RasterizerState* ArkEngine::ArkDX11::ArkDevice::GetSolidRS()
{
	return _solidRS;
}

ID3D11RasterizerState* ArkEngine::ArkDX11::ArkDevice::GetWireRS()
{
	return _wireRS;
}

ID3D11RasterizerState* ArkEngine::ArkDX11::ArkDevice::GetShadowRS()
{
	return _shadowRS;
}


ID3D11ShaderResourceView* ArkEngine::ArkDX11::ArkDevice::GetRandomTex()
{
	return _randomTexSRV;
}

void ArkEngine::ArkDX11::ArkDevice::SetRandomTex(ID3D11ShaderResourceView* randomTex)
{
	_randomTexSRV = randomTex;
}
