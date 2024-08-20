#include <DirectXMath.h>
#include "BasicLight.h"

ArkEngine::ArkDX11::BasicLight::BasicLight(DirectX::XMFLOAT4 ambient, DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 specular)
	: _ambient(ambient), _diffuse (diffuse), _specular(specular)
{

}

void ArkEngine::ArkDX11::BasicLight::SetAmbient(DirectX::XMFLOAT4 ambient)
{
	_ambient = ambient;
}

void ArkEngine::ArkDX11::BasicLight::SetDiffuse(DirectX::XMFLOAT4 diffuse)
{
	_diffuse = diffuse;
}

void ArkEngine::ArkDX11::BasicLight::SetSpecular(DirectX::XMFLOAT4 specular)
{
	_specular = specular;
}


