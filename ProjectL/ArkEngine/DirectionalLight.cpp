#include <DirectXMath.h>
#include "DirectionalLight.h"

ArkEngine::ArkDX11::DirectionalLight::DirectionalLight(DirectX::XMFLOAT4 ambient, DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 specular, DirectX::XMFLOAT3 direction)
	:BasicLight(ambient, diffuse, specular), _direction(direction), _pad(0.0f)
{
	
}

ArkEngine::ArkDX11::DirectionalLight::~DirectionalLight()
{

}

void ArkEngine::ArkDX11::DirectionalLight::SetDir(DirectX::XMFLOAT3 direction)
{
	_direction = direction;
}

DirectX::XMFLOAT3 ArkEngine::ArkDX11::DirectionalLight::GetDir()
{
	return _direction;
}



