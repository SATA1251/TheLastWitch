#include "Light.h"
#include "Transform.h"
#include "GameObject.h"
#include "ToolBox.h"

KunrealEngine::Light::Light()
	:_light(nullptr), _transform(nullptr), _type(LightType::None),
	_ambient(0.0f, 0.0f, 0.0f, 0.0f), _diffuse(0.0f, 0.0f, 0.0f, 0.0f), _specular(0.0f, 0.0f, 0.0f, 0.0f), _direction(0.0f, 0.0f, 0.0f),
	_rangeP(0.0f), _offset(0.0f, 0.0f, 0.0f), _decomposedPos(0.0f, 0.0f, 0.0f)
{

}

KunrealEngine::Light::~Light()
{

}

void KunrealEngine::Light::Initialize()
{
	this->_transform = this->GetOwner()->GetComponent<Transform>();
}

void KunrealEngine::Light::Release()
{
	if (_light != nullptr)
	{
		_light->Delete();
	}
}

void KunrealEngine::Light::FixedUpdate()
{

}

void KunrealEngine::Light::Update()
{
	SetPointPosition();
}

void KunrealEngine::Light::LateUpdate()
{

}

void KunrealEngine::Light::OnTriggerEnter()
{

}

void KunrealEngine::Light::OnTriggerStay()
{

}

void KunrealEngine::Light::OnTriggerExit()
{

}

void KunrealEngine::Light::SetActive(bool active)
{
	if (_light != nullptr)
	{
		_light->SetActive(active);
	}

	this->_isActivated = active;
}

KunrealEngine::LightType KunrealEngine::Light::GetLightType()
{
	if (_light != nullptr)
	{
		return this->_type;
	}
}

