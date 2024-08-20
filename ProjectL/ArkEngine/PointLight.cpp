#include <DirectXMath.h>
#include <string>
#include "PointLight.h"

ArkEngine::ArkDX11::PointLight::PointLight(DirectX::XMFLOAT4 ambient, DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 specular, DirectX::XMFLOAT3 position, float radius, float att)
	:_ambient(ambient), _diffuse(diffuse), _specular(specular), _position(position), _radius(radius),  _attenuation(att)
{
	_att = DirectX::XMFLOAT3(1.0f, 0.1f, 0.01f);

	SetPosition(position);
	//SetAttenuation(_attenuation);
}

ArkEngine::ArkDX11::PointLight::~PointLight()
{
	
}


void ArkEngine::ArkDX11::PointLight::SetAmbient(DirectX::XMFLOAT4& ambient)
{
	_ambient = ambient;
}


void ArkEngine::ArkDX11::PointLight::SetDiffuse(DirectX::XMFLOAT4& diffuse)
{
	_diffuse = diffuse;
}


void ArkEngine::ArkDX11::PointLight::SetSpecular(DirectX::XMFLOAT4& specular)
{
	_specular = specular;
}

void ArkEngine::ArkDX11::PointLight::SetPosition(DirectX::XMFLOAT3 position)
{
	_position = position;
}

void ArkEngine::ArkDX11::PointLight::SetRange(float radius)
{
	_radius = radius;
}

void ArkEngine::ArkDX11::PointLight::SetAttenuation(float attenuation)
{
	_attenuation = attenuation;
}
