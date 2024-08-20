#include <DirectXMath.h>
#include "LightManager.h"
#include "PointLight.h"
#include "DebugObject.h"
#include "IPointLight.h"

ArkEngine::IPointLight::IPointLight(unsigned int lightIndex)
	: _index(lightIndex), _activeIndex(-1), _isActive(false), _debugObject(nullptr)
{
	SetActive(true);

	std::string lightName = "PointLight";
	//_debugObject = new ArkEngine::ArkDX11::DebugObject(lightName, 0, 2);
}

ArkEngine::IPointLight::~IPointLight()
{
	if (_debugObject != nullptr)
	{
		delete _debugObject;
	}
}

void ArkEngine::IPointLight::Delete()
{
	auto& iLightList = ArkEngine::LightManager::GetInstance()->GetIPointLightList();

	if (_isActive == true)
	{
		auto& activeLightList = ArkEngine::LightManager::GetInstance()->GetActivePointLightList();

		activeLightList.erase(activeLightList.begin() + _activeIndex);

		for (auto index : iLightList)
		{
			if (index->GetActiveIndex() > _activeIndex)
			{
				index->MinusActiveIndex();
			}
		}

		_activeIndex = -1;
	}

	auto& lightList = ArkEngine::LightManager::GetInstance()->GetAllPointLightList();

	lightList.erase(lightList.begin() + _index);

	for (int i = _index + 1; i < iLightList.size(); i++)
	{
		iLightList[i]->MinusIndex();
	}

	iLightList.erase(iLightList.begin() + _index);

	delete this;
}

void ArkEngine::IPointLight::SetAmbient(DirectX::XMFLOAT4 ambient)
{
	ArkEngine::LightManager::GetInstance()->GetAllPointLightList()[_index].SetAmbient(ambient);

	if (_activeIndex > -1)
	{
		ArkEngine::LightManager::GetInstance()->GetActivePointLightList()[_activeIndex].SetAmbient(ambient);
	}
}

void ArkEngine::IPointLight::SetDiffuse(DirectX::XMFLOAT4 diffuse)
{
	ArkEngine::LightManager::GetInstance()->GetAllPointLightList()[_index].SetDiffuse(diffuse);
	
	if (_activeIndex > -1)
	{
		ArkEngine::LightManager::GetInstance()->GetActivePointLightList()[_activeIndex].SetDiffuse(diffuse);
	}
}

void ArkEngine::IPointLight::SetSpecular(DirectX::XMFLOAT4 specular)
{
	ArkEngine::LightManager::GetInstance()->GetAllPointLightList()[_index].SetSpecular(specular);
	
	if (_activeIndex > -1)
	{
		ArkEngine::LightManager::GetInstance()->GetActivePointLightList()[_activeIndex].SetSpecular(specular);
	}
}

void ArkEngine::IPointLight::SetPosition(DirectX::XMFLOAT3 position)
{
	ArkEngine::LightManager::GetInstance()->GetAllPointLightList()[_index].SetPosition(position);
	
	if (_activeIndex > -1)
	{
		ArkEngine::LightManager::GetInstance()->GetActivePointLightList()[_activeIndex].SetPosition(position);
	}
	if (_debugObject != nullptr)
	{
		_debugObject->SetPosition(position.x, position.y, position.z);
	}
}

void ArkEngine::IPointLight::SetRange(float range)
{
	float radius = range / 2;

	ArkEngine::LightManager::GetInstance()->GetAllPointLightList()[_index].SetRange(radius);

	if (_activeIndex > -1)
	{
		ArkEngine::LightManager::GetInstance()->GetActivePointLightList()[_activeIndex].SetRange(radius);
	}
	if (_debugObject != nullptr)
	{
		_debugObject->SetScale(radius, radius, radius);
	}
}

void ArkEngine::IPointLight::SetAttenuation(float att)
{
	ArkEngine::LightManager::GetInstance()->GetAllPointLightList()[_index].SetAttenuation(att);

	if (_activeIndex > -1)
	{
		ArkEngine::LightManager::GetInstance()->GetActivePointLightList()[_activeIndex].SetAttenuation(att);
	}
}

bool ArkEngine::IPointLight::GetActive()
{
	return _isActive;
}

void ArkEngine::IPointLight::SetActive(bool ox)
{
	if (_isActive == false && ox == true)
	{
		_isActive = ox;

		auto pointLight = ArkEngine::LightManager::GetInstance()->GetAllPointLightList()[_index];

		ArkEngine::LightManager::GetInstance()->GetActivePointLightList().emplace_back(pointLight);

		_activeIndex = (unsigned int)ArkEngine::LightManager::GetInstance()->GetActivePointLightList().size() - 1;
	}

	else if (_isActive == true && ox == false)
	{
		_isActive = ox;

		auto& activeLightList = ArkEngine::LightManager::GetInstance()->GetActivePointLightList();

		activeLightList.erase(activeLightList.begin() + _activeIndex);

		auto& iLightList = ArkEngine::LightManager::GetInstance()->GetIPointLightList();

		for (auto index : iLightList)
		{
			if (index->GetActiveIndex() > _activeIndex)
			{
				index->MinusActiveIndex();
			}
		}

		_activeIndex = -1;
	}
}

int ArkEngine::IPointLight::GetActiveIndex()
{
	return _activeIndex;
}

void ArkEngine::IPointLight::MinusIndex()
{
	_index--;
}

void ArkEngine::IPointLight::MinusActiveIndex()
{
	_activeIndex--;
}