bool KunrealEngine::Light::GetLightStatus()
{
	if (_light == nullptr)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void KunrealEngine::Light::SetLightState(bool flag)
{
	if (_light != nullptr)
	{
		_light->SetActive(flag);
	}
}

void KunrealEngine::Light::CreateDirectionalLight(DirectX::XMFLOAT4 ambient /*= { 0.f, 0.f, 0.f, 0.f }*/, DirectX::XMFLOAT4 diffuse /*= { 0.f, 0.f, 0.f, 0.f }*/, DirectX::XMFLOAT4 specular /*= { 0.f, 0.f, 0.f, 0.f }*/, DirectX::XMFLOAT3 direction /*= { 0.f, 0.f, 0.f }*/)
{
	if (_light != nullptr)
	{
		_light->Delete();
	}

	//ambient.x = diffuse.x * 0.2f;
	//ambient.y = diffuse.y * 0.2f;
	//ambient.z = diffuse.z * 0.2f;
	//ambient.w = diffuse.w * 0.2f;

	_light = GRAPHICS->CreateDirectionalLight(ambient, diffuse, specular, direction);

	_ambient = ambient;
	_diffuse = diffuse;
	_specular = specular;
	_direction = direction;

	_type = LightType::DirectionalLight;
}

void KunrealEngine::Light::SetDirection(float x, float y, float z)
{
	if (_light != nullptr && _type == LightType::DirectionalLight)
	{
		DirectX::XMFLOAT3 direction(x, y, z);

		auto _castedLight = dynamic_cast<GInterface::GraphicsDirLight*>(_light);
		_castedLight->SetDirection(direction);

		_direction = direction;
	}
}

DirectX::XMFLOAT3 KunrealEngine::Light::GetDirection()
{
	if (_light != nullptr)
	{
		return this->_direction;
	}
}

void KunrealEngine::Light::SetAmbient(float x, float y, float z, float w)
{
	if (_light == nullptr)
	{
		return;
	}

	DirectX::XMFLOAT4 ambient(x, y, z, w);

	if (_type == LightType::DirectionalLight)
	{
		auto _castedLight = dynamic_cast<GInterface::GraphicsDirLight*>(_light);
		_castedLight->SetAmbient(ambient);
	}
	else if (_type == LightType::PointLight)
	{
		auto _castedLight = dynamic_cast<GInterface::GraphicsPointLight*>(_light);
		_castedLight->SetAmbient(ambient);
	}

	_ambient = ambient;
}

DirectX::XMFLOAT4 KunrealEngine::Light::GetAmbient()
{
	if (_light != nullptr)
	{
		return this->_ambient;
	}
}

void KunrealEngine::Light::SetDiffuse(float x, float y, float z, float w)
{
	if (_light == nullptr)
	{
		return;
	}

	DirectX::XMFLOAT4 diffuse(x, y, z, w);

	if (_type == LightType::DirectionalLight)
	{
		auto _castedLight = dynamic_cast<GInterface::GraphicsDirLight*>(_light);
		_castedLight->SetDiffuse(diffuse);
	}
	else if (_type == LightType::PointLight)
	{
		auto _castedLight = dynamic_cast<GInterface::GraphicsPointLight*>(_light);
		_castedLight->SetDiffuse(diffuse);
	}

	_diffuse = diffuse;
}

DirectX::XMFLOAT4 KunrealEngine::Light::GetDiffuse()
{
	if (_light != nullptr)
	{
		return this->_diffuse;
	}
}

void KunrealEngine::Light::SetSpecular(float x, float y, float z, float w)
{
	if (_light == nullptr)
	{
		return;
	}

	DirectX::XMFLOAT4 specular(x, y, z, w);

	if (_type == LightType::DirectionalLight)
	{
		auto _castedLight = dynamic_cast<GInterface::GraphicsDirLight*>(_light);
		_castedLight->SetSpecular(specular);
	}
	else if (_type == LightType::PointLight)
	{
		auto _castedLight = dynamic_cast<GInterface::GraphicsPointLight*>(_light);
		_castedLight->SetSpecular(specular);
	}

	_specular = specular;
}

DirectX::XMFLOAT4 KunrealEngine::Light::GetSpecular()
{
	if (_light != nullptr)
	{
		return this->_specular;
	}
}


void KunrealEngine::Light::SetOffSet(float x, float y, float z)
{
	this->_offset.x = x;
	this->_offset.y = y;
	this->_offset.z = z;
}


DirectX::XMFLOAT3 KunrealEngine::Light::GetOffSet()
{
	return this->_offset;
}

void KunrealEngine::Light::CreatePointLight(DirectX::XMFLOAT4 ambient /*= {0.f, 0.f, 0.f, 0.f}*/, DirectX::XMFLOAT4 diffuse /*= { 0.f, 0.f, 0.f, 0.f }*/, DirectX::XMFLOAT4 specular /*= { 0.f, 0.f, 0.f, 0.f }*/, float range /*= 0.f*/, float attenuation)
{
	if (_light != nullptr)
	{
		_light->Delete();
	}

	_light = GRAPHICS->CreatePointLight(ambient, diffuse, specular, _transform->GetPosition(), range, attenuation);

	_ambient = ambient;
	_diffuse = diffuse;
	_specular = specular;
	_rangeP = range;

	_type = LightType::PointLight;
}

void KunrealEngine::Light::SetPointPosition()
{
	if (_light != nullptr && _type == LightType::PointLight)
	{
		GInterface::GraphicsPointLight* _castedLight = dynamic_cast<GInterface::GraphicsPointLight*>(_light);

		// 부모가 있으면
		if (this->GetOwner()->GetParent() != nullptr)
		{
			DirectX::XMFLOAT4X4 worldTM = this->GetOwner()->GetComponent<Transform>()->GetWorldTM();
			DirectX::XMMATRIX worldMat = DirectX::XMLoadFloat4x4(&worldTM);

			DirectX::XMVECTOR scale, quaternion, translation;

			DirectX::XMMatrixDecompose(&scale, &quaternion, &translation, worldMat);
			DirectX::XMStoreFloat3(&_decomposedPos, translation);;

			DirectX::XMFLOAT3 newPointPos(this->_decomposedPos.x + _offset.x, this->_decomposedPos.y + _offset.y, this->_decomposedPos.z + _offset.z);

			_castedLight->SetPosition(newPointPos);
		}
		// 없으면
		else
		{
			DirectX::XMFLOAT3 newPointPos(this->_transform->GetPosition().x + _offset.x, this->_transform->GetPosition().y + _offset.y, this->_transform->GetPosition().z + _offset.z);

			_castedLight->SetPosition(newPointPos);
		}
	}
}

void KunrealEngine::Light::SetPointRange(float range)
{
	if (_light != nullptr && _type == LightType::PointLight)
	{
		GInterface::GraphicsPointLight* _castedLight = dynamic_cast<GInterface::GraphicsPointLight*>(_light);

		_castedLight->SetRange(range);

		_rangeP = range;
	}
}

float KunrealEngine::Light::GetPointRange()
{
	if (_light != nullptr)
	{
		return this->_rangeP;
	}
}

DirectX::XMFLOAT3 KunrealEngine::Light::GetPointPos()
{
	return _decomposedPos;
}

DirectX::XMFLOAT3 KunrealEngine::Light::ChangeDirectionForPattern(DirectX::XMFLOAT3 pointPos)
{
	DirectX::XMFLOAT3 newDirection = { pointPos };
	
	DirectX::XMVECTOR dirVec = XMLoadFloat3(&newDirection);
	dirVec = DirectX::XMVector3Normalize(dirVec);
	XMStoreFloat3(&newDirection, dirVec);

	newDirection.x = newDirection.x * -1;
	newDirection.z = newDirection.z * -1;

	return newDirection;
}

DirectX::XMFLOAT3 KunrealEngine::Light::ResetDirection(DirectX::XMFLOAT3 currentDirection, float deltaTime, float speed)
{
	DirectX::XMFLOAT3 targetDirection = { -1.0f, -1.0f, 1.0f };

	currentDirection.x += (targetDirection.x - currentDirection.x) * deltaTime * speed;
	currentDirection.z += (targetDirection.z - currentDirection.z) * deltaTime * speed;

	return currentDirection;
}
