#include "SpotLight.h"

ArkEngine::ArkDX11::SpotLight::SpotLight(DirectX::XMFLOAT4 ambient, DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 specular, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction)
	:BasicLight(ambient, diffuse, specular), _position(position), _direction(direction), _pad(0.0f)
{

}

ArkEngine::ArkDX11::SpotLight::~SpotLight()
{

}

void ArkEngine::ArkDX11::SpotLight::SetPosition(DirectX::XMFLOAT3 position)
{
	_position = position;
}

void ArkEngine::ArkDX11::SpotLight::SetDir(DirectX::XMFLOAT3 direction)
{
	_direction = direction;
}

void ArkEngine::ArkDX11::SpotLight::SetRange(float range)
{
	_range = range;
}
