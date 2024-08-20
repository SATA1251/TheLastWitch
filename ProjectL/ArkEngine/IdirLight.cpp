#include <DirectXMath.h>
#include "DirectionalLight.h"
#include "ResourceManager.h"
#include "LightManager.h"
#include "ICamera.h"
#include "IdirLight.h"

ArkEngine::IdirLight::IdirLight(unsigned int lightIndex)
	: _index(lightIndex), _activeIndex(-1), _isActive(false)
{
	SetActive(true);
}

ArkEngine::IdirLight::~IdirLight()
{

}

void ArkEngine::IdirLight::Delete()
{
	if (ResourceManager::GetInstance()->GetShadowCamera().size() > _index)
	{
		ResourceManager::GetInstance()->GetShadowCamera()[_index]->Delete();
	}

	auto& iLightList = ArkEngine::LightManager::GetInstance()->GetIDirLightList();

	if (_isActive == true)
	{
		auto& activeLightList = ArkEngine::LightManager::GetInstance()->GetActiveDirLightList();

		activeLightList.erase(activeLightList.begin() + _activeIndex);

		for (const auto& index : iLightList)
		{
			if (index->GetActiveIndex() > _activeIndex)
			{
				index->MinusActiveIndex();
			}
		}

		_activeIndex = -1;
	}

	auto& lightList = ArkEngine::LightManager::GetInstance()->GetAllDirLightList();
	
	lightList.erase(lightList.begin() + _index);

	for (int i = _index+1; i < iLightList.size(); i++)
	{
		iLightList[i]->MinusIndex();
	}

	iLightList.erase(iLightList.begin() + _index);

	delete this;
}

void ArkEngine::IdirLight::SetAmbient(DirectX::XMFLOAT4 ambient)
{
	ArkEngine::LightManager::GetInstance()->GetAllDirLightList()[_index].SetAmbient(ambient);
	
	if (_activeIndex > -1)
	{
		ArkEngine::LightManager::GetInstance()->GetActiveDirLightList()[_activeIndex].SetAmbient(ambient);
	}
}

void ArkEngine::IdirLight::SetDiffuse(DirectX::XMFLOAT4 diffuse)
{
	ArkEngine::LightManager::GetInstance()->GetAllDirLightList()[_index].SetDiffuse(diffuse);
	
	if (_activeIndex > -1)
	{
		ArkEngine::LightManager::GetInstance()->GetActiveDirLightList()[_activeIndex].SetDiffuse(diffuse);
	}
}

void ArkEngine::IdirLight::SetSpecular(DirectX::XMFLOAT4 specular)
{
	ArkEngine::LightManager::GetInstance()->GetAllDirLightList()[_index].SetSpecular(specular);
	
	if (_activeIndex > -1)
	{
		ArkEngine::LightManager::GetInstance()->GetActiveDirLightList()[_activeIndex].SetSpecular(specular);
	}
}

void ArkEngine::IdirLight::SetDirection(DirectX::XMFLOAT3 direction)
{
	ArkEngine::LightManager::GetInstance()->GetAllDirLightList()[_index].SetDir(direction);
	
	if (_activeIndex > -1)
	{
		ArkEngine::LightManager::GetInstance()->GetActiveDirLightList()[_activeIndex].SetDir(direction);
	}

	auto iCamera = ResourceManager::GetInstance()->GetShadowCamera().back();

	DirectX::XMFLOAT3 targetPos = { 0.0f, 0.0f, 0.0f };
	iCamera->SetCameraPos(targetPos, direction, 200.0f);
}

bool ArkEngine::IdirLight::GetActive()
{
	return _isActive;
}

void ArkEngine::IdirLight::SetActive(bool ox)
{
	if (_isActive == false && ox == true)
	{
		_isActive = ox;

		auto dirLight = ArkEngine::LightManager::GetInstance()->GetAllDirLightList()[_index];
		
		ArkEngine::LightManager::GetInstance()->GetActiveDirLightList().emplace_back(dirLight);

		_activeIndex = (unsigned int)ArkEngine::LightManager::GetInstance()->GetActiveDirLightList().size() - 1;

		ArkEngine::LightManager::GetInstance()->GetActiveIndexList().emplace_back(_index);
	}

	else if (_isActive == true && ox == false)
	{
		_isActive = ox;

		auto& activeLightList = ArkEngine::LightManager::GetInstance()->GetActiveDirLightList();

		activeLightList.erase(activeLightList.begin() + _activeIndex);

		auto& iLightList = ArkEngine::LightManager::GetInstance()->GetIDirLightList();

		for (const auto& index : iLightList)
		{
			if (index->GetActiveIndex() > _activeIndex)
			{
				index->MinusActiveIndex();
			}
		}

		_activeIndex = -1;

		auto& vec = ArkEngine::LightManager::GetInstance()->GetActiveIndexList();

		for (int i = 0; i < vec.size(); i++)
		{
			if (vec[i] == _index)
			{
				vec.erase(vec.begin() + i);
				break;
			}
		}
	}
}

int ArkEngine::IdirLight::GetIndex()
{
	return _index;
}

int ArkEngine::IdirLight::GetActiveIndex()
{
	return _activeIndex;
}

void ArkEngine::IdirLight::MinusIndex()
{
	_index--;
}

void ArkEngine::IdirLight::MinusActiveIndex()
{
	_activeIndex--;
}